#pragma once

#include "email_card.h"
#include "mailbox_list.h"
#include "settings.h"
#include <QObject>
#include <QTcpSocket>
#include <QSslSocket>
#include <QTimer>
#include <QStringList>

class ImapClient : public QObject {
    Q_OBJECT

public:
    enum State {
        Disconnected,
        Connecting,
        Connected,
        Authenticated,
        Selected,
        Error
    };

    explicit ImapClient(QObject* parent = nullptr);
    ~ImapClient();

    // Connection management
    void connectToServer(const Settings& settings);
    void disconnectFromServer();
    State state() const;
    QString lastError() const;

    // Authentication
    void authenticate(const QString& username, const QString& password);

    // Mailbox operations
    QStringList listMailboxes();
    bool selectMailbox(const QString& mailbox);
    QString currentMailbox() const;

    // Email operations
    QList<EmailCard> fetchCards(const QString& mailbox = QString());
    EmailCard fetchCard(const QString& uid, const QString& mailbox = QString());
    bool moveCard(const QString& uid, const QString& fromMailbox, const QString& toMailbox);
    bool deleteCard(const QString& uid, const QString& mailbox = QString());
    bool markAsRead(const QString& uid, bool read = true, const QString& mailbox = QString());
    bool markAsFlagged(const QString& uid, bool flagged = true, const QString& mailbox = QString());

    // Utility
    bool isConnected() const;
    bool isAuthenticated() const;

signals:
    void connected();
    void disconnected();
    void authenticated();
    void error(const QString& message);
    void mailboxSelected(const QString& mailbox);
    void cardsFetched(const QList<EmailCard>& cards);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void onSslErrors(const QList<QSslError>& errors);
    void onReadyRead();

private:
    void sendCommand(const QString& command);
    QString readResponse();
    QStringList readMultilineResponse();
    bool waitForResponse(int timeoutMs = 5000);
    
    QString generateTag();
    bool parseResponse(const QString& response, QString& tag, QString& status, QString& data);
    
    // IMAP command helpers
    bool loginCommand(const QString& username, const QString& password);
    QStringList listCommand();
    bool selectCommand(const QString& mailbox);
    QList<EmailCard> fetchCommand(const QString& range = "1:*");
    bool storeCommand(const QString& uid, const QString& flags, bool add = true);
    bool moveCommand(const QString& uid, const QString& targetMailbox);
    bool expungeCommand();

    // Email parsing
    EmailCard parseEmailHeaders(const QString& headers, const QString& uid);
    QString extractHeaderValue(const QString& headers, const QString& headerName);
    QDateTime parseDate(const QString& dateStr);

    QSslSocket* m_socket;
    State m_state;
    QString m_lastError;
    QString m_currentMailbox;
    int m_tagCounter;
    QString m_responseBuffer;
    
    // Settings
    QString m_server;
    int m_port;
    bool m_useSSL;
};