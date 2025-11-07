#include "services/configservice.h"
#include "utils/logger.h"
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

ConfigService::ConfigService(const QString &configPath, QObject *parent)
    : IConfigService(parent)
    , m_dirty(false)
{
    if (configPath.isEmpty()) {
        QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(appDataPath);
        m_configPath = QDir(appDataPath).filePath("config.json");
    } else {
        m_configPath = configPath;
    }
    
    loadDefaults();
    load();
}

ConfigService::~ConfigService()
{
    if (m_dirty) {
        save();
    }
}

void ConfigService::setValue(const QString &key, const QVariant &value)
{
    QMutexLocker locker(&m_mutex);
    
    QStringList keys = key.split('.');
    m_config = setNestedValue(m_config, keys, value);
    m_dirty = true;
    
    emit valueChanged(key, value);
    LOG_DEBUG(QString("Config value set: %1 = %2").arg(key).arg(value.toString()));
}

QVariant ConfigService::getValue(const QString &key, const QVariant &defaultValue) const
{
    QMutexLocker locker(&m_mutex);
    
    QStringList keys = key.split('.');
    return getNestedValue(m_config, keys, defaultValue);
}

bool ConfigService::hasKey(const QString &key) const
{
    QMutexLocker locker(&m_mutex);
    
    QStringList keys = key.split('.');
    QJsonObject current = m_config;
    
    for (int i = 0; i < keys.size() - 1; ++i) {
        if (!current.contains(keys[i]) || !current[keys[i]].isObject()) {
            return false;
        }
        current = current[keys[i]].toObject();
    }
    
    return current.contains(keys.last());
}

void ConfigService::removeKey(const QString &key)
{
    QMutexLocker locker(&m_mutex);
    
    QStringList keys = key.split('.');
    if (keys.isEmpty()) return;
    
    QJsonObject current = m_config;
    QJsonObject *target = &m_config;
    
    for (int i = 0; i < keys.size() - 1; ++i) {
        if (!current.contains(keys[i]) || !current[keys[i]].isObject()) {
            return;
        }
        current = current[keys[i]].toObject();
        target = &current;
    }
    
    target->remove(keys.last());
    m_dirty = true;
    
    LOG_DEBUG(QString("Config key removed: %1").arg(key));
}

bool ConfigService::save()
{
    QMutexLocker locker(&m_mutex);
    
    QFile file(m_configPath);
    if (!file.open(QIODevice::WriteOnly)) {
        LOG_ERROR(QString("Failed to open config file for writing: %1").arg(m_configPath));
        return false;
    }
    
    QJsonDocument doc(m_config);
    qint64 bytesWritten = file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    if (bytesWritten == -1) {
        LOG_ERROR("Failed to write config file");
        return false;
    }
    
    m_dirty = false;
    emit configSaved();
    LOG_INFO("Configuration saved successfully");
    return true;
}

bool ConfigService::load()
{
    QMutexLocker locker(&m_mutex);
    
    QFile file(m_configPath);
    if (!file.exists()) {
        LOG_INFO(QString("Config file does not exist at %1, using defaults").arg(m_configPath));
        return true;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        LOG_ERROR(QString("Failed to open config file for reading: %1").arg(m_configPath));
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        LOG_ERROR(QString("Failed to parse config file: %1").arg(error.errorString()));
        return false;
    }
    
    if (!doc.isObject()) {
        LOG_ERROR("Config file does not contain a valid JSON object");
        return false;
    }
    
    // 合并配置文件中的值到默认配置中（配置文件的值会覆盖默认值）
    QJsonObject fileConfig = doc.object();
    LOG_DEBUG("Config file content:");
    LOG_DEBUG(QString::fromUtf8(QJsonDocument(fileConfig).toJson(QJsonDocument::Indented)));
    
    mergeConfig(m_config, fileConfig);
    
    LOG_DEBUG("Merged config content:");
    LOG_DEBUG(QString::fromUtf8(QJsonDocument(m_config).toJson(QJsonDocument::Indented)));
    
    // 验证关键配置项
    QVariant playSound = getNestedValue(m_config, QStringList{"download", "onComplete", "playSound"}, false);
    LOG_INFO(QString("After merge, download.onComplete.playSound = %1").arg(playSound.toBool()));
    
    m_dirty = false;
    emit configLoaded();
    LOG_INFO(QString("Configuration loaded successfully from %1").arg(m_configPath));
    return true;
}

