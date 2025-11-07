/**
 * @file configservice.h
 * @brief Configuration service implementation
 * 
 * Provides JSON-based configuration management with nested key support.
 */

#ifndef CONFIGSERVICE_H
#define CONFIGSERVICE_H

#include "core/interfaces/iconfigservice.h"
#include <QJsonObject>
#include <QMutex>
#include <QString>

/**
 * @class ConfigService
 * @brief Thread-safe configuration service using JSON storage
 * 
 * This service provides:
 * - Nested key support (e.g., "download.defaultPath")
 * - Thread-safe operations
 * - Automatic save/load
 * - Default value merging
 * - Dirty flag tracking
 */
class ConfigService : public IConfigService
{
    Q_OBJECT

public:
    /**
     * @brief Construct a ConfigService
     * @param configPath Path to config file (empty = use default location)
     * @param parent Parent QObject
     */
    explicit ConfigService(const QString &configPath = QString(), QObject *parent = nullptr);
    ~ConfigService() override;

    // IConfigService interface
    /**
     * @brief Set a configuration value
     * @param key Dot-separated key (e.g., "download.defaultPath")
     * @param value Value to set
     */
    void setValue(const QString &key, const QVariant &value) override;
    
    /**
     * @brief Get a configuration value
     * @param key Dot-separated key
     * @param defaultValue Default value if key doesn't exist
     * @return Configuration value or defaultValue
     */
    QVariant getValue(const QString &key, const QVariant &defaultValue = QVariant()) const override;
    
    /**
     * @brief Check if a key exists
     * @param key Dot-separated key
     * @return true if key exists
     */
    bool hasKey(const QString &key) const override;
    
    /**
     * @brief Remove a configuration key
     * @param key Dot-separated key to remove
     */
    void removeKey(const QString &key) override;
    
    /**
     * @brief Save configuration to file
     * @return true if saved successfully
     */
    bool save() override;
    
    /**
     * @brief Load configuration from file
     * @return true if loaded successfully
     */
    bool load() override;
    
    /**
     * @brief Check if configuration has unsaved changes
     * @return true if dirty
     */
    bool isDirty() const override;

    /**
     * @brief Set the configuration file path
     * @param path Path to config file
     */
    void setConfigPath(const QString &path);
    
    /**
     * @brief Get the current configuration file path
     * @return Config file path
     */
    QString getConfigPath() const;
    
    /**
     * @brief Reset configuration to default values
     */
    void resetToDefaults();
    
    /**
     * @brief Get all configuration as JSON object
     * @return Complete configuration object
     */
    QJsonObject getAllConfig() const;

private:
    QJsonObject setNestedValue(QJsonObject obj, const QStringList &keys, const QVariant &value) const;
    QVariant getNestedValue(const QJsonObject &obj, const QStringList &keys, const QVariant &defaultValue) const;
    void mergeConfig(QJsonObject &target, const QJsonObject &source) const;  // 合并配置
    void loadDefaults();

    QString m_configPath;
    QJsonObject m_config;
    mutable QMutex m_mutex;
    bool m_dirty;
};

#endif // CONFIGSERVICE_H
