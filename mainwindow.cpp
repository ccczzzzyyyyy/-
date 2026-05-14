#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Left: tower panel
    m_panel = new TowerPanel(this);

    // Right: game view
    m_scene = new GameScene(this);
    m_view = new GameView(m_scene, this);

    m_manager = new GameManager(m_scene, m_panel, this);

    m_panel->setGameManager(m_manager);
    m_view->setGameManager(m_manager);

    mainLayout->addWidget(m_panel);
    mainLayout->addWidget(m_view, 1);

    // HUD updates
    connect(m_manager, &GameManager::moneyChanged, m_panel, &TowerPanel::updateMoney);
    connect(m_manager, &GameManager::waveChanged, m_panel, &TowerPanel::updateWave);
    connect(m_manager, &GameManager::livesChanged, m_panel, &TowerPanel::updateLives);

    // Tower placement
    connect(m_panel, &TowerPanel::towerSelected, this, &MainWindow::onTowerSelected);

    // Tower click (upgrade/sell)
    connect(m_view, &GameView::towerClicked, this, &MainWindow::onTowerClicked);
    // Placement cancelled — reset panel button highlight
    connect(m_view, &GameView::placementCancelled, m_panel, &TowerPanel::clearSelection);

    // Game over / win
    connect(m_manager, &GameManager::gameWon, this, &MainWindow::onGameWon);
    connect(m_manager, &GameManager::gameLost, this, &MainWindow::onGameLost);

    // Initial display
    m_panel->updateMoney(300);
    m_panel->updateWave(0, 20);
    m_panel->updateLives(10);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onTowerSelected(TowerKind kind)
{
    m_view->setPlacementMode(kind);
}

void MainWindow::onTowerClicked(Tower *tower)
{
    int upgradeCost = tower->upgradeCost();
    bool canUpgrade = tower->canUpgrade();
    int sellValue = tower->sellValue();

    QString msg = QString("塔类型: %1\n等级: %2\n伤害: %3\n\n")
                      .arg(tower->stats().name)
                      .arg(tower->level())
                      .arg(tower->damage());
    if (canUpgrade)
        msg += QString("升级费用: %1\n").arg(upgradeCost);
    msg += QString("出售价格: %1").arg(sellValue);

    QMessageBox msgBox;
    msgBox.setWindowTitle("防御塔");
    msgBox.setText(msg);

    QPushButton *upgradeBtn = nullptr;
    if (canUpgrade) {
        upgradeBtn = msgBox.addButton("升级", QMessageBox::ActionRole);
    }
    QPushButton *sellBtn = msgBox.addButton("出售", QMessageBox::ActionRole);
    msgBox.addButton("取消", QMessageBox::RejectRole);

    msgBox.exec();

    if (msgBox.clickedButton() == upgradeBtn && canUpgrade) {
        if (m_manager->canAfford(upgradeCost)) {
            m_manager->spendMoney(upgradeCost);
            m_scene->upgradeTower(tower);
        } else {
            QMessageBox::information(this, "提示", "金币不足，无法升级！");
        }
    } else if (msgBox.clickedButton() == sellBtn) {
        m_manager->earnMoney(sellValue);
        m_scene->sellTower(tower);
    }
}

void MainWindow::onGameWon()
{
    QMessageBox::information(this, "胜利！",
                             "🎉 恭喜！你成功守护了萝卜！\n\n所有敌人都被消灭了！");
}

void MainWindow::onGameLost()
{
    QMessageBox::information(this, "失败...",
                             "💔 萝卜被吃掉了...\n\n再来一次吧！");
}
