#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsItem>
#include <QPainter>
#include <QPointF>
#include <QVector>

enum class EnemyType {
    Normal,
    Fast,
    Tank,
    Boss
};

class Enemy : public QGraphicsItem
{
public:
    Enemy(EnemyType type, const QVector<QPointF> &waypoints, QGraphicsItem *parent = nullptr);
    ~Enemy() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    void updateMovement(int deltaMs);

    void takeDamage(int damage);
    void applySlow(qreal factor, int durationMs);
    bool isDead() const { return m_hp <= 0; }
    bool hasReachedEnd() const { return m_reachedEnd; }

    int maxHp() const { return m_maxHp; }
    int hp() const { return m_hp; }
    int reward() const { return m_reward; }
    int livesCost() const { return m_livesCost; }
    EnemyType enemyType() const { return m_type; }
    qreal speed() const { return m_speed; }
    QPointF pos() const { return scenePos(); }

    void setMaxHp(int hp) { m_maxHp = hp; m_hp = hp; }
    bool hasSkill() const { return m_hasSkill; }
    void setHasSkill(bool has) { m_hasSkill = has; }

private:
    EnemyType m_type;
    int m_maxHp;
    int m_hp;
    int m_reward;
    int m_livesCost = 1;
    qreal m_speed; // pixels per second
    QVector<QPointF> m_waypoints;
    int m_currentWaypoint = 0;
    bool m_reachedEnd = false;
    QColor m_color;
    qreal m_radius;
    qreal m_slowFactor = 1.0;
    int m_slowTimer = 0;
    bool m_hasSkill = false;
};

#endif // ENEMY_H
