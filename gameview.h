#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include "tower.h"

class GameScene;
class GameManager;

class GameView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GameView(GameScene *scene, QWidget *parent = nullptr);
    void setGameManager(GameManager *m) { m_manager = m; }

public slots:
    void setPlacementMode(TowerKind kind);
    void clearPlacementMode();

signals:
    void towerClicked(Tower *tower);
    void placementCancelled();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    GameScene *m_scene;
    GameManager *m_manager = nullptr;
    bool m_placingTower = false;
    TowerKind m_placingKind;
};

#endif // GAMEVIEW_H
