#ifndef TOWERPANEL_H
#define TOWERPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include "tower.h"

class GameManager;

class TowerPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TowerPanel(QWidget *parent = nullptr);

    void setGameManager(GameManager *manager) { m_manager = manager; }

public slots:
    void updateMoney(int money);
    void updateWave(int wave, int total);
    void updateLives(int lives);
    void clearSelection();

signals:
    void towerSelected(TowerKind kind);

private slots:
    void onBottleClicked();
    void onFanClicked();
    void onStarClicked();
    void onBombClicked();
    void onStartWaveClicked();
    void onRestartClicked();
    void onSpeedClicked();
    void onPauseClicked();

private:
    GameManager *m_manager = nullptr;
    QLabel *m_moneyLabel;
    QLabel *m_waveLabel;
    QLabel *m_livesLabel;
    QPushButton *m_bottleBtn;
    QPushButton *m_fanBtn;
    QPushButton *m_starBtn;
    QPushButton *m_bombBtn;
    QPushButton *m_startBtn;
    QPushButton *m_restartBtn;
    QPushButton *m_speedBtn;
    QPushButton *m_pauseBtn;
    TowerKind m_selectedKind = TowerKind::Bottle;

    void updateButtonStyles();
};

#endif // TOWERPANEL_H
