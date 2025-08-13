#include "card_widget.h"
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QDateTime>

CardWidget::CardWidget(const EmailCard& card, QWidget* parent)
    : QFrame(parent)
    , m_card(card)
    , m_selected(false)
{
    setupUI();
    updateDisplay();
    updateStyle();
}

EmailCard CardWidget::card() const {
    return m_card;
}

void CardWidget::setCard(const EmailCard& card) {
    m_card = card;
    updateDisplay();
    updateStyle();
}

bool CardWidget::isSelected() const {
    return m_selected;
}

void CardWidget::setSelected(bool selected) {
    if (m_selected != selected) {
        m_selected = selected;
        updateStyle();
        update();
    }
}

void CardWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setSelected(true);
        emit selected();
    }
    QFrame::mousePressEvent(event);
}

void CardWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();
    }
    QFrame::mouseDoubleClickEvent(event);
}

void CardWidget::paintEvent(QPaintEvent* event) {
    QFrame::paintEvent(event);
    
    // Draw selection border
    if (m_selected) {
        QPainter painter(this);
        painter.setPen(QPen(QColor(0, 120, 215), 2));
        painter.drawRect(rect().adjusted(1, 1, -1, -1));
    }
}

void CardWidget::setupUI() {
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setMinimumHeight(80);
    setMaximumHeight(120);
    setCursor(Qt::PointingHandCursor);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(8, 6, 8, 6);
    
    // Subject
    m_subjectLabel = new QLabel;
    m_subjectLabel->setWordWrap(true);
    m_subjectLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    layout->addWidget(m_subjectLabel);
    
    // From
    m_fromLabel = new QLabel;
    m_fromLabel->setStyleSheet("color: #666; font-size: 10px;");
    layout->addWidget(m_fromLabel);
    
    // Date and status
    QWidget* bottomWidget = new QWidget;
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(5);
    
    m_dateLabel = new QLabel;
    m_dateLabel->setStyleSheet("color: #888; font-size: 9px;");
    
    m_statusLabel = new QLabel;
    m_statusLabel->setStyleSheet("font-size: 9px;");
    
    bottomLayout->addWidget(m_dateLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_statusLabel);
    
    layout->addWidget(bottomWidget);
    layout->addStretch();
}

void CardWidget::updateDisplay() {
    // Subject
    QString subject = m_card.subject();
    if (subject.isEmpty()) {
        subject = "(No Subject)";
    }
    m_subjectLabel->setText(truncateText(subject, 60));
    m_subjectLabel->setToolTip(subject);
    
    // From
    QString from = m_card.from();
    if (from.isEmpty()) {
        from = "(Unknown Sender)";
    }
    m_fromLabel->setText(truncateText(from, 40));
    m_fromLabel->setToolTip(from);
    
    // Date
    m_dateLabel->setText(formatDate(m_card.date()));
    
    // Status indicators
    QStringList statusItems;
    if (!m_card.isRead()) {
        statusItems << "●"; // Unread indicator
    }
    if (m_card.isFlagged()) {
        statusItems << "🚩"; // Flag indicator
    }
    
    m_statusLabel->setText(statusItems.join(" "));
}

void CardWidget::updateStyle() {
    QString baseStyle = "CardWidget { "
                       "background-color: white; "
                       "border: 1px solid #ddd; "
                       "border-radius: 4px; "
                       "margin: 2px; "
                       "}";
    
    QString hoverStyle = "CardWidget:hover { "
                        "background-color: #f5f5f5; "
                        "border-color: #bbb; "
                        "}";
    
    // Different background for unread cards
    if (!m_card.isRead()) {
        baseStyle = "CardWidget { "
                   "background-color: #f0f8ff; "
                   "border: 1px solid #ddd; "
                   "border-radius: 4px; "
                   "margin: 2px; "
                   "}";
        
        hoverStyle = "CardWidget:hover { "
                    "background-color: #e6f3ff; "
                    "border-color: #bbb; "
                    "}";
    }
    
    setStyleSheet(baseStyle + hoverStyle);
}

QString CardWidget::formatDate(const QDateTime& date) const {
    if (!date.isValid()) {
        return "";
    }
    
    QDateTime now = QDateTime::currentDateTime();
    qint64 secondsAgo = date.secsTo(now);
    
    if (secondsAgo < 60) {
        return "now";
    } else if (secondsAgo < 3600) {
        int minutes = secondsAgo / 60;
        return QString("%1m ago").arg(minutes);
    } else if (secondsAgo < 86400) {
        int hours = secondsAgo / 3600;
        return QString("%1h ago").arg(hours);
    } else if (secondsAgo < 604800) {
        int days = secondsAgo / 86400;
        return QString("%1d ago").arg(days);
    } else {
        return date.toString("MMM dd");
    }
}

QString CardWidget::truncateText(const QString& text, int maxLength) const {
    if (text.length() <= maxLength) {
        return text;
    }
    
    return text.left(maxLength - 3) + "...";
}