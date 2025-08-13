#pragma once

#include "../core/kanban_model.h"
#include "kanban_board.h"
#include "settings_dialog.h"
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QTimer>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onConnected();
    void onDisconnected();
    void onError(const QString& message);
    void onMailboxUpdated(const QString& mailbox);
    
    // Menu actions
    void newCard();
    void editCard();
    void deleteCard();
    void moveCard();
    void markAsRead();
    void markAsUnread();
    void flagCard();
    void unflagCard();
    void refresh();
    void showSettings();
    void connectToServer();
    void disconnectFromServer();
    void about();
    void quit();

private:
    void setupUI();
    void setupMenus();
    void setupStatusBar();
    void setupKeyboardShortcuts();
    void updateConnectionStatus();
    void updateWindowTitle();
    
    KanbanModel* m_model;
    KanbanBoard* m_kanbanBoard;
    SettingsDialog* m_settingsDialog;
    
    // Status bar
    QLabel* m_connectionStatusLabel;
    QLabel* m_mailboxCountLabel;
    QLabel* m_cardCountLabel;
    
    // Menus and actions
    QAction* m_newCardAction;
    QAction* m_editCardAction;
    QAction* m_deleteCardAction;
    QAction* m_moveCardAction;
    QAction* m_markReadAction;
    QAction* m_markUnreadAction;
    QAction* m_flagAction;
    QAction* m_unflagAction;
    QAction* m_refreshAction;
    QAction* m_settingsAction;
    QAction* m_connectAction;
    QAction* m_disconnectAction;
    QAction* m_quitAction;
    QAction* m_aboutAction;
};