#ifndef ICONFIGSERVICE_H
#define ICONFIGSERVICE_H

#include <QObject>
#include <QVariant>

class IConfigService : public QObject
{
    Q_OBJECT

public:
    explicit IConfigService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IConfigService() = default;

    // 配置管理
    virtual void setValue(const QString &key, const QVariant &value) = 0;
    virtual QVariant getValue(const QString &key, const QVariant &defaultValue = QVariant()) const = 0;
    virtual bool hasKey(const QString &key) const = 0;
    virtual void removeKey(const QString &key) = 0;
    
    // 持久化
    virtual bool save() = 0;
    virtual bool load() = 0;
    virtual bool isDirty() const = 0;

signals:
    void valueChanged(const QString &key, const QVariant &value);
    void configSaved();
    void configLoaded();
};

#endif // ICONFIGSERVICE_H
