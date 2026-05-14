#include "gamescene.h"
#include "towereffect.h"
#include <QtMath>
#include <QDateTime>
#include <QRandomGenerator>
#include <QGraphicsSceneMouseEvent>

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent)
{
    // Create map
    m_map = new GameMap();
    addItem(m_map);

    setSceneRect(m_map->boundingRect());

    // Create wave controller
    m_waveController = new WaveController(this);
    m_waveController->setWaypoints(m_map->waypoints());

    connect(m_waveController, &WaveController::enemySpawned, this, [this](Enemy *enemy) {
        m_enemies.append(enemy);
        addItem(enemy);
    });
    connect(m_waveController, &WaveController::waveStarted, this, [this](int waveNum) {
        emit waveStarted(waveNum);
    });
    connect(m_waveController, &WaveController::allWavesComplete, this, [this]() {
        m_allWavesSpawned = true;
    });

    // Game timer at ~60 FPS
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GameScene::gameTick);
    m_timer->setInterval(16);
    m_timer->start();
}

GameScene::~GameScene()
{
}

void GameScene::startNextWave()
{
    if (!m_gameOver) {
        clearEnemiesAndProjectiles();
        m_waveController->startNextWave();
    }
}

void GameScene::restartGame()
{
    clearAll();

    delete m_waveController;
    m_waveController = new WaveController(this);
    m_waveController->setWaypoints(m_map->waypoints());

    connect(m_waveController, &WaveController::enemySpawned, this, [this](Enemy *enemy) {
        m_enemies.append(enemy);
        addItem(enemy);
    });
    connect(m_waveController, &WaveController::waveStarted, this, [this](int waveNum) {
        emit waveStarted(waveNum);
    });
    connect(m_waveController, &WaveController::allWavesComplete, this, [this]() {
        m_allWavesSpawned = true;
    });

    m_gameOver = false;
    m_allWavesSpawned = false;
    m_paused = false;
    m_speedMul = 1.0;
    m_bossSkillTimer = 0;
    m_timer->start();
}

void GameScene::stopGame()
{
    m_gameOver = true;
    m_timer->stop();
}

void GameScene::setPaused(bool paused)
{
    m_paused = paused;
}

void GameScene::gameTick()
{
    if (m_gameOver)
        return;

    // Use fixed delta approach for smooth updates
    static qint64 lastTime = 0;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (lastTime == 0) lastTime = now;
    int rawDelta = static_cast<int>(qMin(now - lastTime, qint64(50)));
    lastTime = now;

    if (m_paused)
        return;

    int deltaMs = static_cast<int>(rawDelta * m_speedMul);

    m_waveController->update(deltaMs);
    updateEnemies(deltaMs);
    updateTowers(deltaMs);
    updateProjectiles(deltaMs);
    checkCollisions();
    removeDeadEnemies();
    removeHitProjectiles();

    // Boss skill: every 5s, destroy a random tower
    bool hasSkillBoss = false;
    for (auto *enemy : m_enemies) {
        if (!enemy->isDead() && enemy->hasSkill()) {
            hasSkillBoss = true;
            break;
        }
    }
    if (hasSkillBoss) {
        m_bossSkillTimer += deltaMs;
        if (m_bossSkillTimer >= 7000 && !m_towers.isEmpty()) {
            m_bossSkillTimer = 0;
            int idx = QRandomGenerator::global()->bounded(m_towers.size());
            Tower *tower = m_towers[idx];
            QPointF towerPos = tower->centerPos();
            if (m_selectedTower == tower)
                m_selectedTower = nullptr;
            removeItem(tower);
            m_towers.removeAt(idx);
            delete tower;
            TowerDestroyEffect::play(towerPos, this);
        }
    } else {
        m_bossSkillTimer = 0;
    }
}

void GameScene::updateEnemies(int deltaMs)
{
    for (auto *enemy : m_enemies) {
        if (!enemy->isDead() && !enemy->hasReachedEnd()) {
            enemy->updateMovement(deltaMs);
        }
    }
}

void GameScene::updateTowers(int deltaMs)
{
    for (auto *tower : m_towers) {
        tower->updateFiring(deltaMs, m_enemies);

        if (tower->canFire()) {
            Enemy *target = tower->findTarget(m_enemies);
            if (target) {
                Projectile *proj = new Projectile(
                    tower->firePoint(), target,
                    tower->damage(),
                    tower->hasSplash(), tower->splashRadius(),
                    tower->color(),
                    tower->splitsOnHit(), tower->splitCount(),
                    tower->slowFactor(), tower->slowDurationMs());
                m_projectiles.append(proj);
                addItem(proj);
                tower->resetFireTimer();
            }
        }
    }
}

void GameScene::updateProjectiles(int deltaMs)
{
    for (auto *proj : m_projectiles) {
        proj->updateMovement(deltaMs);
    }
}

