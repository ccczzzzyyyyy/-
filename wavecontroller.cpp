#include "wavecontroller.h"
#include <QtMath>

WaveController::WaveController(QObject *parent)
    : QObject(parent)
{
    initWaves();
}

void WaveController::initWaves()
{
    auto wave = [this](QVector<SpawnEntry> entries) {
        m_waves.append({entries});
    };

    // Wave 1-4: Easy, just normals
    wave({{EnemyType::Normal, 5, 1000}});
    wave({{EnemyType::Normal, 8, 900}});
    wave({{EnemyType::Normal, 10, 800}});
    wave({{EnemyType::Normal, 12, 750}});

    // Wave 5: Boss appears!
    wave({{EnemyType::Boss, 1, 3000},
          {EnemyType::Normal, 6, 800}});

    // Wave 6-9: Mixed enemies
    wave({{EnemyType::Fast, 10, 800}});
    wave({{EnemyType::Normal, 8, 700},
          {EnemyType::Fast, 5, 800}});
    wave({{EnemyType::Tank, 4, 1200}});
    wave({{EnemyType::Normal, 14, 600}});

    // Wave 10: Boss with skill + tanks
    wave({{EnemyType::Boss, 1, 3000},
          {EnemyType::Tank, 4, 1200}});

    // Wave 11-14: Escalating
    wave({{EnemyType::Fast, 15, 500}});
    wave({{EnemyType::Normal, 10, 500},
          {EnemyType::Tank, 3, 1000}});
    wave({{EnemyType::Normal, 12, 500}});
    wave({{EnemyType::Fast, 10, 400},
          {EnemyType::Tank, 4, 1000}});

    // Wave 15: Boss + heavy mixed
    wave({{EnemyType::Boss, 1, 3000},
          {EnemyType::Normal, 8, 600},
          {EnemyType::Fast, 6, 600}});

    // Wave 16-19: Late game
    wave({{EnemyType::Tank, 8, 900}});
    wave({{EnemyType::Fast, 12, 400}});
    wave({{EnemyType::Normal, 15, 400},
          {EnemyType::Tank, 4, 900}});
    wave({{EnemyType::Normal, 25, 350}});

    // Wave 20: Final boss with skill + heavy enemies
    wave({{EnemyType::Boss, 1, 3000},
          {EnemyType::Tank, 6, 800},
          {EnemyType::Fast, 10, 500}});
}

void WaveController::startNextWave()
{
    if (m_currentWave >= m_waves.size()) {
        m_allDone = true;
        emit allWavesComplete();
        return;
    }

    m_entryIndex = 0;
    m_spawnCount = 0;
    m_spawnTimer = 0;
    m_waveActive = true;

    emit waveStarted(m_currentWave + 1);
}

void WaveController::update(int deltaMs)
{
    if (!m_waveActive || m_allDone)
        return;

    const Wave &wave = m_waves[m_currentWave];

    // All entries in this wave done?
    if (m_entryIndex >= wave.entries.size()) {
        m_waveActive = false;
        m_currentWave++;
        return;
    }

    const SpawnEntry &entry = wave.entries[m_entryIndex];

    m_spawnTimer += deltaMs;

    while (m_spawnTimer >= entry.intervalMs && m_spawnCount < entry.count) {
        m_spawnTimer -= entry.intervalMs;
        m_spawnCount++;

        Enemy *enemy = new Enemy(entry.type, m_waypoints);

        // Scale boss HP by wave number
        if (entry.type == EnemyType::Boss) {
            int waveNum = m_currentWave + 1; // 1-indexed
            int bossHp = (2000 + waveNum * 600) * 0.7;
            enemy->setMaxHp(bossHp);

            // Every 10th wave boss has special skill
            if (waveNum % 10 == 0) {
                enemy->setHasSkill(true);
            }
        }

        emit enemySpawned(enemy);
    }

    // Current entry done? Move to next entry
    if (m_spawnCount >= entry.count) {
        m_entryIndex++;
        m_spawnCount = 0;
        m_spawnTimer = 0;
    }
}
