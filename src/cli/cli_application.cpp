#include "cli_application.h"
#include <QTextStream>
#include <QEventLoop>
#include <QTimer>
#include <QLoggingCategory>
#include <iostream>

CliApplication::CliApplication(int argc, char* argv[])
    : QCoreApplication(argc, argv)
    , m_model(nullptr)
    , m_connected(false)
    , m_verbose(false)
    , m_argc(argc)
    , m_argv(argv)
{
    setApplicationName("imap-kanban-cli");
    setApplicationVersion("1.0.0");
    
    setupCommandLineParser();
    
    // Initialize model after command line parsing
    // m_model = new KanbanModel(this);
    // connect(m_model, &KanbanModel::connected, this, &CliApplication::onConnected);
    // connect(m_model, &KanbanModel::disconnected, this, &CliApplication::onDisconnected);
    // connect(m_model, &KanbanModel::error, this, &CliApplication::onError);
}

CliApplication::~CliApplication() {
}

int CliApplication::run() {
    // Convert argc/argv to QStringList
    QStringList args;
    for (int i = 0; i < m_argc; ++i) {
        args << QString::fromLocal8Bit(m_argv[i]);
    }
    
    if (args.isEmpty()) {
        std::cerr << "No arguments available" << std::endl;
        return 1;
    }
    
    // Try to process manually first
    if (args.size() > 1 && (args[1] == "--help" || args[1] == "-h")) {
        std::cout << "Usage: imap-kanban-cli [options] command" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "      --verbose     Enable verbose logging output" << std::endl;
        std::cout << "  -c, --config      Configuration file path" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  list-mailboxes    List available mailboxes" << std::endl;
        std::cout << "  show-cards        Show cards in a mailbox" << std::endl;
        std::cout << "  configure         Configure IMAP settings" << std::endl;
        std::cout << "  status            Show connection status" << std::endl;
        return 0;
    }
    
    return executeCommand();
}

void CliApplication::setupCommandLineParser() {
    m_parser.setApplicationDescription("IMAP-based Kanban board command line interface");
    m_parser.addHelpOption();
    m_parser.addVersionOption();
    
    // Commands
    m_parser.addPositionalArgument("command", "Command to execute", 
        "list-mailboxes|show-cards|move-card|delete-card|mark-read|mark-unread|mark-flag|mark-unflag|configure|status");
    
    // Options
    QCommandLineOption mailboxOption(QStringList() << "m" << "mailbox",
        "Mailbox name", "mailbox");
    m_parser.addOption(mailboxOption);
    
    QCommandLineOption fromOption(QStringList() << "f" << "from",
        "Source mailbox for move operation", "from");
    m_parser.addOption(fromOption);
    
    QCommandLineOption toOption(QStringList() << "t" << "to",
        "Target mailbox for move operation", "to");
    m_parser.addOption(toOption);
    
    QCommandLineOption uidOption(QStringList() << "u" << "uid",
        "Email UID", "uid");
    m_parser.addOption(uidOption);
    
    QCommandLineOption detailedOption(QStringList() << "d" << "detailed",
        "Show detailed information");
    m_parser.addOption(detailedOption);
    
    QCommandLineOption configFileOption(QStringList() << "c" << "config",
        "Configuration file path", "config");
    m_parser.addOption(configFileOption);
    
    QCommandLineOption verboseOption("verbose",
        "Enable verbose logging output");
    m_parser.addOption(verboseOption);
}

