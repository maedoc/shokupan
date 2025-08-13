#include "main_window.h"
#include "card_dialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QKeySequence>
#include <QCloseEvent>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new KanbanModel(this))
    , m_kanbanBoard(new KanbanBoard(m_model, this))
    , m_settingsDialog(nullptr)
    , m_connectionStatusLabel(new QLabel(this))
    , m_mailboxCountLabel(new QLabel(this))
    , m_cardCountLabel(new QLabel(this))
{
    setupUI();
    setupMenus();
    setupStatusBar();
    setupKeyboardShortcuts();
    
    // Connect model signals
    connect(m_model, &KanbanModel::connected, this, &MainWindow::onConnected);
    connect(m_model, &KanbanModel::disconnected, this, &MainWindow::onDisconnected);
    connect(m_model, &KanbanModel::error, this, &MainWindow::onError);
    connect(m_model, &KanbanModel::mailboxUpdated, this, &MainWindow::onMailboxUpdated);
    
    updateConnectionStatus();
    updateWindowTitle();
    
    // Try to connect automatically if settings are available
    const Settings& settings = m_model->settings();
    if (!settings.imapServer().isEmpty() && !settings.username().isEmpty()) {
        connectToServer();
    }
}

MainWindow::~MainWindow() {
}

void MainWindow::closeEvent(QCloseEvent* event) {
    m_model->disconnectFromServer();
    event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    // Handle global keyboard shortcuts
    if (event->key() == Qt::Key_F5) {
        refresh();
        event->accept();
        return;
    }
    
    QMainWindow::keyPressEvent(event);
}

void MainWindow::setupUI() {
    setCentralWidget(m_kanbanBoard);
    resize(1200, 800);
    setMinimumSize(800, 600);
}

void MainWindow::setupMenus() {
    // File menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    
    m_connectAction = fileMenu->addAction("&Connect", this, &MainWindow::connectToServer);
    m_connectAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
    
    m_disconnectAction = fileMenu->addAction("&Disconnect", this, &MainWindow::disconnectFromServer);
    m_disconnectAction->setShortcut(QKeySequence("Ctrl+Shift+D"));
    m_disconnectAction->setEnabled(false);
    
    fileMenu->addSeparator();
    
    m_settingsAction = fileMenu->addAction("&Settings...", this, &MainWindow::showSettings);
    m_settingsAction->setShortcut(QKeySequence("Ctrl+,"));
    
    fileMenu->addSeparator();
    
    m_quitAction = fileMenu->addAction("&Quit", this, &MainWindow::quit);
    m_quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    
    // Edit menu
    QMenu* editMenu = menuBar()->addMenu("&Edit");
    
    m_newCardAction = editMenu->addAction("&New Card", this, &MainWindow::newCard);
    m_newCardAction->setShortcut(QKeySequence("Ctrl+N"));
    m_newCardAction->setEnabled(false);
    
    m_editCardAction = editMenu->addAction("&Edit Card", this, &MainWindow::editCard);
    m_editCardAction->setShortcut(QKeySequence("Ctrl+E"));
    m_editCardAction->setEnabled(false);
    
    m_deleteCardAction = editMenu->addAction("&Delete Card", this, &MainWindow::deleteCard);
    m_deleteCardAction->setShortcut(QKeySequence("Del"));
    m_deleteCardAction->setEnabled(false);
    
    editMenu->addSeparator();
    
    m_moveCardAction = editMenu->addAction("&Move Card", this, &MainWindow::moveCard);
    m_moveCardAction->setShortcut(QKeySequence("Ctrl+M"));
    m_moveCardAction->setEnabled(false);
    
    editMenu->addSeparator();
    
    m_markReadAction = editMenu->addAction("Mark as &Read", this, &MainWindow::markAsRead);
    m_markReadAction->setShortcut(QKeySequence("Ctrl+R"));
    m_markReadAction->setEnabled(false);
    
    m_markUnreadAction = editMenu->addAction("Mark as &Unread", this, &MainWindow::markAsUnread);
    m_markUnreadAction->setShortcut(QKeySequence("Ctrl+U"));
    m_markUnreadAction->setEnabled(false);
    
    m_flagAction = editMenu->addAction("&Flag Card", this, &MainWindow::flagCard);
    m_flagAction->setShortcut(QKeySequence("Ctrl+F"));
    m_flagAction->setEnabled(false);
    
    m_unflagAction = editMenu->addAction("Un&flag Card", this, &MainWindow::unflagCard);
    m_unflagAction->setShortcut(QKeySequence("Ctrl+Shift+F"));
    m_unflagAction->setEnabled(false);
    
    // View menu
    QMenu* viewMenu = menuBar()->addMenu("&View");
    
    m_refreshAction = viewMenu->addAction("&Refresh", this, &MainWindow::refresh);
    m_refreshAction->setShortcut(QKeySequence("F5"));
    m_refreshAction->setEnabled(false);
    
    // Help menu
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    
    m_aboutAction = helpMenu->addAction("&About", this, &MainWindow::about);
}

