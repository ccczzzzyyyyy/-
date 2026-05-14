#ifndef TOWER_H
#define TOWER_H

#include <QGraphicsItem>
#include <QPainter>
#include <QPixmap>
#include <QHash>
#include <QPointF>
#include "enemy.h"

enum class TowerKind {
    Bottle,   // Basic, fast fire
    Fan,      // Splash area
    Star,     // Long range
    Bomb      // High damage, splash
};

struct TowerStats {
    int cost;
    qreal range;
    int damage;
    int fireIntervalMs; // ms between shots
    bool splash;
    qreal splashRadius;
    QColor color;
    QString name;
    bool splitsOnHit = false;
    int splitCount = 0;
    qreal slowFactor = 0;
    int slowDurationMs = 0;
};

class Tower : public QGraphicsItem
{
public:
    Tower(TowerKind kind, QGraphicsItem *parent = nullptr);
    ~Tower() override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    void updateFiring(int deltaMs, const QList<Enemy*> &enemies);
    bool canFire() const;
    Enemy* findTarget(const QList<Enemy*> &enemies) const;
    QPointF firePoint() const;

    TowerKind kind() const { return m_kind; }
    int level() const { return m_level; }
    int upgradeCost() const { return m_stats.cost * m_level; }
    int sellValue() const { return m_stats.cost * m_level / 2; }
    int damage() const { return m_stats.damage * m_level; }
    qreal range() const { return m_stats.range; }
    bool hasSplash() const { return m_stats.splash; }
    qreal splashRadius() const { return m_stats.splashRadius; }
    bool splitsOnHit() const { return m_stats.splitsOnHit; }
    int splitCount() const { return m_stats.splitCount; }
    qreal slowFactor() const { return m_stats.slowFactor; }
    int slowDurationMs() const { return m_stats.slowDurationMs; }
    const TowerStats& stats() const { return m_stats; }
    const QColor& color() const { return m_stats.color; }

    void upgrade();
    void resetFireTimer() { m_fireTimer = 0; }
    bool canUpgrade() const { return m_level < 3; }
    QPointF centerPos() const { return scenePos(); }

    bool isSelected() const { return m_selected; }
    void setSelected(bool sel);

private:
    TowerKind m_kind;
    int m_level = 1;
    int m_fireTimer = 0;
    TowerStats m_stats;
    qreal m_radius = 25;
    bool m_selected = false;

    static TowerStats statsForKind(TowerKind kind);
    static QPixmap imageForKind(TowerKind kind);
    static void loadImages();
    static QHash<TowerKind, QPixmap> s_images;
    static bool s_imagesLoaded;
};

#endif // TOWER_H