int CliApplication::executeCommand() {
    // Convert argc/argv to QStringList
    QStringList args;
    for (int i = 0; i < m_argc; ++i) {
        args << QString::fromLocal8Bit(m_argv[i]);
    }
    
    // Process arguments with the parser first
    m_parser.process(args);
    
    // Handle verbose logging flag
    m_verbose = m_parser.isSet("verbose");
    if (m_verbose) {
        // Enable Qt debug output
        QLoggingCategory::setFilterRules("*=true");
    } else {
        // Disable Qt debug output (default)
        QLoggingCategory::setFilterRules("*.debug=false");
    }
    
    // If a config file was provided, load it into the model settings before any network activity
    QString configPath = m_parser.value("config");
    if (!configPath.isEmpty()) {
        if (!m_model) {
            m_model = new KanbanModel(this);
#ifdef QT_DEBUG
            if (!m_model) {
                qDebug() << "CliApplication: failed to allocate m_model";
            }
#endif
            connect(m_model, &KanbanModel::connected, this, &CliApplication::onConnected);
            connect(m_model, &KanbanModel::disconnected, this, &CliApplication::onDisconnected);
            connect(m_model, &KanbanModel::error, this, &CliApplication::onError);
        }
        m_model->settings().loadFromFile(configPath);
    }
    
    // Get positional arguments (commands)
    const QStringList positionalArgs = m_parser.positionalArguments();
    if (positionalArgs.isEmpty()) {
        std::cout << "Usage: imap-kanban-cli [options] command" << std::endl;
        std::cout << "Use --help for more information" << std::endl;
        return 1;
    }
    
    const QString command = positionalArgs.first();
    
    if (command == "configure") {
        // Initialize model for configuration
        if (!m_model) {
            m_model = new KanbanModel(this);
            connect(m_model, &KanbanModel::connected, this, &CliApplication::onConnected);
            connect(m_model, &KanbanModel::disconnected, this, &CliApplication::onDisconnected);
            connect(m_model, &KanbanModel::error, this, &CliApplication::onError);
        }
        return configure();
    }
    
    if (command == "status") {
        // Initialize model for status
        if (!m_model) {
            m_model = new KanbanModel(this);
            connect(m_model, &KanbanModel::connected, this, &CliApplication::onConnected);
            connect(m_model, &KanbanModel::disconnected, this, &CliApplication::onDisconnected);
            connect(m_model, &KanbanModel::error, this, &CliApplication::onError);
        }
        return status();
    }
    
    // For other commands, we need to connect to the server
    if (!m_model) {
        m_model = new KanbanModel(this);
        connect(m_model, &KanbanModel::connected, this, &CliApplication::onConnected);
        connect(m_model, &KanbanModel::disconnected, this, &CliApplication::onDisconnected);
        connect(m_model, &KanbanModel::error, this, &CliApplication::onError);
    }
    
    if (!m_model->connectToServer()) {
        std::cerr << "Failed to connect to IMAP server: " 
                  << m_model->lastError().toStdString() << std::endl;
        return 1;
    }
    
    if (!waitForConnection()) {
        std::cerr << "Connection timeout or failed: " 
                  << m_lastError.toStdString() << std::endl;
        return 1;
    }
    
    if (command == "list-mailboxes") {
        return listMailboxes();
    } else if (command == "show-cards") {
        QString mailbox = m_parser.value("mailbox");
        if (mailbox.isEmpty()) {
            std::cerr << "Mailbox name required for show-cards command" << std::endl;
            return 1;
        }
        return showCards(mailbox);
    } else if (command == "move-card") {
        QString uid = m_parser.value("uid");
        QString from = m_parser.value("from");
        QString to = m_parser.value("to");
        
        if (uid.isEmpty() || from.isEmpty() || to.isEmpty()) {
            std::cerr << "UID, from, and to mailboxes required for move-card command" << std::endl;
            return 1;
        }
        return moveCard(uid, from, to);
    } else if (command == "delete-card") {
        QString uid = m_parser.value("uid");
        QString mailbox = m_parser.value("mailbox");
        
        if (uid.isEmpty() || mailbox.isEmpty()) {
            std::cerr << "UID and mailbox required for delete-card command" << std::endl;
            return 1;
        }
        return deleteCard(uid, mailbox);
    } else if (command == "mark-read") {
        QString uid = m_parser.value("uid");
        QString mailbox = m_parser.value("mailbox");
        
        if (uid.isEmpty() || mailbox.isEmpty()) {
            std::cerr << "UID and mailbox required for mark-read command" << std::endl;
            return 1;
        }
        return markCard(uid, mailbox, "read", true);
    } else if (command == "mark-unread") {
        QString uid = m_parser.value("uid");
        QString mailbox = m_parser.value("mailbox");
        
        if (uid.isEmpty() || mailbox.isEmpty()) {
            std::cerr << "UID and mailbox required for mark-unread command" << std::endl;
            return 1;
        }
        return markCard(uid, mailbox, "read", false);
    } else if (command == "mark-flag") {
        QString uid = m_parser.value("uid");
        QString mailbox = m_parser.value("mailbox");
        
        if (uid.isEmpty() || mailbox.isEmpty()) {
            std::cerr << "UID and mailbox required for mark-flag command" << std::endl;
            return 1;
        }
        return markCard(uid, mailbox, "flag", true);
    } else if (command == "mark-unflag") {
        QString uid = m_parser.value("uid");
        QString mailbox = m_parser.value("mailbox");
        
        if (uid.isEmpty() || mailbox.isEmpty()) {
            std::cerr << "UID and mailbox required for mark-unflag command" << std::endl;
            return 1;
        }
        return markCard(uid, mailbox, "flag", false);
    } else {
        std::cerr << "Unknown command: " << command.toStdString() << std::endl;
        m_parser.showHelp(1);
        return 1;
    }
}