void MainWindow::setupStatusBar() {
    statusBar()->addWidget(m_connectionStatusLabel);
    statusBar()->addPermanentWidget(m_mailboxCountLabel);
    statusBar()->addPermanentWidget(m_cardCountLabel);
    
    m_connectionStatusLabel->setText("Disconnected");
    m_mailboxCountLabel->setText("Mailboxes: 0");
    m_cardCountLabel->setText("Cards: 0");
}

void MainWindow::setupKeyboardShortcuts() {
    // Additional shortcuts are set up in setupMenus()
    // This method can be used for shortcuts that don't have menu items
}

void MainWindow::updateConnectionStatus() {
    bool connected = m_model->isConnected();
    
    m_connectAction->setEnabled(!connected);
    m_disconnectAction->setEnabled(connected);
    m_refreshAction->setEnabled(connected);
    m_newCardAction->setEnabled(connected);
    
    if (connected) {
        m_connectionStatusLabel->setText("Connected");
        m_connectionStatusLabel->setStyleSheet("color: green;");
    } else {
        m_connectionStatusLabel->setText("Disconnected");
        m_connectionStatusLabel->setStyleSheet("color: red;");
        
        // Disable card-specific actions
        m_editCardAction->setEnabled(false);
        m_deleteCardAction->setEnabled(false);
        m_moveCardAction->setEnabled(false);
        m_markReadAction->setEnabled(false);
        m_markUnreadAction->setEnabled(false);
        m_flagAction->setEnabled(false);
        m_unflagAction->setEnabled(false);
    }
}

void MainWindow::updateWindowTitle() {
    QString title = "IMAP Kanban";
    
    if (m_model->isConnected()) {
        const Settings& settings = m_model->settings();
        title += QString(" - %1@%2").arg(settings.username(), settings.imapServer());
    }
    
    setWindowTitle(title);
}

void MainWindow::onConnected() {
    updateConnectionStatus();
    updateWindowTitle();
    statusBar()->showMessage("Connected to IMAP server", 3000);
}

void MainWindow::onDisconnected() {
    updateConnectionStatus();
    updateWindowTitle();
    statusBar()->showMessage("Disconnected from IMAP server", 3000);
}

void MainWindow::onError(const QString& message) {
    QMessageBox::warning(this, "IMAP Kanban Error", message);
    statusBar()->showMessage("Error: " + message, 5000);
}

void MainWindow::onMailboxUpdated(const QString& mailbox) {
    Q_UNUSED(mailbox)
    
    // Update status bar counts
    QStringList visibleMailboxes = m_model->visibleMailboxes();
    int totalCards = 0;
    
    for (const QString& mb : visibleMailboxes) {
        MailboxList list = m_model->mailboxList(mb);
        totalCards += list.cardCount();
    }
    
    m_mailboxCountLabel->setText(QString("Mailboxes: %1").arg(visibleMailboxes.size()));
    m_cardCountLabel->setText(QString("Cards: %1").arg(totalCards));
}

void MainWindow::newCard() {
    if (!m_model->isConnected()) {
        return;
    }
    
    QStringList mailboxes = m_model->visibleMailboxes();
    if (mailboxes.isEmpty()) {
        QMessageBox::information(this, "New Card", "No visible mailboxes configured.");
        return;
    }
    
    bool ok;
    QString mailbox = QInputDialog::getItem(this, "New Card", "Select mailbox:", 
                                           mailboxes, 0, false, &ok);
    if (!ok || mailbox.isEmpty()) {
        return;
    }
    
    CardDialog dialog(this);
    dialog.setWindowTitle("New Card");
    
    if (dialog.exec() == QDialog::Accepted) {
        // In a real implementation, we would create a new email and send it to the mailbox
        // For now, we'll show a message that this feature needs SMTP support
        QMessageBox::information(this, "New Card", 
            "Creating new cards requires SMTP support, which will be added in a future version.");
    }
}

void MainWindow::editCard() {
    EmailCard card = m_kanbanBoard->selectedCard();
    if (!card.isValid()) {
        QMessageBox::information(this, "Edit Card", "Please select a card to edit.");
        return;
    }
    
    CardDialog dialog(this);
    dialog.setWindowTitle("Edit Card");
    dialog.setCard(card);
    
    if (dialog.exec() == QDialog::Accepted) {
        // In a real implementation, we would update the email
        QMessageBox::information(this, "Edit Card", 
            "Editing cards requires SMTP support, which will be added in a future version.");
    }
}

