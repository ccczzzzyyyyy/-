#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamescene.h"
#include "gameview.h"
#include "towerpanel.h"
#include "gamemanager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTowerSelected(TowerKind kind);
    void onTowerClicked(Tower *tower);
    void onGameWon();
    void onGameLost();

private:
    GameScene *m_scene;
    GameView *m_view;
    TowerPanel *m_panel;
    GameManager *m_manager;
};

#endif // MAINWINDOW_H
