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