void GameScene::checkCollisions()
{
    for (auto *proj : m_projectiles) {
        if (!proj->hasHit())
            continue;

        int dmg = proj->damage();
        QPointF impact = proj->impactPoint();

        if (proj->isSub()) {
            applySubDamage(impact, dmg);
        } else if (proj->splitsOnHit()) {
            Enemy *target = proj->targetEnemy();
            if (target && !target->isDead())
                target->takeDamage(dmg);
            int subDmg = qMax(1, dmg / 5);
            int count = proj->splitCount();
            for (int i = 0; i < count; ++i) {
                qreal angle = 2.0 * 3.1415926535 * i / count;
                QPointF dir(qCos(angle), qSin(angle));
                Projectile *sub = new Projectile(impact, dir, subDmg, 50, proj->color());
                m_projectiles.append(sub);
                addItem(sub);
            }
        } else if (proj->splash()) {
            applySplashDamage(impact, dmg, proj->splashRadius(),
                              proj->slowFactor(), proj->slowDurationMs());
        } else {
            Enemy *target = proj->targetEnemy();
            if (target && !target->isDead())
                target->takeDamage(dmg);
        }
    }
}

void GameScene::applySubDamage(QPointF center, int damage)
{
    Enemy *best = nullptr;
    qreal bestDist = 30;
    for (auto *enemy : m_enemies) {
        if (enemy->isDead()) continue;
        QPointF diff = enemy->pos() - center;
        qreal dist = qSqrt(diff.x() * diff.x() + diff.y() * diff.y());
        if (dist < bestDist) {
            best = enemy;
            bestDist = dist;
        }
    }
    if (best)
        best->takeDamage(damage);
}

void GameScene::applySplashDamage(QPointF center, int damage, qreal radius,
                                   qreal slowFactor, int slowDurationMs)
{
    for (auto *enemy : m_enemies) {
        if (enemy->isDead()) continue;
        QPointF diff = enemy->pos() - center;
        qreal dist = qSqrt(diff.x() * diff.x() + diff.y() * diff.y());
        if (dist <= radius) {
            int actualDmg = static_cast<int>(damage * (1.0 - dist / radius * 0.5));
            enemy->takeDamage(qMax(1, actualDmg));
            if (slowFactor > 0)
                enemy->applySlow(slowFactor, slowDurationMs);
        }
    }
}

void GameScene::removeDeadEnemies()
{
    for (int i = m_enemies.size() - 1; i >= 0; --i) {
        Enemy *enemy = m_enemies[i];
        bool willRemove = enemy->isDead() || enemy->hasReachedEnd();
        if (willRemove) {
            // Null out any projectile targeting this enemy to avoid dangling pointer
            for (auto *proj : m_projectiles) {
                if (proj->targetEnemy() == enemy)
                    proj->onTargetDestroyed();
            }
        }
        if (enemy->isDead()) {
            emit enemyKilled(enemy->reward());
            removeItem(enemy);
            m_enemies.removeAt(i);
            delete enemy;
        } else if (enemy->hasReachedEnd()) {
            emit enemyReachedEnd(enemy->livesCost());
            removeItem(enemy);
            m_enemies.removeAt(i);
            delete enemy;
        }
    }

    // Check win: all waves spawned and no enemies left
    if (m_allWavesSpawned && m_enemies.isEmpty() && !m_gameOver) {
        m_gameOver = true;
        m_timer->stop();
        emit gameWon();
    }
}

void GameScene::removeHitProjectiles()
{
    for (int i = m_projectiles.size() - 1; i >= 0; --i) {
        if (m_projectiles[i]->hasHit()) {
            removeItem(m_projectiles[i]);
            delete m_projectiles[i];
            m_projectiles.removeAt(i);
        }
    }
}

Tower* GameScene::towerAt(QPointF pos) const
{
    for (auto *tower : m_towers) {
        QPointF diff = pos - tower->centerPos();
        if (qSqrt(diff.x() * diff.x() + diff.y() * diff.y()) < 30)
            return tower;
    }
    return nullptr;
}

bool GameScene::canPlaceTowerAt(QPointF pos) const
{
    if (!m_map->isBuildable(pos))
        return false;
    if (towerAt(pos) != nullptr)
        return false;
    return true;
}

bool GameScene::placeTower(TowerKind kind, QPointF pos)
{
    if (!canPlaceTowerAt(pos))
        return false;

    Tower *tower = new Tower(kind);
    tower->setPos(pos);
    m_towers.append(tower);
    addItem(tower);
    return true;
}

void GameScene::upgradeTower(Tower *tower)
{
    tower->upgrade();
}

void GameScene::sellTower(Tower *tower)
{
    if (m_selectedTower == tower)
        m_selectedTower = nullptr;
    m_towers.removeOne(tower);
    removeItem(tower);
    delete tower;
}

void GameScene::selectTower(Tower *tower)
{
    if (m_selectedTower == tower)
        return;
    deselectTower();
    m_selectedTower = tower;
    tower->setSelected(true);
}

void GameScene::deselectTower()
{
    if (m_selectedTower) {
        m_selectedTower->setSelected(false);
        m_selectedTower = nullptr;
    }
}

void GameScene::clearEnemiesAndProjectiles()
{
    for (auto *enemy : m_enemies) {
        removeItem(enemy);
        delete enemy;
    }
    m_enemies.clear();

    for (auto *proj : m_projectiles) {
        removeItem(proj);
        delete proj;
    }
    m_projectiles.clear();
}

void GameScene::clearAll()
{
    for (auto *enemy : m_enemies) {
        removeItem(enemy);
        delete enemy;
    }
    m_enemies.clear();

    for (auto *proj : m_projectiles) {
        removeItem(proj);
        delete proj;
    }
    m_projectiles.clear();

    for (auto *tower : m_towers) {
        removeItem(tower);
        delete tower;
    }
    m_towers.clear();
    m_selectedTower = nullptr;
}
