# 贡献指南

[English](CONTRIBUTING.md) | [中文](CONTRIBUTING_zh.md)

感谢您对 ZNote 项目的关注！本文档提供了贡献指南和说明。

## 行为准则

- 保持尊重和体贴
- 欢迎新人并帮助他们学习
- 专注于建设性的反馈
- 尊重不同的观点和经验

## 如何贡献

### 报告 Bug

1. 检查 [Issues](https://github.com/1716285375/ZNote-dev/issues) 中是否已有相关报告
2. 如果没有，请创建新 issue，包含：
   - 清晰的标题和描述
   - 复现步骤
   - 预期行为 vs 实际行为
   - 系统信息 (操作系统、Qt 版本等)
   - 如适用，请附上截图

### 建议功能

1. 检查该功能是否已被建议
2. 创建新 issue，包含：
   - 清晰的功能描述
   - 使用场景和好处
   - 可能的实现方法（如果您有想法）

### 提交 Pull Request

1. **Fork 仓库**
2. **创建功能分支**
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **进行修改**
   - 遵循代码风格指南
   - 为复杂逻辑添加注释
   - 如需要，更新文档
4. **测试您的修改**
   - 在您的平台上构建和测试
   - 确保没有回归问题
5. **提交更改**
   ```bash
   git commit -m "Add: 您的更改描述"
   ```
   使用清晰、描述性的提交信息：
   - `Add:` 用于新功能
   - `Fix:` 用于 Bug 修复
   - `Update:` 用于更新现有功能
   - `Refactor:` 用于代码重构
   - `Docs:` 用于文档更改
6. **推送到您的 Fork**
   ```bash
   git push origin feature/your-feature-name
   ```
7. **创建 Pull Request**
   - 提供清晰的描述
   - 引用相关 issues
   - 如适用，添加截图

## 开发环境设置

### 前置要求

- Qt 6.9.1 或更高版本
- CMake 3.24 或更高版本
- C++17 兼容的编译器
- Git

### 构建

```bash
git clone https://github.com/1716285375/ZNote-dev.git
cd ZNote-dev/ZNote-dev
cmake --preset Qt-Debug
cmake --build out/build/debug
```

## 代码风格指南

### C++ 风格

- 遵循 Qt 编码规范
- 使用有意义的变量和函数名
- 保持函数简洁专注（尽可能 < 50 行）
- 为复杂逻辑添加注释
- 尽可能使用 `const`
- 优先使用 `nullptr` 而不是 `NULL` 或 `0`

### 文件组织

- 每个文件一个类
- 头文件放在 `include/`
- 源文件放在 `src/`
- 头文件和源文件的目录结构保持一致

### 命名规范

- **类**: PascalCase (例如: `MainWindow`, `DownloadService`)
- **函数**: camelCase (例如: `loadSettings()`, `onTaskFinished()`)
- **变量**: camelCase，成员变量加前缀 (例如: `m_configService`, `m_historyModel`)
- **常量**: UPPER_SNAKE_CASE (例如: `MAX_THREAD_COUNT`)

### 注释

- 单行注释使用 `//`
- 多行注释使用 `/* */`
- 为公共 API 添加文档
- 注释解释"为什么"而不是"做什么"

## 测试

- 在您的平台上测试您的更改
- 测试边界情况和错误条件
- 尽可能确保向后兼容
- 如果修改了现有功能，请更新测试

## 文档

- 如果添加了功能，请更新 README.md
- 为公共 API 添加代码注释
- 用户可见的更改请更新 CHANGELOG.md
- 保持文档与代码同步

## 审查流程

1. 所有 PR 需要至少一个审查
2. 及时处理审查意见
3. 保持 PR 专注且大小合理
4. 建设性地回应反馈

## 问题？

如有问题，请随时创建 issue 或通过讨论区联系。

感谢您为 ZNote 做出贡献！🎉

