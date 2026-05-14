#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "gamescene.h"
#include "towerpanel.h"

class GameScene;
class TowerPanel;

class GameManager : public QObject
{
    Q_OBJECT

public:
    explicit GameManager(GameScene *scene, TowerPanel *panel, QObject *parent = nullptr);

    GameScene* scene() const { return m_scene; }

    int money() const { return m_money; }
    int lives() const { return m_lives; }
    int currentWave() const { return m_scene->currentWave(); }
    int totalWaves() const { return m_scene->totalWaves(); }

    bool canAfford(int cost) const { return m_money >= cost; }
    void spendMoney(int amount);
    void earnMoney(int amount);
    void loseLife(int amount = 1);

    void startNextWave();
    void restartGame();

    void toggleSpeed();
    void togglePause();
    bool isPaused() const;
    qreal speedMultiplier() const;

signals:
    void moneyChanged(int money);
    void livesChanged(int lives);
    void waveChanged(int wave, int total);
    void gameWon();
    void gameLost();

private slots:
    void onEnemyKilled(int reward);
    void onEnemyReachedEnd(int livesCost);
    void onWaveStarted(int waveNum);
    void onGameWon();

private:
    GameScene *m_scene;
    TowerPanel *m_panel;
    int m_money = 300;
    int m_lives = 10;

    void checkGameOver();
};

#endif // GAMEMANAGER_H
