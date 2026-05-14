#include "gamemanager.h"
#include <QMessageBox>

GameManager::GameManager(GameScene *scene, TowerPanel *panel, QObject *parent)
    : QObject(parent), m_scene(scene), m_panel(panel)
{
    connect(m_scene, &GameScene::enemyKilled, this, &GameManager::onEnemyKilled);
    connect(m_scene, &GameScene::enemyReachedEnd, this, &GameManager::onEnemyReachedEnd);
    connect(m_scene, &GameScene::waveStarted, this, &GameManager::onWaveStarted);
    connect(m_scene, &GameScene::gameWon, this, &GameManager::onGameWon);
}

void GameManager::spendMoney(int amount)
{
    m_money -= amount;
    emit moneyChanged(m_money);
}

void GameManager::earnMoney(int amount)
{
    m_money += amount;
    emit moneyChanged(m_money);
}

void GameManager::loseLife(int amount)
{
    m_lives -= amount;
    if (m_lives < 0) m_lives = 0;
    emit livesChanged(m_lives);
    checkGameOver();
}

void GameManager::startNextWave()
{
    m_scene->startNextWave();
}

void GameManager::restartGame()
{
    m_money = 300;
    m_lives = 10;
    emit moneyChanged(m_money);
    emit livesChanged(m_lives);
    m_scene->restartGame();
}

void GameManager::onEnemyKilled(int reward)
{
    earnMoney(reward);
}

void GameManager::onEnemyReachedEnd(int livesCost)
{
    loseLife(livesCost);
}

void GameManager::onWaveStarted(int waveNum)
{
    emit waveChanged(waveNum, m_scene->totalWaves());
}

void GameManager::onGameWon()
{
    emit gameWon();
}

void GameManager::toggleSpeed()
{
    qreal cur = m_scene->speedMultiplier();
    m_scene->setSpeedMultiplier(cur >= 1.9 ? 1.0 : 2.0);
}

void GameManager::togglePause()
{
    m_scene->setPaused(!m_scene->isPaused());
}

bool GameManager::isPaused() const
{
    return m_scene->isPaused();
}

qreal GameManager::speedMultiplier() const
{
    return m_scene->speedMultiplier();
}

void GameManager::checkGameOver()
{
    if (m_lives <= 0) {
        m_lives = 0;
        emit gameLost();
        m_scene->stopGame();
    }
}
