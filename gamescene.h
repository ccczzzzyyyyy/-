#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include "map.h"
#include "tower.h"
#include "enemy.h"
#include "projectile.h"
#include "wavecontroller.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GameScene(QObject *parent = nullptr);
    ~GameScene();

    void startNextWave();
    void restartGame();
    void stopGame();
    bool isGameOver() const { return m_gameOver; }

    void setSpeedMultiplier(qreal mul) { m_speedMul = mul; }
    qreal speedMultiplier() const { return m_speedMul; }
    void setPaused(bool paused);
    bool isPaused() const { return m_paused; }

    int currentWave() const { return m_waveController->currentWave(); }
    int totalWaves() const { return m_waveController->totalWaves(); }

    Tower* towerAt(QPointF pos) const;
    bool placeTower(TowerKind kind, QPointF pos);
    void upgradeTower(Tower *tower);
    void sellTower(Tower *tower);
    void selectTower(Tower *tower);
    void deselectTower();

    GameMap* gameMap() const { return m_map; }
    QList<Enemy*> activeEnemies() const { return m_enemies; }
    bool canPlaceTowerAt(QPointF pos) const;

signals:
    void enemyKilled(int reward);
    void enemyReachedEnd(int livesCost);
    void waveStarted(int waveNum);
    void gameWon();

private slots:
    void gameTick();

private:
    GameMap *m_map;
    QTimer *m_timer;
    WaveController *m_waveController;
    QList<Enemy*> m_enemies;
    QList<Tower*> m_towers;
    QList<Projectile*> m_projectiles;
    Tower *m_selectedTower = nullptr;
    bool m_gameOver = false;
    bool m_allWavesSpawned = false;
    bool m_paused = false;
    qreal m_speedMul = 1.0;
    int m_bossSkillTimer = 0;

    void updateEnemies(int deltaMs);
    void updateTowers(int deltaMs);
    void updateProjectiles(int deltaMs);
    void checkCollisions();
    void applySplashDamage(QPointF center, int damage, qreal radius,
                           qreal slowFactor, int slowDurationMs);
    void applySubDamage(QPointF center, int damage);
    void removeDeadEnemies();
    void removeHitProjectiles();
    void clearAll();
    void clearEnemiesAndProjectiles();
};

#endif // GAMESCENE_H
