#include "imap_client.h"
#include <QDebug>
#include <QRegularExpression>
#include <QEventLoop>
#include <QTimer>

ImapClient::ImapClient(QObject* parent)
    : QObject(parent)
    , m_socket(new QSslSocket(this))
    , m_state(Disconnected)
    , m_tagCounter(0)
    , m_port(993)
    , m_useSSL(true)
{
    // Defensive: ensure m_socket is valid
#ifdef QT_DEBUG
    if (!m_socket) {
        qDebug() << "ImapClient: m_socket is null when connecting signals";
    }
#endif
    connect(m_socket, &QSslSocket::connected, this, &ImapClient::onSocketConnected);
    connect(m_socket, &QSslSocket::disconnected, this, &ImapClient::onSocketDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred),
            this, &ImapClient::onSocketError);
    connect(m_socket, &QSslSocket::sslErrors, this, &ImapClient::onSslErrors);
    connect(m_socket, &QSslSocket::readyRead, this, &ImapClient::onReadyRead);
}

ImapClient::~ImapClient() {
    disconnectFromServer();
}

void ImapClient::connectToServer(const Settings& settings) {
    if (m_state != Disconnected) {
        return;
    }

    m_server = settings.imapServer();
    m_port = settings.imapPort();
    m_useSSL = settings.useSSL();
    
    if (m_server.isEmpty()) {
        m_lastError = "No IMAP server configured";
        m_state = Error;
        emit error(m_lastError);
        return;
    }

    m_state = Connecting;
    m_lastError.clear();
    
    if (m_useSSL) {
        m_socket->connectToHostEncrypted(m_server, m_port);
    } else {
        m_socket->connectToHost(m_server, m_port);
    }
}

void ImapClient::disconnectFromServer() {
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        sendCommand("LOGOUT");
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(3000);
        }
    }
    m_state = Disconnected;
    m_currentMailbox.clear();
}

ImapClient::State ImapClient::state() const {
    return m_state;
}

QString ImapClient::lastError() const {
    return m_lastError;
}

void ImapClient::authenticate(const QString& username, const QString& password) {
    if (m_state != Connected) {
        m_lastError = "Not connected to server";
        emit error(m_lastError);
        return;
    }

    if (loginCommand(username, password)) {
        m_state = Authenticated;
        emit authenticated();
    } else {
        m_state = Error;
        emit error(m_lastError);
    }
}

QStringList ImapClient::listMailboxes() {
    if (m_state != Authenticated && m_state != Selected) {
        qDebug() << "IMAP LIST: Not authenticated or selected.";
        return QStringList();
    }
    QStringList mailboxes = listCommand();
    qDebug() << "IMAP LIST: Top-level mailboxes:" << mailboxes;
    // Recursively traverse mailboxes
    QSet<QString> allMailboxes;
    std::function<void(const QString&)> traverse;
    traverse = [&](const QString& parent) {
        QString tag = generateTag();
        QString command = QString("%1 LIST \"%2\" \"*\"").arg(tag, parent);
        sendCommand(command);
        QStringList responses = readMultilineResponse();
        for (const QString& response : responses) {
            if (response.startsWith("*") && response.contains("LIST")) {
                QRegularExpression re("\\* LIST \\([^)]*\\) \"[^\"]*\" \"([^\"]+)\"");
                QRegularExpressionMatch match = re.match(response);
                if (match.hasMatch()) {
                    QString mailbox = match.captured(1);
                    if (!allMailboxes.contains(mailbox)) {
                        allMailboxes.insert(mailbox);
                        qDebug() << "IMAP RECURSIVE MAILBOX:" << mailbox;
                        // Traverse children if not already traversed
                        traverse(mailbox);
                    }
                }
            }
        }
    };
    for (const QString& mailbox : mailboxes) {
        allMailboxes.insert(mailbox);
        traverse(mailbox);
    }
    qDebug() << "IMAP ALL MAILBOXES:" << allMailboxes.values();
    return allMailboxes.values();
}

bool ImapClient::selectMailbox(const QString& mailbox) {
    if (m_state != Authenticated && m_state != Selected) {
        return false;
    }
    
    if (selectCommand(mailbox)) {
        m_currentMailbox = mailbox;
        m_state = Selected;
        emit mailboxSelected(mailbox);
        return true;
    }
    return false;
}

