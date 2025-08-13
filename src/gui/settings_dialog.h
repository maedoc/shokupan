#pragma once

#include "../core/kanban_model.h"
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QListWidget>
#include <QPushButton>
#include <QTabWidget>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(KanbanModel* model, QWidget* parent = nullptr);

private slots:
    void onTestConnection();
    void onRefreshMailboxes();
    void onAccepted();
    void onRejected();

private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void updateMailboxList();
    
    KanbanModel* m_model;
    
    // Connection tab
    QLineEdit* m_serverEdit;
    QSpinBox* m_portSpinBox;
    QCheckBox* m_sslCheckBox;
    QLineEdit* m_usernameEdit;
    QLineEdit* m_passwordEdit;
    QPushButton* m_testButton;
    
    // Mailboxes tab
    QListWidget* m_availableMailboxes;
    QListWidget* m_visibleMailboxes;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;
    QPushButton* m_refreshButton;
    
    // General tab
    QSpinBox* m_refreshIntervalSpinBox;
    QCheckBox* m_autoRefreshCheckBox;
};