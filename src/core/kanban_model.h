#pragma once

#include "imap_client.h"
#include "mailbox_list.h"
#include "settings.h"
#include <QObject>
#include <QTimer>

class KanbanModel : public QObject {
    Q_OBJECT

public:
    explicit KanbanModel(QObject* parent = nullptr);
    ~KanbanModel();

    // Connection management
    bool connectToServer();
    void disconnectFromServer();
    bool isConnected() const;
    QString lastError() const;

    // Settings
    Settings& settings();
    const Settings& settings() const;
    void saveSettings();

    // Mailbox operations
    QStringList availableMailboxes() const;
    QStringList visibleMailboxes() const;
    void setVisibleMailboxes(const QStringList& mailboxes);
    
    MailboxList mailboxList(const QString& mailbox) const;
    QList<MailboxList> allMailboxLists() const;

    // Card operations
    EmailCard card(const QString& uid, const QString& mailbox) const;
    bool moveCard(const QString& uid, const QString& fromMailbox, const QString& toMailbox);
    bool deleteCard(const QString& uid, const QString& mailbox);
    bool markCardAsRead(const QString& uid, const QString& mailbox, bool read = true);
    bool markCardAsFlagged(const QString& uid, const QString& mailbox, bool flagged = true);

    // Refresh operations

    void refreshAll();
    void refreshMailbox(const QString& mailbox);
    void setAutoRefresh(bool enabled);
    bool autoRefreshEnabled() const;

    // Reload mailbox list from server
    void reloadMailboxes();

signals:
    void connected();
    void disconnected();
    void error(const QString& message);
    void mailboxesChanged();
    void mailboxUpdated(const QString& mailbox);
    void cardMoved(const QString& uid, const QString& fromMailbox, const QString& toMailbox);
    void cardDeleted(const QString& uid, const QString& mailbox);
    void cardUpdated(const QString& uid, const QString& mailbox);

private slots:
    void onImapConnected();
    void onImapDisconnected();
    void onImapAuthenticated();
    void onImapError(const QString& message);
    void onAutoRefreshTimer();

private:
    void updateMailboxList(const QString& mailbox, const QList<EmailCard>& cards);
    void startAutoRefresh();
    void stopAutoRefresh();

    ImapClient* m_imapClient;
    Settings m_settings;
    QTimer* m_autoRefreshTimer;
    
    QStringList m_availableMailboxes;
    QHash<QString, MailboxList> m_mailboxLists;
    
    bool m_autoRefreshEnabled;
    QString m_lastError;
};