QString ImapClient::currentMailbox() const {
    return m_currentMailbox;
}

QList<EmailCard> ImapClient::fetchCards(const QString& mailbox) {
    QString targetMailbox = mailbox.isEmpty() ? m_currentMailbox : mailbox;
    
    if (!targetMailbox.isEmpty() && targetMailbox != m_currentMailbox) {
        if (!selectMailbox(targetMailbox)) {
            return QList<EmailCard>();
        }
    }
    
    if (m_state != Selected) {
        return QList<EmailCard>();
    }
    
    return fetchCommand();
}

EmailCard ImapClient::fetchCard(const QString& uid, const QString& mailbox) {
    QString targetMailbox = mailbox.isEmpty() ? m_currentMailbox : mailbox;
    
    if (!targetMailbox.isEmpty() && targetMailbox != m_currentMailbox) {
        if (!selectMailbox(targetMailbox)) {
            return EmailCard();
        }
    }
    
    if (m_state != Selected) {
        return EmailCard();
    }
    
    QList<EmailCard> cards = fetchCommand(uid);
    return cards.isEmpty() ? EmailCard() : cards.first();
}

bool ImapClient::moveCard(const QString& uid, const QString& fromMailbox, const QString& toMailbox) {
    if (!selectMailbox(fromMailbox)) {
        return false;
    }
    
    return moveCommand(uid, toMailbox);
}

bool ImapClient::deleteCard(const QString& uid, const QString& mailbox) {
    QString targetMailbox = mailbox.isEmpty() ? m_currentMailbox : mailbox;
    
    if (!selectMailbox(targetMailbox)) {
        return false;
    }
    
    // Mark as deleted and expunge
    if (storeCommand(uid, "\\Deleted", true)) {
        return expungeCommand();
    }
    return false;
}

bool ImapClient::markAsRead(const QString& uid, bool read, const QString& mailbox) {
    QString targetMailbox = mailbox.isEmpty() ? m_currentMailbox : mailbox;
    
    if (!selectMailbox(targetMailbox)) {
        return false;
    }
    
    return storeCommand(uid, "\\Seen", read);
}

bool ImapClient::markAsFlagged(const QString& uid, bool flagged, const QString& mailbox) {
    QString targetMailbox = mailbox.isEmpty() ? m_currentMailbox : mailbox;
    
    if (!selectMailbox(targetMailbox)) {
        return false;
    }
    
    return storeCommand(uid, "\\Flagged", flagged);
}

bool ImapClient::isConnected() const {
    return m_state == Connected || m_state == Authenticated || m_state == Selected;
}

bool ImapClient::isAuthenticated() const {
    return m_state == Authenticated || m_state == Selected;
}

void ImapClient::onSocketConnected() {
    m_state = Connected;
    
    // Read server greeting
    if (waitForResponse()) {
        QString response = readResponse();
        if (response.startsWith("* OK")) {
            emit connected();
        } else {
            m_lastError = "Server greeting failed: " + response;
            m_state = Error;
            emit error(m_lastError);
        }
    }
}

void ImapClient::onSocketDisconnected() {
    m_state = Disconnected;
    m_currentMailbox.clear();
    emit disconnected();
}

void ImapClient::onSocketError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)
    m_lastError = m_socket->errorString();
    qDebug() << "IMAP SOCKET ERROR:" << m_lastError;
    m_state = Error;
    emit this->error(m_lastError);
}

void ImapClient::onSslErrors(const QList<QSslError>& errors) {
    // For development, ignore SSL errors
    // In production, you should handle these properly
    m_socket->ignoreSslErrors();
    
    QString errorStr = "SSL Errors: ";
    for (const QSslError& error : errors) {
        errorStr += error.errorString() + "; ";
    }
    qDebug() << errorStr;
}

void ImapClient::onReadyRead() {
    m_responseBuffer += m_socket->readAll();
}

void ImapClient::sendCommand(const QString& command) {
    QString fullCommand = command + "\r\n";
    qDebug() << "IMAP SEND:" << command;
    m_socket->write(fullCommand.toUtf8());
    m_socket->flush();
}

