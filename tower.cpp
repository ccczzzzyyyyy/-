#include "tower.h"
#include <QtMath>

QHash<TowerKind, QPixmap> Tower::s_images;
bool Tower::s_imagesLoaded = false;

void Tower::loadImages()
{
    if (s_imagesLoaded) return;
    s_images[TowerKind::Bottle] = QPixmap(":/images/bottle.png");
    s_images[TowerKind::Fan]    = QPixmap(":/images/fan.png");
    s_images[TowerKind::Star]   = QPixmap(":/images/star.png");
    s_images[TowerKind::Bomb]   = QPixmap(":/images/bomb.png");
    s_imagesLoaded = true;
}

QPixmap Tower::imageForKind(TowerKind kind)
{
    loadImages();
    return s_images.value(kind);
}

TowerStats Tower::statsForKind(TowerKind kind)
{
    switch (kind) {
    case TowerKind::Bottle:
        return { 100, 120, 15, 500, false, 0, QColor(100, 200, 255), "瓶子塔" };
    case TowerKind::Fan:
        return { 150, 100, 15, 600, true, 60, QColor(150, 255, 150), "风扇塔", false, 0, 0.7, 2000 };
    case TowerKind::Star:
        return { 200, 180, 25, 900, false, 0, QColor(255, 255, 100), "星星塔", true, 5 };
    case TowerKind::Bomb:
        return { 300, 90, 20, 1500, true, 80, QColor(255, 120, 50), "炸弹塔" };
    }
    return {};
}

Tower::Tower(TowerKind kind, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_kind(kind), m_stats(statsForKind(kind))
{
    loadImages();
}

Tower::~Tower() {}

QRectF Tower::boundingRect() const
{
    qreal r = qMax(m_radius, m_stats.range);
    return QRectF(-r, -r, r * 2, r * 2);
}

QPainterPath Tower::shape() const
{
    QPainterPath path;
    path.addEllipse(QPointF(0, 0), m_radius, m_radius);
    return path;
}

void Tower::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QPixmap img = imageForKind(m_kind);
    qreal size = m_radius * 2;

    if (!img.isNull()) {
        painter->drawPixmap(QRectF(-size / 2, -size / 2, size, size), img, img.rect());
    } else {
        // Fallback: procedural circle
        QRadialGradient grad(0, 0, m_radius);
        grad.setColorAt(0, m_stats.color.lighter(150));
        grad.setColorAt(1, m_stats.color.darker(150));
        painter->setBrush(grad);
        painter->setPen(QPen(Qt::black, 2));
        painter->drawEllipse(QPointF(0, 0), m_radius, m_radius);
    }

    // Level indicator
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    for (int i = 0; i < m_level; ++i) {
        painter->drawEllipse(QPointF(-8 + i * 8, 0), 3, 3);
    }

    // Range indicator — only when selected
    if (m_selected) {
        painter->setPen(QPen(m_stats.color, 1.5, Qt::DashLine));
        painter->setBrush(QColor(m_stats.color.red(), m_stats.color.green(),
                                  m_stats.color.blue(), 30));
        painter->drawEllipse(QPointF(0, 0), m_stats.range, m_stats.range);
    }
}

void Tower::updateFiring(int deltaMs, const QList<Enemy*> &enemies)
{
    m_fireTimer += deltaMs;
}

bool Tower::canFire() const
{
    return m_fireTimer >= m_stats.fireIntervalMs;
}

Enemy* Tower::findTarget(const QList<Enemy*> &enemies) const
{
    Enemy *best = nullptr;
    qreal bestDist = 1e9;

    for (auto *enemy : enemies) {
        if (enemy->isDead() || enemy->hasReachedEnd())
            continue;
        QPointF diff = enemy->pos() - scenePos();
        qreal dist = qSqrt(diff.x() * diff.x() + diff.y() * diff.y());
        if (dist <= m_stats.range + 20) {
            if (dist < bestDist) {
                best = enemy;
                bestDist = dist;
            }
        }
    }
    return best;
}

QPointF Tower::firePoint() const
{
    return scenePos();
}

void Tower::upgrade()
{
    if (m_level < 3)
        m_level++;
}

void Tower::setSelected(bool sel)
{
    if (m_selected != sel) {
        m_selected = sel;
        update();
    }
}
