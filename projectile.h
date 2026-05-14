#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <QGraphicsItem>
#include <QPainter>
#include <QPointF>
#include "enemy.h"

class Projectile : public QGraphicsItem
{
public:
    // Main projectile
    Projectile(QPointF start, Enemy *target, int damage, bool splash, qreal splashRadius,
               QColor color, bool splitsOnHit = false, int splitCount = 0,
               qreal slowFactor = 0, int slowDurationMs = 0,
               QGraphicsItem *parent = nullptr);
    // Sub-projectile (flies in a fixed direction)
    Projectile(QPointF start, QPointF direction, int damage, qreal maxDistance,
               QColor color, QGraphicsItem *parent = nullptr);

    ~Projectile() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    void updateMovement(int deltaMs);
    bool hasHit() const { return m_hasHit; }
    QPointF impactPoint() const { return m_impactPoint; }
    int damage() const { return m_damage; }
    bool splash() const { return m_splash; }
    qreal splashRadius() const { return m_splashRadius; }
    Enemy* targetEnemy() const { return m_target; }
    void onTargetDestroyed() { m_target = nullptr; m_hasHit = true; }

    bool splitsOnHit() const { return m_splitsOnHit; }
    int splitCount() const { return m_splitCount; }
    bool isSub() const { return m_isSub; }
    QColor color() const { return m_color; }
    qreal slowFactor() const { return m_slowFactor; }
    int slowDurationMs() const { return m_slowDurationMs; }

private:
    Enemy *m_target = nullptr;
    int m_damage;
    bool m_splash;
    qreal m_splashRadius;
    QColor m_color;
    bool m_hasHit = false;
    QPointF m_impactPoint;
    qreal m_speed = 300;

    bool m_splitsOnHit = false;
    int m_splitCount = 0;

    bool m_isSub = false;
    QPointF m_direction;
    qreal m_maxDistance = 0;
    qreal m_traveledDistance = 0;

    qreal m_slowFactor = 0;
    int m_slowDurationMs = 0;
};

#endif // PROJECTILE_H
