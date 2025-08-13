#include "kanban_model.h"
#include <QDebug>

KanbanModel::KanbanModel(QObject* parent)
    : QObject(parent)
    , m_imapClient(new ImapClient(this))
    , m_autoRefreshTimer(new QTimer(this))
    , m_autoRefreshEnabled(false)
{
    connect(m_imapClient, &ImapClient::connected, this, &KanbanModel::onImapConnected);
    connect(m_imapClient, &ImapClient::disconnected, this, &KanbanModel::onImapDisconnected);
    connect(m_imapClient, &ImapClient::authenticated, this, &KanbanModel::onImapAuthenticated);
    connect(m_imapClient, &ImapClient::error, this, &KanbanModel::onImapError);
    
    connect(m_autoRefreshTimer, &QTimer::timeout, this, &KanbanModel::onAutoRefreshTimer);
    m_autoRefreshTimer->setSingleShot(false);
}

KanbanModel::~KanbanModel() {
    disconnectFromServer();
}

bool KanbanModel::connectToServer() {
    if (m_settings.imapServer().isEmpty() || m_settings.username().isEmpty()) {
        m_lastError = "IMAP server or username not configured";
        emit error(m_lastError);
        return false;
    }

    m_imapClient->connectToServer(m_settings);
    return true;
}

void KanbanModel::disconnectFromServer() {
    stopAutoRefresh();
    m_imapClient->disconnectFromServer();
    m_availableMailboxes.clear();
    m_mailboxLists.clear();
}

bool KanbanModel::isConnected() const {
    return m_imapClient->isAuthenticated();
}

QString KanbanModel::lastError() const {
    return m_lastError.isEmpty() ? m_imapClient->lastError() : m_lastError;
}

Settings& KanbanModel::settings() {
    return m_settings;
}

const Settings& KanbanModel::settings() const {
    return m_settings;
}

void KanbanModel::saveSettings() {
    m_settings.save();
}

QStringList KanbanModel::availableMailboxes() const {
    return m_availableMailboxes;
}

QStringList KanbanModel::visibleMailboxes() const {
    return m_settings.visibleMailboxes();
}

void KanbanModel::setVisibleMailboxes(const QStringList& mailboxes) {
    m_settings.setVisibleMailboxes(mailboxes);
    emit mailboxesChanged();
}

MailboxList KanbanModel::mailboxList(const QString& mailbox) const {
    return m_mailboxLists.value(mailbox, MailboxList(mailbox));
}

QList<MailboxList> KanbanModel::allMailboxLists() const {
    QList<MailboxList> lists;
    const QStringList visible = visibleMailboxes();
    
    for (const QString& mailbox : visible) {
        lists.append(m_mailboxLists.value(mailbox, MailboxList(mailbox)));
    }
    
    return lists;
}

EmailCard KanbanModel::card(const QString& uid, const QString& mailbox) const {
    const MailboxList& list = m_mailboxLists.value(mailbox);
    return list.card(uid);
}

bool KanbanModel::moveCard(const QString& uid, const QString& fromMailbox, const QString& toMailbox) {
    if (!isConnected()) {
        m_lastError = "Not connected to IMAP server";
        emit error(m_lastError);
        return false;
    }

    if (m_imapClient->moveCard(uid, fromMailbox, toMailbox)) {
        // Update local model
        if (m_mailboxLists.contains(fromMailbox)) {
            EmailCard card = m_mailboxLists[fromMailbox].card(uid);
            m_mailboxLists[fromMailbox].removeCard(uid);
            
            if (m_mailboxLists.contains(toMailbox)) {
                m_mailboxLists[toMailbox].addCard(card);
            }
        }
        
        emit cardMoved(uid, fromMailbox, toMailbox);
        emit mailboxUpdated(fromMailbox);
        emit mailboxUpdated(toMailbox);
        return true;
    }
    
    m_lastError = m_imapClient->lastError();
    emit error(m_lastError);
    return false;
}

bool KanbanModel::deleteCard(const QString& uid, const QString& mailbox) {
    if (!isConnected()) {
        m_lastError = "Not connected to IMAP server";
        emit error(m_lastError);
        return false;
    }

    if (m_imapClient->deleteCard(uid, mailbox)) {
        // Update local model
        if (m_mailboxLists.contains(mailbox)) {
            m_mailboxLists[mailbox].removeCard(uid);
        }
        
        emit cardDeleted(uid, mailbox);
        emit mailboxUpdated(mailbox);
        return true;
    }
    
    m_lastError = m_imapClient->lastError();
    emit error(m_lastError);
    return false;
}

