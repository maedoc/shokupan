#pragma once

#include "../core/email_card.h"
#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QMouseEvent>

class CardWidget : public QFrame {
    Q_OBJECT

public:
    explicit CardWidget(const EmailCard& card, QWidget* parent = nullptr);
    
    EmailCard card() const;
    void setCard(const EmailCard& card);
    
    bool isSelected() const;
    void setSelected(bool selected);

signals:
    void selected();
    void doubleClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUI();
    void updateDisplay();
    void updateStyle();
    QString formatDate(const QDateTime& date) const;
    QString truncateText(const QString& text, int maxLength) const;
    
    EmailCard m_card;
    bool m_selected;
    
    QLabel* m_subjectLabel;
    QLabel* m_fromLabel;
    QLabel* m_dateLabel;
    QLabel* m_statusLabel;
};