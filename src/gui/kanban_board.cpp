#include "kanban_board.h"
#include <QScrollBar>
#include <QApplication>

// MailboxColumn implementation

MailboxColumn::MailboxColumn(const QString& mailboxName, QWidget* parent)
    : QFrame(parent)
    , m_mailboxName(mailboxName)
    , m_selectedCard(nullptr)
{
    setupUI();
}

QString MailboxColumn::mailboxName() const {
    return m_mailboxName;
}

void MailboxColumn::setMailboxName(const QString& name) {
    m_mailboxName = name;
    m_titleLabel->setText(name);
}

void MailboxColumn::addCard(CardWidget* card) {
    if (!card || m_cards.contains(card)) {
        return;
    }
    
    m_cards.append(card);
    m_cardsLayout->addWidget(card);
    
    connect(card, &CardWidget::selected, this, &MailboxColumn::onCardSelected);
    connect(card, &CardWidget::doubleClicked, this, &MailboxColumn::onCardDoubleClicked);
    
    updateCardCount();
}

void MailboxColumn::removeCard(CardWidget* card) {
    if (!card || !m_cards.contains(card)) {
        return;
    }
    
    m_cards.removeAll(card);
    m_cardsLayout->removeWidget(card);
    card->setParent(nullptr);
    
    if (m_selectedCard == card) {
        m_selectedCard = nullptr;
    }
    
    updateCardCount();
}

void MailboxColumn::clearCards() {
    for (CardWidget* card : m_cards) {
        m_cardsLayout->removeWidget(card);
        card->deleteLater();
    }
    m_cards.clear();
    m_selectedCard = nullptr;
    updateCardCount();
}

QList<CardWidget*> MailboxColumn::cards() const {
    return m_cards;
}

CardWidget* MailboxColumn::selectedCard() const {
    return m_selectedCard;
}

void MailboxColumn::updateCardCount() {
    m_countLabel->setText(QString("(%1)").arg(m_cards.size()));
}

void MailboxColumn::onCardSelected() {
    CardWidget* card = qobject_cast<CardWidget*>(sender());
    if (!card) {
        return;
    }
    
    // Deselect previous card
    if (m_selectedCard && m_selectedCard != card) {
        m_selectedCard->setSelected(false);
    }
    
    m_selectedCard = card;
    emit cardSelected(card);
}

void MailboxColumn::onCardDoubleClicked() {
    CardWidget* card = qobject_cast<CardWidget*>(sender());
    if (card) {
        emit cardDoubleClicked(card);
    }
}

void MailboxColumn::setupUI() {
    setFrameStyle(QFrame::StyledPanel);
    setMinimumWidth(300);
    setMaximumWidth(400);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(5);
    layout->setContentsMargins(5, 5, 5, 5);
    
    // Header
    QWidget* headerWidget = new QWidget;
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    m_titleLabel = new QLabel(m_mailboxName);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    m_countLabel = new QLabel("(0)");
    m_countLabel->setStyleSheet("color: gray;");
    
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addWidget(m_countLabel);
    headerLayout->addStretch();
    
    layout->addWidget(headerWidget);
    
    // Cards area
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_cardsWidget = new QWidget;
    m_cardsLayout = new QVBoxLayout(m_cardsWidget);
    m_cardsLayout->setSpacing(5);
    m_cardsLayout->setContentsMargins(0, 0, 0, 0);
    m_cardsLayout->addStretch();
    
    m_scrollArea->setWidget(m_cardsWidget);
    layout->addWidget(m_scrollArea);
}

// KanbanBoard implementation

KanbanBoard::KanbanBoard(KanbanModel* model, QWidget* parent)
    : QWidget(parent)
    , m_model(model)
    , m_selectedCard(nullptr)
{
    setupUI();
    
    // Defensive: log if model is null before connecting
#ifdef QT_DEBUG
    if (!m_model) {
        qDebug() << "KanbanBoard: m_model is null when connecting signals";
    }
#endif
    connect(m_model, &KanbanModel::connected, this, &KanbanBoard::onConnected);
    connect(m_model, &KanbanModel::disconnected, this, &KanbanBoard::onDisconnected);
    connect(m_model, &KanbanModel::mailboxesChanged, this, &KanbanBoard::onMailboxesChanged);
    connect(m_model, &KanbanModel::mailboxUpdated, this, &KanbanBoard::onMailboxUpdated);
}

