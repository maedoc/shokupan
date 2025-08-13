#pragma once

#include "../core/email_card.h"
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QCheckBox>

class CardDialog : public QDialog {
    Q_OBJECT

public:
    explicit CardDialog(QWidget* parent = nullptr);
    
    EmailCard card() const;
    void setCard(const EmailCard& card);

private slots:
    void onAccepted();
    void onRejected();

private:
    void setupUI();
    
    EmailCard m_card;
    
    QLineEdit* m_subjectEdit;
    QLineEdit* m_fromEdit;
    QLineEdit* m_toEdit;
    QTextEdit* m_bodyEdit;
    QLabel* m_dateLabel;
    QLabel* m_uidLabel;
    QCheckBox* m_readCheckBox;
    QCheckBox* m_flaggedCheckBox;
};