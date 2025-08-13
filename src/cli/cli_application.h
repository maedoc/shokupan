#pragma once

#include "../core/kanban_model.h"
#include <QCoreApplication>
#include <QCommandLineParser>

class CliApplication : public QCoreApplication {
    Q_OBJECT

public:
    CliApplication(int argc, char* argv[]);
    ~CliApplication();

    int run();

private slots:
    void onConnected();
    void onDisconnected();
    void onError(const QString& message);

private:
    void setupCommandLineParser();
    int executeCommand();
    
    // Command implementations
    int listMailboxes();
    int showCards(const QString& mailbox);
    int moveCard(const QString& uid, const QString& fromMailbox, const QString& toMailbox);
    int deleteCard(const QString& uid, const QString& mailbox);
    int markCard(const QString& uid, const QString& mailbox, const QString& flag, bool set);
    int configure();
    int status();
    
    // Helper methods
    void printCard(const EmailCard& card, bool detailed = false);
    void printMailboxList(const MailboxList& list);
    bool waitForConnection(int timeoutMs = 10000);
    QString promptForInput(const QString& prompt, bool hidden = false);
    
    QCommandLineParser m_parser;
    KanbanModel* m_model;
    bool m_connected;
    QString m_lastError;
    bool m_verbose;
    int m_argc;
    char** m_argv;
};