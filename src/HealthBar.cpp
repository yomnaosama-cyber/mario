#include "HealthBar.h"
#include <QBrush>
#include <QPen>
#include <QFont>

HealthBar::HealthBar(const QString& name, int x, int y, int maxHp)
    : currentHealth(maxHp), maxHealth(maxHp), ownerName(name) {
    
    // Background (dark red)
    setRect(x, y, 300, 30);
    setBrush(QBrush(Qt::darkRed));
    setPen(QPen(Qt::black, 2));
    setZValue(200);
    
    // Fill bar (green)
    fillBar = new QGraphicsRectItem(x, y, 300, 30, this);
    fillBar->setBrush(QBrush(Qt::green));
    fillBar->setPen(QPen(Qt::NoPen));
    fillBar->setZValue(201);
    
    // Label - name only, no numbers
    label = new QGraphicsTextItem(name, this);
    label->setDefaultTextColor(Qt::white);
    label->setFont(QFont("Arial", 12, QFont::Bold));
    label->setPos(x + 10, y + 5);
    label->setZValue(202);
    
    hide();
}

HealthBar::~HealthBar() {
    delete fillBar;
    delete label;
}

void HealthBar::updateHealth(int current, int max) {
    currentHealth = current;
    maxHealth = max;
    
    int width = static_cast<int>(300.0 * current / max);
    fillBar->setRect(rect().x(), rect().y(), width, 30);
    label->setPlainText(ownerName);
    
    // Change color based on health percentage
    if (current > max * 0.6) {
        fillBar->setBrush(QBrush(Qt::green));
    } else if (current > max * 0.3) {
        fillBar->setBrush(QBrush(Qt::yellow));
    } else {
        fillBar->setBrush(QBrush(Qt::red));
    }
}

void HealthBar::show() {
    QGraphicsRectItem::show();
    fillBar->show();
    label->show();
}

void HealthBar::hide() {
    QGraphicsRectItem::hide();
    fillBar->hide();
    label->hide();
}