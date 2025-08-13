#include "email_card.h"

EmailCard::EmailCard() 
    : m_read(false)
    , m_flagged(false)
{
}

EmailCard::EmailCard(const QString& uid, const QString& subject, const QString& from, 
                     const QDateTime& date, const QString& body)
    : m_uid(uid)
    , m_subject(subject)
    , m_from(from)
    , m_date(date)
    , m_body(body)
    , m_read(false)
    , m_flagged(false)
{
}

QString EmailCard::uid() const {
    return m_uid;
}

QString EmailCard::subject() const {
    return m_subject;
}

QString EmailCard::from() const {
    return m_from;
}

QString EmailCard::to() const {
    return m_to;
}

QDateTime EmailCard::date() const {
    return m_date;
}

QString EmailCard::body() const {
    return m_body;
}

QStringList EmailCard::flags() const {
    return m_flags;
}

bool EmailCard::isRead() const {
    return m_read;
}

bool EmailCard::isFlagged() const {
    return m_flagged;
}

void EmailCard::setUid(const QString& uid) {
    m_uid = uid;
}

void EmailCard::setSubject(const QString& subject) {
    m_subject = subject;
}

void EmailCard::setFrom(const QString& from) {
    m_from = from;
}

void EmailCard::setTo(const QString& to) {
    m_to = to;
}

void EmailCard::setDate(const QDateTime& date) {
    m_date = date;
}

void EmailCard::setBody(const QString& body) {
    m_body = body;
}

void EmailCard::setFlags(const QStringList& flags) {
    m_flags = flags;
    m_read = flags.contains("\\Seen");
    m_flagged = flags.contains("\\Flagged");
}

void EmailCard::setRead(bool read) {
    m_read = read;
    if (read && !m_flags.contains("\\Seen")) {
        m_flags.append("\\Seen");
    } else if (!read) {
        m_flags.removeAll("\\Seen");
    }
}

void EmailCard::setFlagged(bool flagged) {
    m_flagged = flagged;
    if (flagged && !m_flags.contains("\\Flagged")) {
        m_flags.append("\\Flagged");
    } else if (!flagged) {
        m_flags.removeAll("\\Flagged");
    }
}

QString EmailCard::summary() const {
    QString summary = m_subject;
    if (summary.isEmpty()) {
        summary = "(No Subject)";
    }
    
    if (!m_from.isEmpty()) {
        summary += QString(" - %1").arg(m_from);
    }
    
    return summary;
}

bool EmailCard::isValid() const {
    return !m_uid.isEmpty();
}