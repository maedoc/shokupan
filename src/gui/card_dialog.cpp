#include "card_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QGroupBox>

CardDialog::CardDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    
    connect(this, &QDialog::accepted, this, &CardDialog::onAccepted);
    connect(this, &QDialog::rejected, this, &CardDialog::onRejected);
}

EmailCard CardDialog::card() const {
    return m_card;
}

void CardDialog::setCard(const EmailCard& card) {
    m_card = card;
    
    m_subjectEdit->setText(card.subject());
    m_fromEdit->setText(card.from());
    m_toEdit->setText(card.to());
    m_bodyEdit->setPlainText(card.body());
    m_dateLabel->setText(card.date().toString("yyyy-MM-dd hh:mm:ss"));
    m_uidLabel->setText(card.uid());
    m_readCheckBox->setChecked(card.isRead());
    m_flaggedCheckBox->setChecked(card.isFlagged());
}

void CardDialog::setupUI() {
    setModal(true);
    resize(600, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Basic information
    QGroupBox* basicGroup = new QGroupBox("Email Information");
    QFormLayout* basicLayout = new QFormLayout(basicGroup);
    
    m_subjectEdit = new QLineEdit;
    basicLayout->addRow("Subject:", m_subjectEdit);
    
    m_fromEdit = new QLineEdit;
    basicLayout->addRow("From:", m_fromEdit);
    
    m_toEdit = new QLineEdit;
    basicLayout->addRow("To:", m_toEdit);
    
    m_dateLabel = new QLabel;
    basicLayout->addRow("Date:", m_dateLabel);
    
    m_uidLabel = new QLabel;
    basicLayout->addRow("UID:", m_uidLabel);
    
    mainLayout->addWidget(basicGroup);
    
    // Body
    QGroupBox* bodyGroup = new QGroupBox("Body");
    QVBoxLayout* bodyLayout = new QVBoxLayout(bodyGroup);
    
    m_bodyEdit = new QTextEdit;
    m_bodyEdit->setMinimumHeight(200);
    bodyLayout->addWidget(m_bodyEdit);
    
    mainLayout->addWidget(bodyGroup);
    
    // Flags
    QGroupBox* flagsGroup = new QGroupBox("Flags");
    QHBoxLayout* flagsLayout = new QHBoxLayout(flagsGroup);
    
    m_readCheckBox = new QCheckBox("Read");
    m_flaggedCheckBox = new QCheckBox("Flagged");
    
    flagsLayout->addWidget(m_readCheckBox);
    flagsLayout->addWidget(m_flaggedCheckBox);
    flagsLayout->addStretch();
    
    mainLayout->addWidget(flagsGroup);
    
    // Dialog buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    mainLayout->addWidget(buttonBox);
}

void CardDialog::onAccepted() {
    m_card.setSubject(m_subjectEdit->text());
    m_card.setFrom(m_fromEdit->text());
    m_card.setTo(m_toEdit->text());
    m_card.setBody(m_bodyEdit->toPlainText());
    m_card.setRead(m_readCheckBox->isChecked());
    m_card.setFlagged(m_flaggedCheckBox->isChecked());
}

void CardDialog::onRejected() {
    // Nothing to do on rejection
}