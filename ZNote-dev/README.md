# ZNote - Bilibili Video Downloader

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Qt](https://img.shields.io/badge/Qt-6.9.1-green.svg)](https://www.qt.io/)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)

[English](README.md) | [中文](README_zh.md)

A modern, cross-platform desktop application for downloading videos from Bilibili and other platforms using yt-dlp. Built with Qt6 and C++17.

一个现代化的跨平台桌面应用程序，用于从 Bilibili 和其他平台下载视频，基于 yt-dlp 构建。使用 Qt6 和 C++17 开发。

## ✨ Features

- 🎬 **Multi-platform Support**: Download videos from Bilibili and other platforms supported by yt-dlp
- 📥 **Batch Download**: Download multiple videos or entire playlists at once
- 🎯 **Smart Queue Management**: Intelligent task queue with concurrent download support
- 📊 **Download History**: Track all your downloads with detailed history records
- 🎨 **Modern UI**: Beautiful, responsive interface with light/dark theme support
- ⚙️ **Customizable Settings**: Configure download paths, file naming, thread count, and more
- 🔊 **Notifications**: Sound notifications when downloads complete
- 📁 **Auto-open Directory**: Automatically open download directory when finished
- 💾 **Persistent Storage**: JSON-based configuration and history storage

## 📋 Requirements

- **Qt 6.9.1** or later (Core, Gui, Widgets, Multimedia)
- **CMake 3.24** or later
- **C++17** compatible compiler
- **yt-dlp** (automatically detected or can be placed in application directory)

### Platform Support

- ✅ Windows (tested on Windows 10/11)
- ✅ Linux (should work with Qt6)
- ✅ macOS (should work with Qt6)

## 🚀 Installation

### Building from Source

1. **Clone the repository**
   ```bash
   git clone https://github.com/1716285375/ZNote-dev.git
   cd ZNote-dev/ZNote-dev
   ```

2. **Install Qt6**
   - Download and install Qt6 from [qt.io](https://www.qt.io/download)
   - Make sure Qt6 is in your PATH or set `CMAKE_PREFIX_PATH`

3. **Configure and build**
   ```bash
   # Using CMake presets (recommended)
   cmake --preset Qt-Debug
   cmake --build out/build/debug
   
   # Or manually
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```

4. **Install yt-dlp** (if not in PATH)
   - Download from [yt-dlp releases](https://github.com/yt-dlp/yt-dlp/releases)
   - Place `yt-dlp.exe` (Windows) or `yt-dlp` (Linux/macOS) in the application directory

### Windows Quick Start

1. Download the latest release from [Releases](https://github.com/1716285375/ZNote-dev/releases)
2. Extract and run `ZNote-dev.exe`
3. Place `yt-dlp.exe` in the same directory as the executable

## 📖 Usage

### Basic Usage

1. **Launch the application**
2. **Enter a video URL** in the input field (supports Bilibili video URLs and playlists)
3. **Click "解析" (Parse)** to analyze the video
4. **Select videos** you want to download from the list
5. **Configure download settings**:
   - Set download directory
   - Choose file prefix/suffix
   - Set thread count (1-10)
6. **Click "下载" (Download)** to start downloading

### Configuration

The application uses `config.json` for settings. A default configuration is created on first run.

**Example `config.json`:**
```json
{
  "download": {
    "defaultPath": "C:/Downloads/Bilibili",
    "threadCount": 4,
    "filePrefix": "ZN_",
    "fileSuffix": "_video",
    "onComplete": {
      "playSound": true,
      "autoOpenDir": false
    }
  }
}
```

### Settings

- **Default Path**: Default download directory
- **Thread Count**: Number of concurrent downloads (1-10)
- **File Prefix/Suffix**: Customize downloaded file names
- **Play Sound**: Enable sound notification when download completes
- **Auto Open Directory**: Automatically open download folder when finished

## 📁 Project Structure

```
ZNote-dev/
├── assets/              # Resources (icons, styles, sounds)
├── include/             # Header files
│   ├── app/            # Application core
│   ├── component/      # UI components
│   ├── core/           # Core functionality
│   ├── services/       # Service layer
│   ├── ui/             # UI classes
│   └── utils/           # Utility functions
├── src/                # Source files
├── tests/              # Test files
├── CMakeLists.txt      # CMake configuration
├── resources.qrc       # Qt resource file
└── README.md          # This file
```

## 🛠️ Development

### Building

```bash
# Debug build
cmake --preset Qt-Debug
cmake --build out/build/debug

# Release build
cmake --preset Qt-Release  # (if configured)
cmake --build out/build/release
```

### Code Style

- Follow Qt coding conventions
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and small

### Architecture

The application follows a layered architecture:

- **UI Layer**: Qt widgets and models
- **Service Layer**: Business logic (DownloadService, ConfigService, HistoryService)
- **Core Layer**: Core functionality (URL parsing, video downloading)
- **Utils Layer**: Utility functions and helpers

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) ([中文](CONTRIBUTING_zh.md)) for details on our code of conduct and the process for submitting pull requests.

欢迎贡献！请阅读贡献指南了解详情。

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- [yt-dlp](https://github.com/yt-dlp/yt-dlp) - The amazing video downloader
- [Qt](https://www.qt.io/) - The cross-platform framework
- [Bilibili](https://www.bilibili.com/) - The video platform

## 📧 Contact

- **Issues**: [GitHub Issues](https://github.com/1716285375/ZNote-dev/issues)
- **Discussions**: [GitHub Discussions](https://github.com/1716285375/ZNote-dev/discussions)

## 📅 Changelog

### Version 1.0.0
- ✨ Initial release
- 🎬 Multi-platform video download support
- 📊 Download history management
- 🎨 Modern UI with theme support
- ⚙️ Comprehensive settings system
- 🔊 Sound notifications
- 📁 Auto-open directory feature

---

**Note**: This project is for educational purposes. Please respect the terms of service of video platforms and copyright laws.
