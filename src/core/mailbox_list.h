#pragma once

#include "email_card.h"
#include <QString>
#include <QList>

class MailboxList {
public:
    MailboxList();
    MailboxList(const QString& name);
    
    // Basic properties
    QString name() const;
    void setName(const QString& name);
    
    QString displayName() const;
    void setDisplayName(const QString& displayName);
    
    // Card management
    void addCard(const EmailCard& card);
    void removeCard(const QString& uid);
    void updateCard(const EmailCard& card);
    EmailCard card(const QString& uid) const;
    QList<EmailCard> cards() const;
    void setCards(const QList<EmailCard>& cards);
    
    // Utility
    int cardCount() const;
    bool hasCard(const QString& uid) const;
    void clear();
    
    // Sorting
    enum SortOrder {
        DateAscending,
        DateDescending,
        SubjectAscending,
        SubjectDescending,
        FromAscending,
        FromDescending
    };
    
    void sortCards(SortOrder order);
    
private:
    QString m_name;
    QString m_displayName;
    QList<EmailCard> m_cards;
    
    int findCardIndex(const QString& uid) const;
};