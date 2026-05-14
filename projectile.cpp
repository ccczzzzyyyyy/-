#include "projectile.h"
#include <QtMath>
#include <QtGlobal>

Projectile::Projectile(QPointF start, Enemy *target, int damage, bool splash,
                       qreal splashRadius, QColor color, bool splitsOnHit, int splitCount,
                       qreal slowFactor, int slowDurationMs, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_target(target), m_damage(damage),
      m_splash(splash), m_splashRadius(splashRadius), m_color(color),
      m_splitsOnHit(splitsOnHit), m_splitCount(splitCount),
      m_slowFactor(slowFactor), m_slowDurationMs(slowDurationMs)
{
    setPos(start);
}

Projectile::Projectile(QPointF start, QPointF direction, int damage, qreal maxDistance,
                       QColor color, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_damage(damage), m_color(color),
      m_isSub(true), m_direction(direction), m_maxDistance(maxDistance)
{
    setPos(start);
    // Normalize direction
    qreal len = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (len > 0)
        m_direction /= len;
}

Projectile::~Projectile() {}

QRectF Projectile::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
}

void Projectile::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(m_color);
    painter->setPen(Qt::NoPen);
    if (m_isSub)
        painter->drawEllipse(QPointF(0, 0), 2.5, 2.5);
    else
        painter->drawEllipse(QPointF(0, 0), 4, 4);
}

void Projectile::updateMovement(int deltaMs)
{
    if (m_hasHit)
        return;

    qreal moveDistance = m_speed * deltaMs / 1000.0;

    if (m_isSub) {
        m_traveledDistance += moveDistance;
        if (m_traveledDistance >= m_maxDistance) {
            m_hasHit = true;
            m_impactPoint = pos();
        } else {
            prepareGeometryChange();
            setPos(pos() + m_direction * moveDistance);
        }
        return;
    }

    if (!m_target || m_target->isDead()) {
        m_hasHit = true;
        m_impactPoint = pos();
        return;
    }

    QPointF targetPos = m_target->pos();
    QPointF current = pos();
    QPointF dir = targetPos - current;
    qreal dist = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());

    if (dist <= moveDistance + 5) {
        m_hasHit = true;
        m_impactPoint = targetPos;
    } else {
        qreal ratio = moveDistance / dist;
        prepareGeometryChange();
        setPos(current + dir * ratio);
    }
}