int CliApplication::listMailboxes() {
    QStringList mailboxes = m_model->availableMailboxes();
    QStringList visible = m_model->visibleMailboxes();
    
    std::cout << "Available mailboxes:" << std::endl;
    for (const QString& mailbox : mailboxes) {
        std::cout << "  " << mailbox.toStdString();
        if (visible.contains(mailbox)) {
            std::cout << " (visible)";
        }
        std::cout << std::endl;
    }
    
    return 0;
}

int CliApplication::showCards(const QString& mailbox) {
    MailboxList list = m_model->mailboxList(mailbox);
    
    std::cout << "Cards in mailbox '" << mailbox.toStdString() << "':" << std::endl;
    std::cout << "Total: " << list.cardCount() << " cards" << std::endl;
    std::cout << std::endl;
    
    bool detailed = m_parser.isSet("detailed");
    const QList<EmailCard> cards = list.cards();
    
    for (const EmailCard& card : cards) {
        printCard(card, detailed);
        std::cout << std::endl;
    }
    
    return 0;
}

int CliApplication::moveCard(const QString& uid, const QString& fromMailbox, const QString& toMailbox) {
    if (m_model->moveCard(uid, fromMailbox, toMailbox)) {
        std::cout << "Card " << uid.toStdString() 
                  << " moved from '" << fromMailbox.toStdString() 
                  << "' to '" << toMailbox.toStdString() << "'" << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to move card: " << m_model->lastError().toStdString() << std::endl;
        return 1;
    }
}

int CliApplication::deleteCard(const QString& uid, const QString& mailbox) {
    if (m_model->deleteCard(uid, mailbox)) {
        std::cout << "Card " << uid.toStdString() 
                  << " deleted from '" << mailbox.toStdString() << "'" << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to delete card: " << m_model->lastError().toStdString() << std::endl;
        return 1;
    }
}

int CliApplication::markCard(const QString& uid, const QString& mailbox, const QString& flag, bool set) {
    bool success = false;
    QString action;
    
    if (flag == "read") {
        success = m_model->markCardAsRead(uid, mailbox, set);
        action = set ? "marked as read" : "marked as unread";
    } else if (flag == "flag") {
        success = m_model->markCardAsFlagged(uid, mailbox, set);
        action = set ? "flagged" : "unflagged";
    }
    
    if (success) {
        std::cout << "Card " << uid.toStdString() 
                  << " " << action.toStdString() << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to update card: " << m_model->lastError().toStdString() << std::endl;
        return 1;
    }
}

