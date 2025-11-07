#include "utils/stylemanager.h"
#include "utils/logger.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringConverter>

StyleManager* StyleManager::s_instance = nullptr;

StyleManager::StyleManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(Light)
{
}

StyleManager* StyleManager::instance()
{
    if (!s_instance) {
        s_instance = new StyleManager(qApp);
    }
    return s_instance;
}

void StyleManager::setTheme(Theme theme)
{
    if (m_currentTheme == theme) {
        return;
    }

    if (loadStyle(theme)) {
        m_currentTheme = theme;
        emit themeChanged(theme);
        LOG_INFO(QString("Theme changed to: %1").arg(theme == Light ? "Light" : "Dark"));
    } else {
        LOG_ERROR(QString("Failed to load theme: %1").arg(theme == Light ? "Light" : "Dark"));
    }
}

bool StyleManager::loadStyle(Theme theme)
{
    QString resourcePath;
    if (theme == Light) {
        resourcePath = ":/qss/light.qss";
    } else {
        resourcePath = ":/qss/dark.qss";
    }

    return loadStyleFromResource(resourcePath);
}

bool StyleManager::loadStyleFromResource(const QString &resourcePath)
{
    QFile file(resourcePath);
    if (!file.exists()) {
        LOG_ERROR(QString("Style file not found: %1").arg(resourcePath));
        return false;
    }

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        LOG_ERROR(QString("Failed to open style file: %1").arg(resourcePath));
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    m_styleSheet = stream.readAll();
    file.close();

    if (m_styleSheet.isEmpty()) {
        LOG_WARNING(QString("Style file is empty: %1").arg(resourcePath));
        return false;
    }

    qApp->setStyleSheet(m_styleSheet);
    LOG_INFO(QString("Style loaded from resource: %1").arg(resourcePath));
    return true;
}

bool StyleManager::loadStyleFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        LOG_ERROR(QString("Style file not found: %1").arg(filePath));
        return false;
    }

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        LOG_ERROR(QString("Failed to open style file: %1").arg(filePath));
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    m_styleSheet = stream.readAll();
    file.close();

    if (m_styleSheet.isEmpty()) {
        LOG_WARNING(QString("Style file is empty: %1").arg(filePath));
        return false;
    }

    qApp->setStyleSheet(m_styleSheet);
    LOG_INFO(QString("Style loaded from file: %1").arg(filePath));
    return true;
}

