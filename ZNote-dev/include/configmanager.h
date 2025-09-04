#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QVariant>
#include <QMutex>

class ConfigManager : public QObject
{
	Q_OBJECT
public:
	// 获取单例
	static ConfigManager& instance();

	// 设置 JSON 文件路径
	void setFilePath(const QString& path);

	// 基础类型读取/写入
	void setValue(const QString& key, const QVariant& value);
	QVariant getValue(const QString& key, const QVariant& defaultValue = QVariant());

	// 保存到文件
	bool save();

	// 从文件加载
	bool load();

signals:
	void valueChanged(const QString& path, const QVariant& value);

private:
	explicit ConfigManager(QObject* parent = nullptr);
	~ConfigManager();

	Q_DISABLE_COPY(ConfigManager)

	QJsonObject setNestedValue(QJsonObject obj, const QStringList& keys, const QVariant& value);
	QVariant getNestedValue(const QJsonObject& obj, const QStringList& keys, const QVariant& defaultValue) const;

	QString filePath_;
	QJsonObject config_;
	QMutex mutex_;
};

#endif // CONFIGMANAGER_H