QString ImapClient::readResponse() {
    if (!waitForResponse()) {
        qDebug() << "IMAP ERROR: No response received.";
        return QString();
    }
    int endIndex = m_responseBuffer.indexOf("\r\n");
    if (endIndex == -1) {
        qDebug() << "IMAP ERROR: Malformed response.";
        return QString();
    }
    QString response = m_responseBuffer.left(endIndex);
    m_responseBuffer.remove(0, endIndex + 2);
    qDebug() << "IMAP RECV:" << response;
    return response;
}

QStringList ImapClient::readMultilineResponse() {
    QStringList responses;
    while (true) {
        QString response = readResponse();
        if (response.isEmpty()) {
            break;
        }
        responses.append(response);
        // Check if this is the final response (tagged response)
    if (response.contains(QRegularExpression("^A\\d+ (OK|NO|BAD)"))) {
            break;
        }
    }
    qDebug() << "IMAP MULTILINE RECV:" << responses;
    return responses;
}

bool ImapClient::waitForResponse(int timeoutMs) {
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(m_socket, &QSslSocket::readyRead, &loop, &QEventLoop::quit);
    
    timer.start(timeoutMs);
    
    while (!m_responseBuffer.contains("\r\n") && timer.isActive()) {
        loop.exec();
    }
    
    return m_responseBuffer.contains("\r\n");
}

QString ImapClient::generateTag() {
    return QString("A%1").arg(++m_tagCounter, 4, 10, QChar('0'));
}

bool ImapClient::parseResponse(const QString& response, QString& tag, QString& status, QString& data) {
    QRegularExpression re("^(A\\d+)\\s+(OK|NO|BAD)\\s*(.*)$");
    QRegularExpressionMatch match = re.match(response);
    
    if (match.hasMatch()) {
        tag = match.captured(1);
        status = match.captured(2);
        data = match.captured(3);
        return true;
    }
    
    return false;
}

bool ImapClient::loginCommand(const QString& username, const QString& password) {
    QString tag = generateTag();
    QString command = QString("%1 LOGIN %2 %3").arg(tag, username, password);
    sendCommand(command);
    QString response = readResponse();
    QString responseTag, status, data;
    if (parseResponse(response, responseTag, status, data) && responseTag == tag) {
        if (status == "OK") {
            qDebug() << "IMAP LOGIN SUCCESS";
            return true;
        } else {
            m_lastError = "Login failed: " + data;
            qDebug() << "IMAP LOGIN FAILED:" << m_lastError;
        }
    } else {
        m_lastError = "Invalid login response: " + response;
        qDebug() << "IMAP LOGIN INVALID RESPONSE:" << response;
    }
    return false;
}

QStringList ImapClient::listCommand() {
    QString tag = generateTag();
    QString command = QString("%1 LIST \"\" \"*\"").arg(tag);
    
    sendCommand(command);
    
    QStringList responses = readMultilineResponse();
    QStringList mailboxes;
    
    for (const QString& response : responses) {
        if (response.startsWith("*") && response.contains("LIST")) {
            // Parse LIST response: * LIST (\HasNoChildren) "." BACKLOG
            // The mailbox name is the last word in the response
            QStringList parts = response.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 4) {
                QString mailboxName = parts.last();
                // Remove quotes if present
                if (mailboxName.startsWith('"') && mailboxName.endsWith('"')) {
                    mailboxName = mailboxName.mid(1, mailboxName.length() - 2);
                }
                mailboxes.append(mailboxName);
            }
        }
    }
    
    return mailboxes;
}

bool ImapClient::selectCommand(const QString& mailbox) {
    QString tag = generateTag();
    QString command = QString("%1 SELECT \"%2\"").arg(tag, mailbox);
    
    sendCommand(command);
    
    QStringList responses = readMultilineResponse();
    
    for (const QString& response : responses) {
        QString responseTag, status, data;
        if (parseResponse(response, responseTag, status, data) && responseTag == tag) {
            return status == "OK";
        }
    }
    
    return false;
}

