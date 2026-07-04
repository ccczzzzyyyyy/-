# 保卫萝卜 - Carrot Defense
基于 Qt 6 和 C++17 开发的 2D 塔防游戏

## 目录
- [游戏简介](#游戏简介)
- [运行环境](#运行环境)
- [编译运行](#编译运行)
  - [Linux / macOS](#linux--macos)
  - [Windows MinGW](#windows-mingw)
  - [Windows MSVC 2022](#windows-msvc-2022)
- [目录结构](#目录结构)
- [架构设计](#架构设计)
- [核心类说明](#核心类说明)
  - [MainWindow](#mainwindow)
  - [GameScene](#gamescene)
  - [Map](#map)
  - [Tower 防御塔](#tower-防御塔)
  - [Enemy 敌人](#enemy-敌人)
  - [Projectile 抛射物](#projectile-抛射物)
  - [WaveController](#wavecontroller)
  - [TowerPanel](#towerpanel)
  - [TowerDestroyEffect](#towerdestroyeffect)
- [操作说明](#操作说明)
- [快捷键补充](#快捷键补充)
- [经济系统](#经济系统)
- [胜负规则](#胜负规则)
- [可扩展功能（待开发）](#可扩展功能待开发)
- [开源协议](#开源协议)

## 游戏简介
玩家需要在蜿蜒的路径上建造和升级防御塔，阻止一波波敌人抵达终点（胡萝卜）。
每波敌人被消灭后可获得金币，用于建造更多防御塔或升级现有防御塔。
成功抵御全部 20 波敌人即可获胜；若胡萝卜生命值归零，则游戏失败。

## 运行环境
- Qt 6.x（开发使用 Qt 6.11.0）
- C++17 标准
- 编译器支持：MSVC 2022 64-bit / MinGW / GCC

## 编译运行
### Linux / macOS
```bash
qmake CarrotDefense.pro
make
./CarrotDefense
Windows MinGW
bash
运行
qmake CarrotDefense.pro
mingw32-make
.\CarrotDefense.exe
Windows MSVC 2022
先在 VS 终端加载编译环境：
cmd
vcvars64.bat
qmake CarrotDefense.pro
nmake
.\release\CarrotDefense.exe
图形化编译（全平台通用）
使用 Qt Creator 打开 CarrotDefense.pro，直接点击「构建并运行」。
目录结构
text
CarrotDefense/
├── CarrotDefense.pro          # Qt 项目文件
├── main.cpp                   # 程序入口
├── resources.qrc              # Qt 资源文件
├── mainwindow.h/cpp           # 主窗口（布局容器）
├── gamescene.h/cpp            # 游戏场景（核心逻辑循环）
├── gameview.h/cpp             # 游戏视图（鼠标交互）
├── gamemanager.h/cpp          # 游戏状态管理（金币、生命、胜负）
├── map.h/cpp                  # 地图（瓦片网格、路径）
├── tower.h/cpp                # 防御塔实体
├── enemy.h/cpp                # 敌人实体
├── projectile.h/cpp           # 抛射物实体
├── wavecontroller.h/cpp       # 波次生成系统
├── towerpanel.h/cpp           # 侧边栏 UI（建造与控制面板）
├── towereffect.h/cpp          # 防御塔摧毁特效
├── bottle.png                 # 瓶子塔精灵
├── fan.png                    # 风扇塔精灵
├── star.png                   # 星星塔精灵
└── bomb.png                   # 炸弹塔精灵
核心类说明
MainWindow
顶级窗口容器，负责布局编排与信号槽连接。
将 TowerPanel、GameView、GameManager 组装在一起，处理塔的升级 / 出售弹窗对话框。
GameScene
核心游戏循环，以 60FPS 驱动每帧更新，执行流程：
生成敌人 → 移动敌人 → 防御塔瞄准射击 → 抛射物飞行 → 碰撞检测 → 清理死亡对象
支持 1x / 2x 游戏速度切换。
Map
10 列 × 8 行瓦片网格地图，瓦片分为三类：
路径：敌人行走通道
草地：可建造防御塔
不可建造区域：装饰障碍物
S 形路径从地图左侧蜿蜒至右下角胡萝卜终点。
Tower (防御塔)
共 4 种类型，最高可升级至 3 级：
类型	基础费用	基础伤害	射程	射速	特殊效果
瓶子塔	100	15	120px	500ms	无，均衡基础塔
风扇塔	150	15	100px	600ms	溅射 60px，命中减速 30% 持续 2 秒
星星塔	200	25	180px	900ms	命中后分裂 5 枚小子弹
炸弹塔	300	45	90px	1500ms	溅射 80px，大范围群体伤害
升级规则：升级费用 = 基础费用 × 当前等级
出售返还金币 = 基础费用 × 当前等级 ÷ 2
Enemy (敌人)
4 种程序化卡通怪物：
类型	生命值	移动速度	击杀奖励	抵达终点扣除生命
普通	150	70 px/s	30	1
快速	90	110 px/s	40	1
坦克	450	45 px/s	70	2
Boss	1200+	35 px/s	300	5
Boss 特殊机制：
第 10、20 波出现 Boss，每 7 秒随机摧毁一座场上防御塔，摧毁无金币补偿。
Projectile (抛射物)
分为两类：
主抛射物：自动追踪目标飞行
子抛射物：固定短距离飞行
支持 4 种命中模式：单体伤害、溅射伤害、分裂弹头、子抛射物寻敌。
WaveController
总共 20 波敌人生成调度：
前 4 波：仅普通敌人
第 5 波：首次刷新 Boss
5~19 波：敌人种类、数量逐步递增
第 20 波：最终 Boss 带领混合大军
TowerPanel
左侧深色侧边栏 UI：
HUD 实时展示金币、当前波次、剩余生命值
4 个防御塔建造按钮，选中高亮
全局控制：倍速切换、暂停 / 继续、下一波、重新开始
TowerDestroyEffect
防御塔被 Boss 摧毁时的动画特效，包含冲击波、闪光、粒子碎片，总时长 600ms。
操作说明
操作	操作方式
建造防御塔	点击左侧塔按钮 → 点击地图草地瓦片
查看 / 升级 / 出售塔	左键 / 右键点击已放置的防御塔
取消建造模式	右键空地 或 按下 Esc 键
开启下一波敌人	点击「开始下一波」按钮
切换二倍速	点击「二倍速」按钮切换
暂停 / 继续游戏	点击「暂停」按钮切换
重新开局	点击「重新开始」按钮
快捷键补充
空格：快速暂停 / 继续
1：一倍速
2：二倍速
Esc：取消建造模式
经济系统
初始金币：300
防御塔基础建造费用：100 / 150 / 200 / 300
敌人击杀奖励：普通 30 / 快速 40 / 坦克 70 / Boss300
升级费用 = 基础费用 × 当前等级
出售返还金币 = 基础费用 × 当前等级 ÷ 2
胜负规则
胜利：成功抵御全部 20 波敌人，剩余生命值无额外金币奖励
失败：敌人抵达胡萝卜终点扣除生命值，生命值归零时游戏结束
可扩展功能（待开发）
新增更多防御塔、敌人种类
本地存档 / 读档系统
自定义地图编辑器
音效、背景音乐系统
难度选择（简单 / 普通 / 困难）
塔皮肤、特效美化
开源协议
本项目采用 MIT 开源协议，可自由修改、分发商用，详见仓库根目录 LICENSE 文件。

---

# 二、配套新增 CONTRIBUTING.md（贡献规范，新建文件）
文件名：`CONTRIBUTING.md`
```markdown
# Carrot Defense 贡献指南
欢迎提交 Issue / PR 参与本塔防项目开发！

## 1. 提 Issue 规范
1. Bug 反馈：标题以 `[Bug]` 开头，写明复现步骤、系统环境、报错信息
2. 文档优化：标题以 `[Docs]` 开头，描述需要修改的内容
3. 新功能需求：标题以 `[Feature]` 开头，说明功能用途与设计思路
4. 新手友好任务会打上 `good first issue` 标签，适合初次开源贡献

## 2. 提交 PR 流程
1. Fork 本仓库到自己账号
2. 新建分支，分支命名规范：
   - 文档修改：`docs/xxx` 例：`docs/fix-readme-format`
   - 功能开发：`feature/xxx`
   - Bug修复：`fix/xxx`
3. 本地修改完成后提交代码，commit 规范：
   - `docs: 优化README编译说明`
   - `feature: 新增音效模块`
   - `fix: 修复Boss摧毁塔闪退bug`
4. Push 到自己 Fork 仓库，发起 PR
5. PR 正文填写 `Closes #工单编号`，自动关联对应 Issue
6. 等待维护者代码审核，按意见修改后更新提交

## 3. 代码/文档约束
1. 文档修改仅改动 Markdown，不修改业务代码
2. C++ 代码遵循 Qt 官方编码规范，统一 4 空格缩进
3. 新增功能必须同步更新 README 文档说明
4. 不提交无关冗余文件、编译产物、临时缓存