int CliApplication::configure() {
    std::cout << "IMAP Kanban Configuration" << std::endl;
    std::cout << "=========================" << std::endl;
    
    Settings& settings = m_model->settings();
    
    // IMAP server settings
    QString server = promptForInput(QString("IMAP Server [%1]: ").arg(settings.imapServer()));
    if (!server.isEmpty()) {
        settings.setImapServer(server);
    }
    
    QString portStr = promptForInput(QString("IMAP Port [%1]: ").arg(settings.imapPort()));
    if (!portStr.isEmpty()) {
        bool ok;
        int port = portStr.toInt(&ok);
        if (ok && port > 0 && port <= 65535) {
            settings.setImapPort(port);
        }
    }
    
    QString sslStr = promptForInput(QString("Use SSL [%1]: ").arg(settings.useSSL() ? "yes" : "no"));
    if (!sslStr.isEmpty()) {
        settings.setUseSSL(sslStr.toLower().startsWith("y"));
    }
    
    QString username = promptForInput(QString("Username [%1]: ").arg(settings.username()));
    if (!username.isEmpty()) {
        settings.setUsername(username);
    }
    
    QString password = promptForInput("Password: ", true);
    if (!password.isEmpty()) {
        settings.setPassword(password);
    }
    
    // Save settings
    m_model->saveSettings();
    
    std::cout << "Configuration saved." << std::endl;
    return 0;
}

int CliApplication::status() {
    const Settings& settings = m_model->settings();
    
    std::cout << "IMAP Kanban Status" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "Server: " << settings.imapServer().toStdString() << ":" << settings.imapPort() << std::endl;
    std::cout << "SSL: " << (settings.useSSL() ? "enabled" : "disabled") << std::endl;
    std::cout << "Username: " << settings.username().toStdString() << std::endl;
    std::cout << "Connected: " << (m_model->isConnected() ? "yes" : "no") << std::endl;
    
    QStringList visible = settings.visibleMailboxes();
    std::cout << "Visible mailboxes (" << visible.size() << "): ";
    for (int i = 0; i < visible.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << visible[i].toStdString();
    }
    std::cout << std::endl;
    
    return 0;
}

void CliApplication::printCard(const EmailCard& card, bool detailed) {
    std::cout << "UID: " << card.uid().toStdString() << std::endl;
    std::cout << "Subject: " << card.subject().toStdString() << std::endl;
    std::cout << "From: " << card.from().toStdString() << std::endl;
    std::cout << "Date: " << card.date().toString().toStdString() << std::endl;
    
    if (detailed) {
        std::cout << "To: " << card.to().toStdString() << std::endl;
        std::cout << "Read: " << (card.isRead() ? "yes" : "no") << std::endl;
        std::cout << "Flagged: " << (card.isFlagged() ? "yes" : "no") << std::endl;
        
        if (!card.body().isEmpty()) {
            QString body = card.body();
            if (body.length() > 200) {
                body = body.left(200) + "...";
            }
            std::cout << "Body: " << body.toStdString() << std::endl;
        }
    }
}

void CliApplication::printMailboxList(const MailboxList& list) {
    std::cout << "Mailbox: " << list.displayName().toStdString() 
              << " (" << list.cardCount() << " cards)" << std::endl;
}

bool CliApplication::waitForConnection(int timeoutMs) {
    if (m_connected) {
        return true;
    }
    
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    if (m_model) {
        connect(m_model, &KanbanModel::connected, &loop, &QEventLoop::quit);
        connect(m_model, &KanbanModel::error, &loop, &QEventLoop::quit);
    }
    
    timer.start(timeoutMs);
    loop.exec();
    
    return m_connected;
}

QString CliApplication::promptForInput(const QString& prompt, bool hidden) {
    std::cout << prompt.toStdString();
    std::cout.flush();
    
    QString input;
    if (hidden) {
        // For password input, we'd ideally use platform-specific methods
        // For now, just use regular input (not secure)
        std::string stdInput;
        std::getline(std::cin, stdInput);
        input = QString::fromStdString(stdInput);
    } else {
        std::string stdInput;
        std::getline(std::cin, stdInput);
        input = QString::fromStdString(stdInput);
    }
    
    return input.trimmed();
}

void CliApplication::onConnected() {
    m_connected = true;
}

void CliApplication::onDisconnected() {
    m_connected = false;
}

void CliApplication::onError(const QString& message) {
    m_lastError = message;
}