#include "towerpanel.h"
#include "gamemanager.h"
#include <QGroupBox>
#include <QFont>
#include <QMessageBox>

TowerPanel::TowerPanel(QWidget *parent)
    : QWidget(parent)
{
    setFixedWidth(200);
    setStyleSheet("background-color: #2c3e50; color: white;");

    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(8);
    layout->setContentsMargins(10, 10, 10, 10);

    // Title
    auto *title = new QLabel("🏰 保卫萝卜");
    title->setFont(QFont("Microsoft YaHei", 16, QFont::Bold));
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #f39c12; padding: 5px;");
    layout->addWidget(title);

    // Separator
    auto *sep1 = new QFrame();
    sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet("color: #7f8c8d;");
    layout->addWidget(sep1);

    // Money display
    m_moneyLabel = new QLabel("💰 金币: 300");
    m_moneyLabel->setFont(QFont("Microsoft YaHei", 12));
    m_moneyLabel->setStyleSheet("color: #f1c40f; padding: 3px;");
    layout->addWidget(m_moneyLabel);

    // Wave display
    m_waveLabel = new QLabel("🌊 波次: 0/20");
    m_waveLabel->setFont(QFont("Microsoft YaHei", 12));
    m_waveLabel->setStyleSheet("color: #3498db; padding: 3px;");
    layout->addWidget(m_waveLabel);

    // Lives display
    m_livesLabel = new QLabel("❤️ 生命: 10");
    m_livesLabel->setFont(QFont("Microsoft YaHei", 12));
    m_livesLabel->setStyleSheet("color: #e74c3c; padding: 3px;");
    layout->addWidget(m_livesLabel);

    // Separator
    auto *sep2 = new QFrame();
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("color: #7f8c8d;");
    layout->addWidget(sep2);

    // Tower buttons section
    auto *towerGroup = new QGroupBox("防御塔");
    towerGroup->setStyleSheet("QGroupBox { color: #ecf0f1; font-weight: bold; padding-top: 10px; }");
    auto *towerLayout = new QVBoxLayout(towerGroup);
    towerLayout->setSpacing(5);

    TowerStats bottleStats = TowerStats{100, 120, 15, 500, false, 0, QColor(100,200,255), "瓶子塔"};
    TowerStats fanStats    = TowerStats{150, 100, 10, 600, true, 60, QColor(150,255,150), "风扇塔"};
    TowerStats starStats   = TowerStats{200, 180, 25, 900, false, 0, QColor(255,255,100), "星星塔"};
    TowerStats bombStats   = TowerStats{300, 90, 50, 1500, true, 80, QColor(255,120,50), "炸弹塔"};

    auto makeBtn = [&](const QString &text, const QString &style) -> QPushButton* {
        auto *btn = new QPushButton(text);
        btn->setStyleSheet(style);
        btn->setMinimumHeight(40);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFont(QFont("Microsoft YaHei", 10));
        return btn;
    };

    m_bottleBtn = makeBtn("🍼 瓶子塔 - 100💰",
        "QPushButton { background-color: #2980b9; color: white; border: 2px solid #3498db; border-radius: 6px; }"
        "QPushButton:hover { background-color: #3498db; }");
    m_bottleBtn->setToolTip("基础塔 | 伤害:15 | 范围:120 | 攻速:快");
    towerLayout->addWidget(m_bottleBtn);

    m_fanBtn = makeBtn("🌬️ 风扇塔 - 150💰",
        "QPushButton { background-color: #27ae60; color: white; border: 2px solid #2ecc71; border-radius: 6px; }"
        "QPushButton:hover { background-color: #2ecc71; }");
    m_fanBtn->setToolTip("减速溅射 | 伤害:15 | 范围:100 | 减速30%/2秒");
    towerLayout->addWidget(m_fanBtn);

    m_starBtn = makeBtn("⭐ 星星塔 - 200💰",
        "QPushButton { background-color: #f39c12; color: white; border: 2px solid #f1c40f; border-radius: 6px; }"
        "QPushButton:hover { background-color: #f1c40f; }");
    m_starBtn->setToolTip("远程塔 | 伤害:25 | 范围:180 | 攻速:慢");
    towerLayout->addWidget(m_starBtn);

    m_bombBtn = makeBtn("💣 炸弹塔 - 300💰",
        "QPushButton { background-color: #c0392b; color: white; border: 2px solid #e74c3c; border-radius: 6px; }"
        "QPushButton:hover { background-color: #e74c3c; }");
    m_bombBtn->setToolTip("溅射伤害 | 伤害:20 | 范围:90 | 溅射范围:80");
    towerLayout->addWidget(m_bombBtn);

    layout->addWidget(towerGroup);

    // Separator
    auto *sep3 = new QFrame();
    sep3->setFrameShape(QFrame::HLine);
    sep3->setStyleSheet("color: #7f8c8d;");
    layout->addWidget(sep3);

    // Control buttons
    m_startBtn = new QPushButton("▶ 开始下一波");
    m_startBtn->setStyleSheet(
        "QPushButton { background-color: #e67e22; color: white; border: 2px solid #f39c12;"
        " border-radius: 8px; padding: 8px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background-color: #f39c12; }");
    m_startBtn->setCursor(Qt::PointingHandCursor);
    m_startBtn->setMinimumHeight(45);
    layout->addWidget(m_startBtn);

    m_restartBtn = new QPushButton("🔄 重新开始");
    m_restartBtn->setStyleSheet(
        "QPushButton { background-color: #7f8c8d; color: white; border: 2px solid #95a5a6;"
        " border-radius: 8px; padding: 6px; }"
        "QPushButton:hover { background-color: #95a5a6; }");
    m_restartBtn->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_restartBtn);

    // Separator
    auto *sep4 = new QFrame();
    sep4->setFrameShape(QFrame::HLine);
    sep4->setStyleSheet("color: #7f8c8d;");
    layout->addWidget(sep4);

    // Speed and pause buttons
    m_speedBtn = new QPushButton("⏩ 二倍速");
    m_speedBtn->setStyleSheet(
        "QPushButton { background-color: #8e44ad; color: white; border: 2px solid #9b59b6;"
        " border-radius: 8px; padding: 8px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background-color: #9b59b6; }");
    m_speedBtn->setCursor(Qt::PointingHandCursor);
    m_speedBtn->setMinimumHeight(40);
    m_speedBtn->setCheckable(true);
    layout->addWidget(m_speedBtn);

    m_pauseBtn = new QPushButton("⏸️ 暂停");
    m_pauseBtn->setStyleSheet(
        "QPushButton { background-color: #c0392b; color: white; border: 2px solid #e74c3c;"
        " border-radius: 8px; padding: 8px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background-color: #e74c3c; }");
    m_pauseBtn->setCursor(Qt::PointingHandCursor);
    m_pauseBtn->setMinimumHeight(40);
    m_pauseBtn->setCheckable(true);
    layout->addWidget(m_pauseBtn);

    layout->addStretch();

    // Connections
    connect(m_bottleBtn, &QPushButton::clicked, this, &TowerPanel::onBottleClicked);
    connect(m_fanBtn, &QPushButton::clicked, this, &TowerPanel::onFanClicked);
    connect(m_starBtn, &QPushButton::clicked, this, &TowerPanel::onStarClicked);
    connect(m_bombBtn, &QPushButton::clicked, this, &TowerPanel::onBombClicked);
    connect(m_startBtn, &QPushButton::clicked, this, &TowerPanel::onStartWaveClicked);
    connect(m_restartBtn, &QPushButton::clicked, this, &TowerPanel::onRestartClicked);
    connect(m_speedBtn, &QPushButton::clicked, this, &TowerPanel::onSpeedClicked);
    connect(m_pauseBtn, &QPushButton::clicked, this, &TowerPanel::onPauseClicked);
}

