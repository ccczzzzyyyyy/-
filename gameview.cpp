#include "gameview.h"
#include "gamescene.h"
#include "gamemanager.h"
#include "map.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

GameView::GameView(GameScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), m_scene(scene)
{
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(GameMap::COLS * GameMap::TILE_SIZE,
                 GameMap::ROWS * GameMap::TILE_SIZE);
    setMouseTracking(true);
}

void GameView::setPlacementMode(TowerKind kind)
{
    m_placingTower = true;
    m_placingKind = kind;
    setCursor(Qt::CrossCursor);
    m_scene->deselectTower();
}

void GameView::clearPlacementMode()
{
    if (m_placingTower) {
        m_placingTower = false;
        setCursor(Qt::ArrowCursor);
        emit placementCancelled();
    }
}

void GameView::mousePressEvent(QMouseEvent *event)
{
    // Right click: open upgrade if on existing tower, otherwise cancel placement
    if (event->button() == Qt::RightButton) {
        if (m_placingTower) {
            QPointF scenePos = mapToScene(event->pos());
            Tower *existing = m_scene->towerAt(scenePos);
            if (existing) {
                clearPlacementMode();
                m_scene->selectTower(existing);
                emit towerClicked(existing);
                return;
            }
        }
        clearPlacementMode();
        return;
    }

    QPointF scenePos = mapToScene(event->pos());

    if (m_placingTower && m_manager) {
        // Placement mode: try to place tower
        int cost = 0;
        switch (m_placingKind) {
        case TowerKind::Bottle: cost = 100; break;
        case TowerKind::Fan:    cost = 150; break;
        case TowerKind::Star:   cost = 200; break;
        case TowerKind::Bomb:   cost = 300; break;
        }

        if (!m_manager->canAfford(cost)) {
            QMessageBox::information(this, "提示", "金币不足！");
            return;
        }

        // Snap to tile center
        int col = static_cast<int>(scenePos.x()) / GameMap::TILE_SIZE;
        int row = static_cast<int>(scenePos.y()) / GameMap::TILE_SIZE;
        QPointF snapPos = m_scene->gameMap()->tileCenter(col, row);

        if (m_scene->canPlaceTowerAt(snapPos)) {
            m_scene->placeTower(m_placingKind, snapPos);
            m_manager->spendMoney(cost);
            // Stay in placement mode for next tower
        } else {
            QMessageBox::information(this, "提示", "此处不能放置防御塔！");
        }
        return;
    }

    // Not in placement mode: check if clicking on existing tower
    Tower *existing = m_scene->towerAt(scenePos);
    if (existing) {
        m_scene->selectTower(existing);
        emit towerClicked(existing);
        return;
    }

    // Clicked empty space — deselect
    m_scene->deselectTower();
    QGraphicsView::mousePressEvent(event);
}

void GameView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        clearPlacementMode();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}