bool KanbanModel::markCardAsRead(const QString& uid, const QString& mailbox, bool read) {
    if (!isConnected()) {
        m_lastError = "Not connected to IMAP server";
        emit error(m_lastError);
        return false;
    }

    if (m_imapClient->markAsRead(uid, read, mailbox)) {
        // Update local model
        if (m_mailboxLists.contains(mailbox)) {
            EmailCard card = m_mailboxLists[mailbox].card(uid);
            if (card.isValid()) {
                card.setRead(read);
                m_mailboxLists[mailbox].updateCard(card);
            }
        }
        
        emit cardUpdated(uid, mailbox);
        emit mailboxUpdated(mailbox);
        return true;
    }
    
    m_lastError = m_imapClient->lastError();
    emit error(m_lastError);
    return false;
}

bool KanbanModel::markCardAsFlagged(const QString& uid, const QString& mailbox, bool flagged) {
    if (!isConnected()) {
        m_lastError = "Not connected to IMAP server";
        emit error(m_lastError);
        return false;
    }

    if (m_imapClient->markAsFlagged(uid, flagged, mailbox)) {
        // Update local model
        if (m_mailboxLists.contains(mailbox)) {
            EmailCard card = m_mailboxLists[mailbox].card(uid);
            if (card.isValid()) {
                card.setFlagged(flagged);
                m_mailboxLists[mailbox].updateCard(card);
            }
        }
        
        emit cardUpdated(uid, mailbox);
        emit mailboxUpdated(mailbox);
        return true;
    }
    
    m_lastError = m_imapClient->lastError();
    emit error(m_lastError);
    return false;
}

void KanbanModel::refreshAll() {
    if (!isConnected()) {
        return;
    }

    const QStringList visible = visibleMailboxes();
    for (const QString& mailbox : visible) {
        refreshMailbox(mailbox);
    }
}

void KanbanModel::refreshMailbox(const QString& mailbox) {
    if (!isConnected()) {
        return;
    }

    QList<EmailCard> cards = m_imapClient->fetchCards(mailbox);
    updateMailboxList(mailbox, cards);
    emit mailboxUpdated(mailbox);
}

void KanbanModel::setAutoRefresh(bool enabled) {
    m_autoRefreshEnabled = enabled;
    
    if (enabled && isConnected()) {
        startAutoRefresh();
    } else {
        stopAutoRefresh();
    }
}

bool KanbanModel::autoRefreshEnabled() const {
    return m_autoRefreshEnabled;
}

void KanbanModel::onImapConnected() {
    // Authenticate automatically
    m_imapClient->authenticate(m_settings.username(), m_settings.password());
}

void KanbanModel::onImapDisconnected() {
    stopAutoRefresh();
    m_availableMailboxes.clear();
    m_mailboxLists.clear();
    emit disconnected();
}

void KanbanModel::onImapAuthenticated() {
    // Fetch available mailboxes
    m_availableMailboxes = m_imapClient->listMailboxes();
    
    // If no visible mailboxes are configured, use all available ones
    if (m_settings.visibleMailboxes().isEmpty()) {
        m_settings.setVisibleMailboxes(m_availableMailboxes);
    }
    
    emit connected();
    emit mailboxesChanged();
    
    // Start auto-refresh if enabled
    if (m_autoRefreshEnabled) {
        startAutoRefresh();
    }
    
    // Initial refresh
    refreshAll();
}

void KanbanModel::onImapError(const QString& message) {
    m_lastError = message;
    emit error(message);
}

void KanbanModel::onAutoRefreshTimer() {
    refreshAll();
}

void KanbanModel::updateMailboxList(const QString& mailbox, const QList<EmailCard>& cards) {
    MailboxList& list = m_mailboxLists[mailbox];
    list.setName(mailbox);
    list.setCards(cards);
    list.sortCards(MailboxList::DateDescending);
}

void KanbanModel::startAutoRefresh() {
    if (m_settings.refreshInterval() > 0) {
        m_autoRefreshTimer->start(m_settings.refreshInterval() * 1000);
    }
}

void KanbanModel::stopAutoRefresh() {
    m_autoRefreshTimer->stop();
}