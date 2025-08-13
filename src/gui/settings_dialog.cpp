#include "settings_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QSplitter>

SettingsDialog::SettingsDialog(KanbanModel* model, QWidget* parent)
    : QDialog(parent)
    , m_model(model)
{
    setupUI();
    loadSettings();
    
    connect(m_testButton, &QPushButton::clicked, this, &SettingsDialog::onTestConnection);
    connect(m_refreshButton, &QPushButton::clicked, this, &SettingsDialog::onRefreshMailboxes);
    connect(m_addButton, &QPushButton::clicked, this, [this]() {
        QListWidgetItem* item = m_availableMailboxes->currentItem();
        if (item) {
            QString mailbox = item->text();
            if (m_visibleMailboxes->findItems(mailbox, Qt::MatchExactly).isEmpty()) {
                m_visibleMailboxes->addItem(mailbox);
            }
        }
    });
    connect(m_removeButton, &QPushButton::clicked, this, [this]() {
        QListWidgetItem* item = m_visibleMailboxes->currentItem();
        if (item) {
            delete item;
        }
    });
}

void SettingsDialog::setupUI() {
    setWindowTitle("Settings");
    setModal(true);
    resize(600, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QTabWidget* tabWidget = new QTabWidget;
    
    // Connection tab
    QWidget* connectionTab = new QWidget;
    QVBoxLayout* connectionLayout = new QVBoxLayout(connectionTab);
    
    QGroupBox* serverGroup = new QGroupBox("IMAP Server");
    QFormLayout* serverLayout = new QFormLayout(serverGroup);
    
    m_serverEdit = new QLineEdit;
    m_serverEdit->setPlaceholderText("imap.gmail.com");
    serverLayout->addRow("Server:", m_serverEdit);
    
    m_portSpinBox = new QSpinBox;
    m_portSpinBox->setRange(1, 65535);
    m_portSpinBox->setValue(993);
    serverLayout->addRow("Port:", m_portSpinBox);
    
    m_sslCheckBox = new QCheckBox("Use SSL/TLS");
    m_sslCheckBox->setChecked(true);
    serverLayout->addRow(m_sslCheckBox);
    
    connectionLayout->addWidget(serverGroup);
    
    QGroupBox* authGroup = new QGroupBox("Authentication");
    QFormLayout* authLayout = new QFormLayout(authGroup);
    
    m_usernameEdit = new QLineEdit;
    m_usernameEdit->setPlaceholderText("your.email@gmail.com");
    authLayout->addRow("Username:", m_usernameEdit);
    
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("Your password or app password");
    authLayout->addRow("Password:", m_passwordEdit);
    
    connectionLayout->addWidget(authGroup);
    
    m_testButton = new QPushButton("Test Connection");
    connectionLayout->addWidget(m_testButton);
    
    connectionLayout->addStretch();
    
    tabWidget->addTab(connectionTab, "Connection");
    
    // Mailboxes tab
    QWidget* mailboxesTab = new QWidget;
    QVBoxLayout* mailboxesLayout = new QVBoxLayout(mailboxesTab);
    
    QLabel* mailboxesLabel = new QLabel("Select which mailboxes to show as Kanban columns:");
    mailboxesLayout->addWidget(mailboxesLabel);
    
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    
    // Available mailboxes
    QWidget* availableWidget = new QWidget;
    QVBoxLayout* availableLayout = new QVBoxLayout(availableWidget);
    availableLayout->addWidget(new QLabel("Available Mailboxes:"));
    
    m_availableMailboxes = new QListWidget;
    availableLayout->addWidget(m_availableMailboxes);
    
    m_refreshButton = new QPushButton("Refresh");
    availableLayout->addWidget(m_refreshButton);
    
    splitter->addWidget(availableWidget);
    
    // Control buttons
    QWidget* controlWidget = new QWidget;
    QVBoxLayout* controlLayout = new QVBoxLayout(controlWidget);
    controlLayout->addStretch();
    
    m_addButton = new QPushButton("Add →");
    m_removeButton = new QPushButton("← Remove");
    
    controlLayout->addWidget(m_addButton);
    controlLayout->addWidget(m_removeButton);
    controlLayout->addStretch();
    
    splitter->addWidget(controlWidget);
    
    // Visible mailboxes
    QWidget* visibleWidget = new QWidget;
    QVBoxLayout* visibleLayout = new QVBoxLayout(visibleWidget);
    visibleLayout->addWidget(new QLabel("Visible Mailboxes:"));
    
    m_visibleMailboxes = new QListWidget;
    visibleLayout->addWidget(m_visibleMailboxes);
    
    splitter->addWidget(visibleWidget);
    
    splitter->setSizes({200, 100, 200});
    mailboxesLayout->addWidget(splitter);
    
    tabWidget->addTab(mailboxesTab, "Mailboxes");
    
    // General tab
    QWidget* generalTab = new QWidget;
    QVBoxLayout* generalLayout = new QVBoxLayout(generalTab);
    
    QGroupBox* refreshGroup = new QGroupBox("Auto Refresh");
    QFormLayout* refreshLayout = new QFormLayout(refreshGroup);
    
    m_autoRefreshCheckBox = new QCheckBox("Enable auto refresh");
    refreshLayout->addRow(m_autoRefreshCheckBox);
    
    m_refreshIntervalSpinBox = new QSpinBox;
    m_refreshIntervalSpinBox->setRange(10, 3600);
    m_refreshIntervalSpinBox->setSuffix(" seconds");
    m_refreshIntervalSpinBox->setValue(30);
    refreshLayout->addRow("Interval:", m_refreshIntervalSpinBox);
    
    generalLayout->addWidget(refreshGroup);
    generalLayout->addStretch();
    
    tabWidget->addTab(generalTab, "General");
    
    mainLayout->addWidget(tabWidget);
    
    // Dialog buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::onRejected);
    
    mainLayout->addWidget(buttonBox);
}

void SettingsDialog::loadSettings() {
    const Settings& settings = m_model->settings();
    
    m_serverEdit->setText(settings.imapServer());
    m_portSpinBox->setValue(settings.imapPort());
    m_sslCheckBox->setChecked(settings.useSSL());
    m_usernameEdit->setText(settings.username());
    m_passwordEdit->setText(settings.password());
    
    m_refreshIntervalSpinBox->setValue(settings.refreshInterval());
    m_autoRefreshCheckBox->setChecked(m_model->autoRefreshEnabled());
    
    updateMailboxList();
}

void SettingsDialog::saveSettings() {
    Settings& settings = m_model->settings();
    
    settings.setImapServer(m_serverEdit->text().trimmed());
    settings.setImapPort(m_portSpinBox->value());
    settings.setUseSSL(m_sslCheckBox->isChecked());
    settings.setUsername(m_usernameEdit->text().trimmed());
    settings.setPassword(m_passwordEdit->text());
    
    settings.setRefreshInterval(m_refreshIntervalSpinBox->value());
    m_model->setAutoRefresh(m_autoRefreshCheckBox->isChecked());
    
    // Save visible mailboxes
    QStringList visibleMailboxes;
    for (int i = 0; i < m_visibleMailboxes->count(); ++i) {
        visibleMailboxes.append(m_visibleMailboxes->item(i)->text());
    }
    settings.setVisibleMailboxes(visibleMailboxes);
    
    m_model->saveSettings();
}

void SettingsDialog::updateMailboxList() {
    m_availableMailboxes->clear();
    m_visibleMailboxes->clear();
    
    QStringList available = m_model->availableMailboxes();
    QStringList visible = m_model->visibleMailboxes();
    
    for (const QString& mailbox : available) {
        m_availableMailboxes->addItem(mailbox);
    }
    
    for (const QString& mailbox : visible) {
        m_visibleMailboxes->addItem(mailbox);
    }
}

void SettingsDialog::onTestConnection() {
    // Test connection (this would require a separate test method in the model)
    QMessageBox::information(this, "Test Connection", 
        "Connection testing is not yet implemented. "
        "Please save settings and try connecting from the main window.");
}

void SettingsDialog::onRefreshMailboxes() {
    if (!m_model->isConnected()) {
        QMessageBox::information(this, "Refresh Mailboxes", 
            "Please connect to the IMAP server first.");
        return;
    }
    // Force model to reload mailboxes from server
    m_model->reloadMailboxes();
    updateMailboxList();
    QMessageBox::information(this, "Refresh Mailboxes", "Mailbox list refreshed.");
}

void SettingsDialog::onAccepted() {
    saveSettings();
    accept();
}

void SettingsDialog::onRejected() {
    reject();
}