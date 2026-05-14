#ifndef MAP_H
#define MAP_H

#include <QGraphicsItem>
#include <QVector>
#include <QPointF>

enum class TileType {
    Path,
    Grass,
    Unbuildable
};

class GameMap : public QGraphicsItem
{
public:
    static constexpr int TILE_SIZE = 80;
    static constexpr int COLS = 10;
    static constexpr int ROWS = 8;

    GameMap();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    TileType tileAt(int col, int row) const;
    TileType tileAtPos(QPointF pos) const;
    QPointF tileCenter(int col, int row) const;
    bool isBuildable(QPointF pos) const;
    bool isValidTile(int col, int row) const;

    const QVector<QPointF>& waypoints() const { return m_waypoints; }
    QPointF carrotPos() const { return m_carrotPos; }

private:
    TileType m_grid[ROWS][COLS];
    QVector<QPointF> m_waypoints;
    QPointF m_carrotPos;
    void initMap();
};

#endif // MAP_H