EmailCard KanbanBoard::selectedCard() const {
    if (m_selectedCard) {
        return m_selectedCard->card();
    }
    return EmailCard();
}

QString KanbanBoard::selectedMailbox() const {
    return m_selectedMailbox;
}

void KanbanBoard::onConnected() {
    updateColumns();
}

void KanbanBoard::onDisconnected() {
    // Clear all columns
    for (MailboxColumn* column : m_columns) {
        column->clearCards();
        m_columnsLayout->removeWidget(column);
        column->deleteLater();
    }
    m_columns.clear();
    m_selectedCard = nullptr;
    m_selectedMailbox.clear();
}

void KanbanBoard::onMailboxesChanged() {
    updateColumns();
}

void KanbanBoard::onMailboxUpdated(const QString& mailbox) {
    updateColumn(mailbox);
}

void KanbanBoard::onCardSelected(CardWidget* card) {
    // Deselect cards in other columns
    for (MailboxColumn* column : m_columns) {
        if (column->selectedCard() != card) {
            for (CardWidget* otherCard : column->cards()) {
                if (otherCard != card) {
                    otherCard->setSelected(false);
                }
            }
        }
    }
    
    m_selectedCard = card;
    
    // Find which mailbox this card belongs to
    for (MailboxColumn* column : m_columns) {
        if (column->cards().contains(card)) {
            m_selectedMailbox = column->mailboxName();
            break;
        }
    }
    
    if (card) {
        emit cardSelected(card->card(), m_selectedMailbox);
    }
}

void KanbanBoard::onCardDoubleClicked(CardWidget* card) {
    if (card) {
        // Find which mailbox this card belongs to
        QString mailbox;
        for (MailboxColumn* column : m_columns) {
            if (column->cards().contains(card)) {
                mailbox = column->mailboxName();
                break;
            }
        }
        
        emit cardDoubleClicked(card->card(), mailbox);
    }
}

void KanbanBoard::setupUI() {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_columnsWidget = new QWidget;
    m_columnsLayout = new QHBoxLayout(m_columnsWidget);
    m_columnsLayout->setSpacing(10);
    m_columnsLayout->setContentsMargins(10, 10, 10, 10);
    m_columnsLayout->addStretch();
    
    m_scrollArea->setWidget(m_columnsWidget);
    m_layout->addWidget(m_scrollArea);
}

void KanbanBoard::updateColumns() {
    if (!m_model->isConnected()) {
        return;
    }
    
    QStringList visibleMailboxes = m_model->visibleMailboxes();
    
    // Remove columns that are no longer visible
    for (int i = m_columns.size() - 1; i >= 0; --i) {
        MailboxColumn* column = m_columns[i];
        if (!visibleMailboxes.contains(column->mailboxName())) {
            m_columns.removeAt(i);
            m_columnsLayout->removeWidget(column);
            column->deleteLater();
        }
    }
    
    // Add new columns
    for (const QString& mailbox : visibleMailboxes) {
        if (!findColumn(mailbox)) {
            MailboxColumn* column = new MailboxColumn(mailbox);
            connect(column, &MailboxColumn::cardSelected, this, &KanbanBoard::onCardSelected);
            connect(column, &MailboxColumn::cardDoubleClicked, this, &KanbanBoard::onCardDoubleClicked);
            
            m_columns.append(column);
            m_columnsLayout->insertWidget(m_columnsLayout->count() - 1, column);
            
            // Load cards for this mailbox
            updateColumn(mailbox);
        }
    }
}

void KanbanBoard::updateColumn(const QString& mailbox) {
    MailboxColumn* column = findColumn(mailbox);
    if (!column) {
        return;
    }
    
    // Clear existing cards
    column->clearCards();
    
    // Add new cards
    MailboxList list = m_model->mailboxList(mailbox);
    const QList<EmailCard> cards = list.cards();
    
    for (const EmailCard& card : cards) {
        CardWidget* cardWidget = new CardWidget(card);
        column->addCard(cardWidget);
    }
}

MailboxColumn* KanbanBoard::findColumn(const QString& mailbox) {
    for (MailboxColumn* column : m_columns) {
        if (column->mailboxName() == mailbox) {
            return column;
        }
    }
    return nullptr;
}