QList<EmailCard> ImapClient::fetchCommand(const QString& range) {
    QString tag = generateTag();
    QString fetchRange = range.isEmpty() ? "1:*" : range;
    QString command = QString("%1 FETCH %2 (UID FLAGS ENVELOPE BODY[HEADER])").arg(tag, fetchRange);
    
    sendCommand(command);
    
    QStringList responses = readMultilineResponse();
    QList<EmailCard> cards;
    
    QString currentHeaders;
    QString currentUid;
    QStringList currentFlags;
    
    for (const QString& response : responses) {
        if (response.startsWith("*") && response.contains("FETCH")) {
            // Parse FETCH response
            QRegularExpression uidRe("UID (\\d+)");
            QRegularExpressionMatch uidMatch = uidRe.match(response);
            if (uidMatch.hasMatch()) {
                currentUid = uidMatch.captured(1);
            }
            
            // Parse flags
            QRegularExpression flagsRe("FLAGS \\(([^)]*)\\)");
            QRegularExpressionMatch flagsMatch = flagsRe.match(response);
            if (flagsMatch.hasMatch()) {
                currentFlags = flagsMatch.captured(1).split(' ', Qt::SkipEmptyParts);
            }
        } else if (!response.startsWith("*") && !response.contains(tag)) {
            // This is header data
            currentHeaders += response + "\n";
        } else if (response.contains(tag)) {
            // End of fetch, create card
            if (!currentUid.isEmpty()) {
                EmailCard card = parseEmailHeaders(currentHeaders, currentUid);
                card.setFlags(currentFlags);
                cards.append(card);
            }
            
            // Reset for next card
            currentHeaders.clear();
            currentUid.clear();
            currentFlags.clear();
        }
    }
    
    return cards;
}

bool ImapClient::storeCommand(const QString& uid, const QString& flags, bool add) {
    QString tag = generateTag();
    QString operation = add ? "+FLAGS" : "-FLAGS";
    QString command = QString("%1 UID STORE %2 %3 (%4)").arg(tag, uid, operation, flags);
    
    sendCommand(command);
    
    QString response = readResponse();
    QString responseTag, status, data;
    
    return parseResponse(response, responseTag, status, data) && 
           responseTag == tag && status == "OK";
}

bool ImapClient::moveCommand(const QString& uid, const QString& targetMailbox) {
    QString tag = generateTag();
    QString command = QString("%1 UID MOVE %2 \"%3\"").arg(tag, uid, targetMailbox);
    
    sendCommand(command);
    
    QString response = readResponse();
    QString responseTag, status, data;
    
    return parseResponse(response, responseTag, status, data) && 
           responseTag == tag && status == "OK";
}

bool ImapClient::expungeCommand() {
    QString tag = generateTag();
    QString command = QString("%1 EXPUNGE").arg(tag);
    
    sendCommand(command);
    
    QString response = readResponse();
    QString responseTag, status, data;
    
    return parseResponse(response, responseTag, status, data) && 
           responseTag == tag && status == "OK";
}

EmailCard ImapClient::parseEmailHeaders(const QString& headers, const QString& uid) {
    EmailCard card;
    card.setUid(uid);
    
    QString subject = extractHeaderValue(headers, "Subject");
    QString from = extractHeaderValue(headers, "From");
    QString to = extractHeaderValue(headers, "To");
    QString dateStr = extractHeaderValue(headers, "Date");
    
    card.setSubject(subject);
    card.setFrom(from);
    card.setTo(to);
    card.setDate(parseDate(dateStr));
    
    return card;
}

QString ImapClient::extractHeaderValue(const QString& headers, const QString& headerName) {
    QRegularExpression re(QString("^%1:\\s*(.+)$").arg(headerName), 
                         QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(headers);
    
    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }
    
    return QString();
}

QDateTime ImapClient::parseDate(const QString& dateStr) {
    // Try different date formats commonly used in email headers
    QStringList formats = {
        "ddd, dd MMM yyyy hh:mm:ss +hhmm",
        "dd MMM yyyy hh:mm:ss +hhmm",
        "ddd, dd MMM yyyy hh:mm:ss",
        "dd MMM yyyy hh:mm:ss"
    };
    
    for (const QString& format : formats) {
        QDateTime dt = QDateTime::fromString(dateStr, format);
        if (dt.isValid()) {
            return dt;
        }
    }
    
    return QDateTime::currentDateTime();
}