void TowerPanel::updateMoney(int money)
{
    m_moneyLabel->setText(QString("💰 金币: %1").arg(money));
}

void TowerPanel::updateWave(int wave, int total)
{
    m_waveLabel->setText(QString("🌊 波次: %1/%2").arg(wave).arg(total));
}

void TowerPanel::updateLives(int lives)
{
    QString hearts;
    for (int i = 0; i < lives; ++i)
        hearts += "❤️";
    m_livesLabel->setText(QString("生命: %1  %2").arg(lives).arg(hearts));
}

void TowerPanel::updateButtonStyles()
{
    auto resetBtn = [](QPushButton *btn, const QString &baseColor, const QString &borderColor) {
        btn->setStyleSheet(
            QString("QPushButton { background-color: %1; color: white; border: 2px solid %2;"
                    " border-radius: 6px; } QPushButton:hover { background-color: %2; }")
                .arg(baseColor, borderColor));
    };

    resetBtn(m_bottleBtn, "#2980b9", "#3498db");
    resetBtn(m_fanBtn, "#27ae60", "#2ecc71");
    resetBtn(m_starBtn, "#f39c12", "#f1c40f");
    resetBtn(m_bombBtn, "#c0392b", "#e74c3c");

    auto highlightBtn = [](QPushButton *btn, const QString &borderColor) {
        btn->setStyleSheet(
            QString("QPushButton { background-color: #1a252f; color: white; border: 3px solid %1;"
                    " border-radius: 6px; }").arg(borderColor));
    };

    switch (m_selectedKind) {
    case TowerKind::Bottle: highlightBtn(m_bottleBtn, "#00ffff"); break;
    case TowerKind::Fan:    highlightBtn(m_fanBtn, "#00ff00"); break;
    case TowerKind::Star:   highlightBtn(m_starBtn, "#ffff00"); break;
    case TowerKind::Bomb:   highlightBtn(m_bombBtn, "#ff4444"); break;
    }
}

