# Contributing to ZNote

[English](CONTRIBUTING.md) | [中文](CONTRIBUTING_zh.md)

Thank you for your interest in contributing to ZNote! This document provides guidelines and instructions for contributing.

感谢您对 ZNote 项目的关注！本文档提供了贡献指南和说明。

## Code of Conduct

- Be respectful and considerate
- Welcome newcomers and help them learn
- Focus on constructive feedback
- Respect different viewpoints and experiences

## How to Contribute

### Reporting Bugs

1. Check if the bug has already been reported in [Issues](https://github.com/1716285375/ZNote-dev/issues)
2. If not, create a new issue with:
   - Clear title and description
   - Steps to reproduce
   - Expected vs actual behavior
   - System information (OS, Qt version, etc.)
   - Screenshots if applicable

### Suggesting Features

1. Check if the feature has already been suggested
2. Create a new issue with:
   - Clear description of the feature
   - Use cases and benefits
   - Possible implementation approach (if you have ideas)

### Pull Requests

1. **Fork the repository**
2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Make your changes**
   - Follow the code style guidelines
   - Add comments for complex logic
   - Update documentation if needed
4. **Test your changes**
   - Build and test on your platform
   - Ensure no regressions
5. **Commit your changes**
   ```bash
   git commit -m "Add: Description of your changes"
   ```
   Use clear, descriptive commit messages:
   - `Add:` for new features
   - `Fix:` for bug fixes
   - `Update:` for updates to existing features
   - `Refactor:` for code refactoring
   - `Docs:` for documentation changes
6. **Push to your fork**
   ```bash
   git push origin feature/your-feature-name
   ```
7. **Create a Pull Request**
   - Provide a clear description
   - Reference related issues
   - Add screenshots if applicable

## Development Setup

### Prerequisites

- Qt 6.9.1 or later
- CMake 3.24 or later
- C++17 compatible compiler
- Git

### Building

```bash
git clone https://github.com/1716285375/ZNote-dev.git
cd ZNote-dev/ZNote-dev
cmake --preset Qt-Debug
cmake --build out/build/debug
```

## Code Style Guidelines

### C++ Style

- Follow Qt coding conventions
- Use meaningful names for variables and functions
- Keep functions focused and small (< 50 lines when possible)
- Add comments for complex logic
- Use `const` whenever possible
- Prefer `nullptr` over `NULL` or `0`

### File Organization

- One class per file
- Header files in `include/`
- Source files in `src/`
- Match directory structure between headers and sources

### Naming Conventions

- **Classes**: PascalCase (e.g., `MainWindow`, `DownloadService`)
- **Functions**: camelCase (e.g., `loadSettings()`, `onTaskFinished()`)
- **Variables**: camelCase with prefix for members (e.g., `m_configService`, `m_historyModel`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_THREAD_COUNT`)

### Comments

- Use `//` for single-line comments
- Use `/* */` for multi-line comments
- Document public APIs
- Explain "why" not "what" in comments

## Testing

- Test your changes on your platform
- Test edge cases and error conditions
- Ensure backward compatibility when possible
- Update tests if you modify existing functionality

## Documentation

- Update README.md if you add features
- Add code comments for public APIs
- Update CHANGELOG.md for user-facing changes
- Keep documentation in sync with code

## Review Process

1. All PRs require at least one review
2. Address review comments promptly
3. Keep PRs focused and reasonably sized
4. Respond to feedback constructively

## Questions?

Feel free to open an issue for questions or reach out through discussions.

Thank you for contributing to ZNote! 🎉

