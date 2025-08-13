#pragma once

#include <QString>
#include <QStringList>
#include <QSettings>

class Settings {
public:
    Settings();
    
    // IMAP connection settings
    QString imapServer() const;
    void setImapServer(const QString& server);
    
    int imapPort() const;
    void setImapPort(int port);
    
    bool useSSL() const;
    void setUseSSL(bool ssl);
    
    QString username() const;
    void setUsername(const QString& username);
    
    QString password() const;
    void setPassword(const QString& password);
    
    // Kanban settings
    QStringList visibleMailboxes() const;
    void setVisibleMailboxes(const QStringList& mailboxes);
    
    // UI settings
    int refreshInterval() const;
    void setRefreshInterval(int seconds);
    
    // Save/load
    void save();
    void load();
    
private:
    QSettings m_settings;
    
    QString m_imapServer;
    int m_imapPort;
    bool m_useSSL;
    QString m_username;
    QString m_password;
    QStringList m_visibleMailboxes;
    int m_refreshInterval;
};