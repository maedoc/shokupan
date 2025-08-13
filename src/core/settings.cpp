#include "settings.h"

Settings::Settings() 
    : m_settings("IMAPKanban", "IMAPKanban")
    , m_imapPort(993)
    , m_useSSL(true)
    , m_refreshInterval(30)
{
    load();
}

QString Settings::imapServer() const {
    return m_imapServer;
}

void Settings::setImapServer(const QString& server) {
    m_imapServer = server;
}

int Settings::imapPort() const {
    return m_imapPort;
}

void Settings::setImapPort(int port) {
    m_imapPort = port;
}

bool Settings::useSSL() const {
    return m_useSSL;
}

void Settings::setUseSSL(bool ssl) {
    m_useSSL = ssl;
}

QString Settings::username() const {
    return m_username;
}

void Settings::setUsername(const QString& username) {
    m_username = username;
}

QString Settings::password() const {
    return m_password;
}

void Settings::setPassword(const QString& password) {
    m_password = password;
}

QStringList Settings::visibleMailboxes() const {
    return m_visibleMailboxes;
}

void Settings::setVisibleMailboxes(const QStringList& mailboxes) {
    m_visibleMailboxes = mailboxes;
}

int Settings::refreshInterval() const {
    return m_refreshInterval;
}

void Settings::setRefreshInterval(int seconds) {
    m_refreshInterval = seconds;
}

void Settings::save() {
    m_settings.setValue("imap/server", m_imapServer);
    m_settings.setValue("imap/port", m_imapPort);
    m_settings.setValue("imap/ssl", m_useSSL);
    m_settings.setValue("imap/username", m_username);
    m_settings.setValue("imap/password", m_password);
    m_settings.setValue("kanban/visibleMailboxes", m_visibleMailboxes);
    m_settings.setValue("ui/refreshInterval", m_refreshInterval);
    m_settings.sync();
}

void Settings::load() {
    m_imapServer = m_settings.value("imap/server", "").toString();
    m_imapPort = m_settings.value("imap/port", 993).toInt();
    m_useSSL = m_settings.value("imap/ssl", true).toBool();
    m_username = m_settings.value("imap/username", "").toString();
    m_password = m_settings.value("imap/password", "").toString();
    m_visibleMailboxes = m_settings.value("kanban/visibleMailboxes", QStringList()).toStringList();
    m_refreshInterval = m_settings.value("ui/refreshInterval", 30).toInt();
}