void TowerPanel::onBottleClicked()  { m_selectedKind = TowerKind::Bottle; updateButtonStyles(); emit towerSelected(m_selectedKind); }
void TowerPanel::onFanClicked()     { m_selectedKind = TowerKind::Fan; updateButtonStyles(); emit towerSelected(m_selectedKind); }
void TowerPanel::onStarClicked()    { m_selectedKind = TowerKind::Star; updateButtonStyles(); emit towerSelected(m_selectedKind); }
void TowerPanel::onBombClicked()    { m_selectedKind = TowerKind::Bomb; updateButtonStyles(); emit towerSelected(m_selectedKind); }

void TowerPanel::onStartWaveClicked()
{
    if (m_manager)
        m_manager->startNextWave();
}

void TowerPanel::clearSelection()
{
    auto resetBtn = [](QPushButton *btn, const QString &baseColor, const QString &borderColor) {
        btn->setStyleSheet(
            QString("QPushButton { background-color: %1; color: white; border: 2px solid %2;"
                    " border-radius: 6px; } QPushButton:hover { background-color: %2; }")
                .arg(baseColor, borderColor));
    };
    resetBtn(m_bottleBtn, "#2980b9", "#3498db");
    resetBtn(m_fanBtn, "#27ae60", "#2ecc71");
    resetBtn(m_starBtn, "#f39c12", "#f1c40f");
    resetBtn(m_bombBtn, "#c0392b", "#e74c3c");
}

void TowerPanel::onRestartClicked()
{
    if (m_manager) {
        auto result = QMessageBox::question(this, "重新开始",
                                            "确定要重新开始游戏吗？",
                                            QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes) {
            m_manager->restartGame();
            // Reset control button states
            m_speedBtn->setText("⏩ 二倍速");
            m_speedBtn->setStyleSheet(
                "QPushButton { background-color: #8e44ad; color: white; border: 2px solid #9b59b6;"
                " border-radius: 8px; padding: 8px; font-weight: bold; font-size: 13px; }"
                "QPushButton:hover { background-color: #9b59b6; }");
            m_pauseBtn->setText("⏸️ 暂停");
            m_pauseBtn->setStyleSheet(
                "QPushButton { background-color: #c0392b; color: white; border: 2px solid #e74c3c;"
                " border-radius: 8px; padding: 8px; font-weight: bold; font-size: 13px; }"
                "QPushButton:hover { background-color: #e74c3c; }");
        }
    }
}

void TowerPanel::onSpeedClicked()
{
    if (!m_manager) return;
    m_manager->toggleSpeed();
    bool fast = m_manager->speedMultiplier() > 1.5;
    m_speedBtn->setText(fast ? "⏩ 一倍速" : "⏩ 二倍速");
    m_speedBtn->setStyleSheet(
        fast
            ? "QPushButton { background-color: #6c3483; color: white; border: 3px solid #d2b4de;"
              " border-radius: 8px; padding: 8px; font-weight: bold; font-size: 13px; }"
              "QPushButton:hover { background-color: #7d3c98; }"
            : "QPushButton { background-color: #8e44ad; color: white; border: 2px solid #9b59b6;"
              " border-radius: 8px; padding: 8px; font-weight: bold; font-size: 13px; }"
              "QPushButton:hover { background-color: #9b59b6; }");
}

void TowerPanel::onPauseClicked()
{
    if (!m_manager) return;
    m_manager->togglePause();
    bool paused = m_manager->isPaused();
    m_pauseBtn->setText(paused ? "▶ 继续" : "⏸️ 暂停");
    m_pauseBtn->setStyleSheet(
        paused
            ? "QPushButton { background-color: #27ae60; color: white; border: 3px solid #2ecc71;"
              " border-radius: 8px; padding: 8px; font-weight: bold; font-size: 13px; }"
              "QPushButton:hover { background-color: #2ecc71; }"
            : "QPushButton { background-color: #c0392b; color: white; border: 2px solid #e74c3c;"
              " border-radius: 8px; padding: 8px; font-weight: bold; font-size: 13px; }"
              "QPushButton:hover { background-color: #e74c3c; }");
}
