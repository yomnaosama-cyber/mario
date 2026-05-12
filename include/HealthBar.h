#ifndef HEALTHBAR_H
#define HEALTHBAR_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

class HealthBar : public QObject, public QGraphicsRectItem {
    Q_OBJECT
    
private:
    int currentHealth;
    int maxHealth;
    QGraphicsRectItem* fillBar;
    QGraphicsTextItem* label;
    QString ownerName;
    
public:
    HealthBar(const QString& name, int x, int y, int maxHp);
    ~HealthBar();
    
    void updateHealth(int current, int max);
    void show();
    void hide();
};

#endif
