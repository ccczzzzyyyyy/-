#include "map.h"
#include <QPainter>
#include <QtMath>

GameMap::GameMap()
{
    setPos(0, 0);
    initMap();
}

void GameMap::initMap()
{
    // Initialize all tiles to grass
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            m_grid[r][c] = TileType::Grass;

    // Define a winding path: col, row
    // Path goes: start(0,2) -> (1,2) -> (2,2) -> (3,2) -> (4,2) -> (4,3) -> (4,4)
    //            -> (4,5) -> (3,5) -> (2,5) -> (1,5) -> (1,4) -> (1,3) -> (2,3)
    //            -> (3,3) -> (3,4) -> (2,4) -> (2,5) ... let me make a simpler path
    // Simple S-shaped path:
    struct { int c, r; } pathTiles[] = {
        {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2},
        {5, 3}, {5, 4},
        {4, 4}, {3, 4}, {2, 4}, {1, 4}, {0, 4},
        {0, 5},
        {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5},
        {5, 6},
        {6, 6}, {7, 6}, {8, 6}, {9, 6}
    };

    int count = sizeof(pathTiles) / sizeof(pathTiles[0]);
    for (int i = 0; i < count; ++i) {
        int c = pathTiles[i].c;
        int r = pathTiles[i].r;
        m_grid[r][c] = TileType::Path;
    }

    // Mark some edge tiles as unbuildable (decorative rocks/water)
    m_grid[0][0] = TileType::Unbuildable;
    m_grid[0][1] = TileType::Unbuildable;
    m_grid[0][6] = TileType::Unbuildable;
    m_grid[0][7] = TileType::Unbuildable;
    m_grid[7][0] = TileType::Unbuildable;
    m_grid[7][9] = TileType::Unbuildable;

    // Generate waypoints from path tiles (center of each path tile)
    for (int i = 0; i < count; ++i) {
        int c = pathTiles[i].c;
        int r = pathTiles[i].r;
        m_waypoints.append(tileCenter(c, r));
    }

    // Carrot is at the last waypoint
    m_carrotPos = tileCenter(pathTiles[count - 1].c, pathTiles[count - 1].r);
}

QRectF GameMap::boundingRect() const
{
    return QRectF(0, 0, COLS * TILE_SIZE, ROWS * TILE_SIZE);
}

void GameMap::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            QRectF rect(c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            switch (m_grid[r][c]) {
            case TileType::Path:
                painter->fillRect(rect, QColor(210, 180, 140)); // tan path
                break;
            case TileType::Grass:
                painter->fillRect(rect, QColor(163, 254, 137)); // green grass
                break;
            case TileType::Unbuildable:
                painter->fillRect(rect, QColor(100, 100, 100)); // grey rock
                break;
            }
            painter->setPen(QColor(0, 0, 0, 40));
            painter->drawRect(rect);
        }
    }

    // Draw carrot at the end
    QPointF cp = carrotPos();
    painter->setBrush(QColor(255, 80, 30));
    painter->setPen(Qt::black);
    painter->drawEllipse(cp, 20, 20);
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 14, QFont::Bold));
    painter->drawText(QRectF(cp.x() - 20, cp.y() - 20, 40, 40),
                      Qt::AlignCenter, "🥕");
}

TileType GameMap::tileAt(int col, int row) const
{
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS)
        return TileType::Unbuildable;
    return m_grid[row][col];
}

TileType GameMap::tileAtPos(QPointF pos) const
{
    int col = static_cast<int>(pos.x()) / TILE_SIZE;
    int row = static_cast<int>(pos.y()) / TILE_SIZE;
    return tileAt(col, row);
}

QPointF GameMap::tileCenter(int col, int row) const
{
    return QPointF(col * TILE_SIZE + TILE_SIZE / 2.0,
                   row * TILE_SIZE + TILE_SIZE / 2.0);
}

bool GameMap::isBuildable(QPointF pos) const
{
    return tileAtPos(pos) == TileType::Grass;
}

bool GameMap::isValidTile(int col, int row) const
{
    return col >= 0 && col < COLS && row >= 0 && row < ROWS;
}
