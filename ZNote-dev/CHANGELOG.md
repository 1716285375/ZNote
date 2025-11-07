# Changelog

[English](CHANGELOG.md) | [中文](CHANGELOG_zh.md)

All notable changes to this project will be documented in this file.

本文档记录项目的所有重要变更。

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)，版本号遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

## [2.0.0] - 2025-01-XX

### Added
- Initial public release
- Multi-platform video download support (Bilibili and other platforms via yt-dlp)
- Batch download functionality for multiple videos and playlists
- Intelligent task queue with concurrent download support
- Download history management with JSON storage
- Modern UI with light/dark theme support
- Comprehensive settings system:
  - Configurable download paths
  - File naming (prefix/suffix)
  - Thread count configuration (1-10)
  - Sound notifications
  - Auto-open directory option
- Persistent configuration and history storage
- Sound notifications on download completion
- Auto-open download directory feature
- Application icon and branding

### Technical Details
- Built with Qt 6.9.1 and C++17
- CMake-based build system
- Layered architecture (UI, Service, Core, Utils)
- Thread-safe operations with QMutex
- JSON-based configuration and history storage
- Cross-platform support (Windows, Linux, macOS)

## [Unreleased]

### Planned
- Search and filter functionality in history
- Export/import history
- Download progress details
- Pause/resume downloads
- Download speed limits
- Proxy support
- More theme options