void MainWindow::deleteCard() {
    EmailCard card = m_kanbanBoard->selectedCard();
    QString mailbox = m_kanbanBoard->selectedMailbox();
    
    if (!card.isValid() || mailbox.isEmpty()) {
        QMessageBox::information(this, "Delete Card", "Please select a card to delete.");
        return;
    }
    
    int ret = QMessageBox::question(this, "Delete Card", 
        QString("Are you sure you want to delete the card '%1'?").arg(card.subject()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        if (!m_model->deleteCard(card.uid(), mailbox)) {
            QMessageBox::warning(this, "Delete Card", 
                QString("Failed to delete card: %1").arg(m_model->lastError()));
        }
    }
}

void MainWindow::moveCard() {
    EmailCard card = m_kanbanBoard->selectedCard();
    QString fromMailbox = m_kanbanBoard->selectedMailbox();
    
    if (!card.isValid() || fromMailbox.isEmpty()) {
        QMessageBox::information(this, "Move Card", "Please select a card to move.");
        return;
    }
    
    QStringList mailboxes = m_model->visibleMailboxes();
    mailboxes.removeAll(fromMailbox);
    
    if (mailboxes.isEmpty()) {
        QMessageBox::information(this, "Move Card", "No other mailboxes available.");
        return;
    }
    
    bool ok;
    QString toMailbox = QInputDialog::getItem(this, "Move Card", 
        QString("Move '%1' to:").arg(card.subject()), 
        mailboxes, 0, false, &ok);
    
    if (ok && !toMailbox.isEmpty()) {
        if (!m_model->moveCard(card.uid(), fromMailbox, toMailbox)) {
            QMessageBox::warning(this, "Move Card", 
                QString("Failed to move card: %1").arg(m_model->lastError()));
        }
    }
}

void MainWindow::markAsRead() {
    EmailCard card = m_kanbanBoard->selectedCard();
    QString mailbox = m_kanbanBoard->selectedMailbox();
    
    if (!card.isValid() || mailbox.isEmpty()) {
        return;
    }
    
    m_model->markCardAsRead(card.uid(), mailbox, true);
}

void MainWindow::markAsUnread() {
    EmailCard card = m_kanbanBoard->selectedCard();
    QString mailbox = m_kanbanBoard->selectedMailbox();
    
    if (!card.isValid() || mailbox.isEmpty()) {
        return;
    }
    
    m_model->markCardAsRead(card.uid(), mailbox, false);
}

void MainWindow::flagCard() {
    EmailCard card = m_kanbanBoard->selectedCard();
    QString mailbox = m_kanbanBoard->selectedMailbox();
    
    if (!card.isValid() || mailbox.isEmpty()) {
        return;
    }
    
    m_model->markCardAsFlagged(card.uid(), mailbox, true);
}

void MainWindow::unflagCard() {
    EmailCard card = m_kanbanBoard->selectedCard();
    QString mailbox = m_kanbanBoard->selectedMailbox();
    
    if (!card.isValid() || mailbox.isEmpty()) {
        return;
    }
    
    m_model->markCardAsFlagged(card.uid(), mailbox, false);
}

void MainWindow::refresh() {
    if (m_model->isConnected()) {
        m_model->refreshAll();
        statusBar()->showMessage("Refreshed", 2000);
    }
}

void MainWindow::showSettings() {
    if (!m_settingsDialog) {
        m_settingsDialog = new SettingsDialog(m_model, this);
    }
    
    m_settingsDialog->exec();
}

void MainWindow::connectToServer() {
    const Settings& settings = m_model->settings();
    
    if (settings.imapServer().isEmpty() || settings.username().isEmpty()) {
        QMessageBox::information(this, "Connect", 
            "Please configure IMAP settings first.");
        showSettings();
        return;
    }
    
    if (!m_model->connectToServer()) {
        QMessageBox::warning(this, "Connect", 
            QString("Failed to connect: %1").arg(m_model->lastError()));
    } else {
        statusBar()->showMessage("Connecting...", 3000);
    }
}

void MainWindow::disconnectFromServer() {
    m_model->disconnectFromServer();
}

void MainWindow::about() {
    QMessageBox::about(this, "About IMAP Kanban",
        "<h3>IMAP Kanban</h3>"
        "<p>Version 1.0.0</p>"
        "<p>A Kanban/Trello-style application that works entirely on top of IMAP.</p>"
        "<p>Different lists are mailboxes, and cards are emails.</p>"
        "<p><a href=\"https://github.com/your-repo/imap-kanban\">GitHub Repository</a></p>");
}

void MainWindow::quit() {
    close();
}