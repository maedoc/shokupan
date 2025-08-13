#pragma once

#include <QString>
#include <QDateTime>
#include <QStringList>

class EmailCard {
public:
    EmailCard();
    EmailCard(const QString& uid, const QString& subject, const QString& from, 
              const QDateTime& date, const QString& body = QString());
    
    // Getters
    QString uid() const;
    QString subject() const;
    QString from() const;
    QString to() const;
    QDateTime date() const;
    QString body() const;
    QStringList flags() const;
    bool isRead() const;
    bool isFlagged() const;
    
    // Setters
    void setUid(const QString& uid);
    void setSubject(const QString& subject);
    void setFrom(const QString& from);
    void setTo(const QString& to);
    void setDate(const QDateTime& date);
    void setBody(const QString& body);
    void setFlags(const QStringList& flags);
    void setRead(bool read);
    void setFlagged(bool flagged);
    
    // Utility
    QString summary() const;
    bool isValid() const;
    
private:
    QString m_uid;
    QString m_subject;
    QString m_from;
    QString m_to;
    QDateTime m_date;
    QString m_body;
    QStringList m_flags;
    bool m_read;
    bool m_flagged;
};