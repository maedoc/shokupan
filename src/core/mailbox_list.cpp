#include "mailbox_list.h"
#include <algorithm>

MailboxList::MailboxList() {
}

MailboxList::MailboxList(const QString& name) 
    : m_name(name)
    , m_displayName(name)
{
}

QString MailboxList::name() const {
    return m_name;
}

void MailboxList::setName(const QString& name) {
    m_name = name;
    if (m_displayName.isEmpty()) {
        m_displayName = name;
    }
}

QString MailboxList::displayName() const {
    return m_displayName.isEmpty() ? m_name : m_displayName;
}

void MailboxList::setDisplayName(const QString& displayName) {
    m_displayName = displayName;
}

void MailboxList::addCard(const EmailCard& card) {
    if (!card.isValid()) {
        return;
    }
    
    int index = findCardIndex(card.uid());
    if (index >= 0) {
        m_cards[index] = card;
    } else {
        m_cards.append(card);
    }
}

void MailboxList::removeCard(const QString& uid) {
    int index = findCardIndex(uid);
    if (index >= 0) {
        m_cards.removeAt(index);
    }
}

void MailboxList::updateCard(const EmailCard& card) {
    addCard(card); // addCard handles both add and update
}

EmailCard MailboxList::card(const QString& uid) const {
    int index = findCardIndex(uid);
    if (index >= 0) {
        return m_cards[index];
    }
    return EmailCard();
}

QList<EmailCard> MailboxList::cards() const {
    return m_cards;
}

void MailboxList::setCards(const QList<EmailCard>& cards) {
    m_cards = cards;
}

int MailboxList::cardCount() const {
    return m_cards.size();
}

bool MailboxList::hasCard(const QString& uid) const {
    return findCardIndex(uid) >= 0;
}

void MailboxList::clear() {
    m_cards.clear();
}

void MailboxList::sortCards(SortOrder order) {
    std::sort(m_cards.begin(), m_cards.end(), [order](const EmailCard& a, const EmailCard& b) {
        switch (order) {
            case DateAscending:
                return a.date() < b.date();
            case DateDescending:
                return a.date() > b.date();
            case SubjectAscending:
                return a.subject().toLower() < b.subject().toLower();
            case SubjectDescending:
                return a.subject().toLower() > b.subject().toLower();
            case FromAscending:
                return a.from().toLower() < b.from().toLower();
            case FromDescending:
                return a.from().toLower() > b.from().toLower();
            default:
                return a.date() > b.date();
        }
    });
}

int MailboxList::findCardIndex(const QString& uid) const {
    for (int i = 0; i < m_cards.size(); ++i) {
        if (m_cards[i].uid() == uid) {
            return i;
        }
    }
    return -1;
}