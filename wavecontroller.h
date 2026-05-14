#ifndef WAVECONTROLLER_H
#define WAVECONTROLLER_H

#include <QObject>
#include <QVector>
#include "enemy.h"

struct SpawnEntry {
    EnemyType type;
    int count;
    int intervalMs; // interval between each spawn
};

// A wave can contain multiple spawn entries (e.g., boss + minions)
struct Wave {
    QVector<SpawnEntry> entries;
};

class WaveController : public QObject
{
    Q_OBJECT

public:
    explicit WaveController(QObject *parent = nullptr);

    void setWaypoints(const QVector<QPointF> &wp) { m_waypoints = wp; }
    void update(int deltaMs);

    void startNextWave();
    bool isWaveActive() const { return m_waveActive; }
    int currentWave() const { return m_currentWave; }
    int totalWaves() const { return m_waves.size(); }
    bool allWavesDone() const { return m_allDone; }

signals:
    void enemySpawned(Enemy *enemy);
    void waveStarted(int waveNum);
    void allWavesComplete();

private:
    QVector<Wave> m_waves;
    QVector<QPointF> m_waypoints;
    int m_currentWave = 0;
    bool m_waveActive = false;
    bool m_allDone = false;

    // Spawning state
    int m_entryIndex = 0;    // which SpawnEntry within current wave
    int m_spawnCount = 0;    // how many spawned from current entry
    int m_spawnTimer = 0;

    void initWaves();
};

#endif // WAVECONTROLLER_H
