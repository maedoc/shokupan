#pragma once

#include "../core/kanban_model.h"
#include "card_widget.h"
#include <QWidget>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QList>

class MailboxColumn : public QFrame {
    Q_OBJECT

public:
    explicit MailboxColumn(const QString& mailboxName, QWidget* parent = nullptr);
    
    QString mailboxName() const;
    void setMailboxName(const QString& name);
    
    void addCard(CardWidget* card);
    void removeCard(CardWidget* card);
    void clearCards();
    
    QList<CardWidget*> cards() const;
    CardWidget* selectedCard() const;
    
    void updateCardCount();

signals:
    void cardSelected(CardWidget* card);
    void cardDoubleClicked(CardWidget* card);

private slots:
    void onCardSelected();
    void onCardDoubleClicked();

private:
    void setupUI();
    
    QString m_mailboxName;
    QLabel* m_titleLabel;
    QLabel* m_countLabel;
    QVBoxLayout* m_cardsLayout;
    QWidget* m_cardsWidget;
    QScrollArea* m_scrollArea;
    QList<CardWidget*> m_cards;
    CardWidget* m_selectedCard;
};

class KanbanBoard : public QWidget {
    Q_OBJECT

public:
    explicit KanbanBoard(KanbanModel* model, QWidget* parent = nullptr);
    
    EmailCard selectedCard() const;
    QString selectedMailbox() const;

signals:
    void cardSelected(const EmailCard& card, const QString& mailbox);
    void cardDoubleClicked(const EmailCard& card, const QString& mailbox);

private slots:
    void onConnected();
    void onDisconnected();
    void onMailboxesChanged();
    void onMailboxUpdated(const QString& mailbox);
    void onCardSelected(CardWidget* card);
    void onCardDoubleClicked(CardWidget* card);

private:
    void setupUI();
    void updateColumns();
    void updateColumn(const QString& mailbox);
    MailboxColumn* findColumn(const QString& mailbox);
    
    KanbanModel* m_model;
    QHBoxLayout* m_layout;
    QScrollArea* m_scrollArea;
    QWidget* m_columnsWidget;
    QHBoxLayout* m_columnsLayout;
    
    QList<MailboxColumn*> m_columns;
    CardWidget* m_selectedCard;
    QString m_selectedMailbox;
};