bool ConfigService::isDirty() const
{
    QMutexLocker locker(&m_mutex);
    return m_dirty;
}

void ConfigService::setConfigPath(const QString &path)
{
    QMutexLocker locker(&m_mutex);
    m_configPath = path;
}

QString ConfigService::getConfigPath() const
{
    QMutexLocker locker(&m_mutex);
    return m_configPath;
}

void ConfigService::resetToDefaults()
{
    QMutexLocker locker(&m_mutex);
    m_config = QJsonObject();
    loadDefaults();
    m_dirty = true;
    LOG_INFO("Configuration reset to defaults");
}

QJsonObject ConfigService::getAllConfig() const
{
    QMutexLocker locker(&m_mutex);
    return m_config;
}

QJsonObject ConfigService::setNestedValue(QJsonObject obj, const QStringList &keys, const QVariant &value) const
{
    if (keys.isEmpty()) {
        return obj;
    }
    
    QString key = keys.first();
    if (keys.size() == 1) {
        obj[key] = QJsonValue::fromVariant(value);
        return obj;
    }
    
    QJsonObject child = obj.contains(key) && obj[key].isObject() ? obj[key].toObject() : QJsonObject();
    QStringList remainingKeys = keys;
    remainingKeys.removeFirst();
    obj[key] = setNestedValue(child, remainingKeys, value);
    return obj;
}

QVariant ConfigService::getNestedValue(const QJsonObject &obj, const QStringList &keys, const QVariant &defaultValue) const
{
    if (keys.isEmpty()) {
        return defaultValue;
    }
    
    QString key = keys.first();
    if (!obj.contains(key)) {
        LOG_DEBUG(QString("Config key not found: %1, using default").arg(keys.join(".")));
        return defaultValue;
    }
    
    if (keys.size() == 1) {
        QVariant value = obj[key].toVariant();
        LOG_DEBUG(QString("Config value for %1: %2").arg(key).arg(value.toString()));
        return value;
    }
    
    if (!obj[key].isObject()) {
        LOG_DEBUG(QString("Config key %1 is not an object, using default").arg(key));
        return defaultValue;
    }
    
    QStringList remainingKeys = keys;
    remainingKeys.removeFirst();
    return getNestedValue(obj[key].toObject(), remainingKeys, defaultValue);
}

void ConfigService::mergeConfig(QJsonObject &target, const QJsonObject &source) const
{
    // 递归合并 JSON 对象
    for (auto it = source.begin(); it != source.end(); ++it) {
        QString key = it.key();
        QJsonValue value = it.value();
        
        if (value.isObject() && target.contains(key) && target[key].isObject()) {
            // 如果都是对象，递归合并
            QJsonObject targetObj = target[key].toObject();
            QJsonObject sourceObj = value.toObject();
            mergeConfig(targetObj, sourceObj);
            target[key] = targetObj;
        } else {
            // 否则直接覆盖
            target[key] = value;
        }
    }
}

void ConfigService::loadDefaults()
{
    // 默认配置
    m_config = QJsonObject{
        {"download", QJsonObject{
            {"defaultPath", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)},
            {"threadCount", 4},
            {"retryCount", 3},
            {"timeout", 30000}
        }},
        {"ui", QJsonObject{
            {"theme", "light"},
            {"language", "zh_CN"},
            {"windowGeometry", QJsonObject{
                {"width", 1200},
                {"height", 800}
            }}
        }},
        {"logging", QJsonObject{
            {"level", "info"},
            {"consoleOutput", true},
            {"fileOutput", true}
        }}
    };
}
