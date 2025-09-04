#include "configmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonValue>
#include <QMutexLocker>

ConfigManager& ConfigManager::instance()
{
	static ConfigManager _instance;
	return _instance;
}

ConfigManager::ConfigManager(QObject* parent)
	: QObject(parent)
{
}

ConfigManager::~ConfigManager()
{
}

QJsonObject ConfigManager::setNestedValue(QJsonObject obj, const QStringList& keys, const QVariant& value)
{
	if (keys.isEmpty())
		return obj;

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

QVariant ConfigManager::getNestedValue(const QJsonObject& obj, const QStringList& keys, const QVariant& defaultValue) const
{
	if (keys.isEmpty())
		return defaultValue;

	QString key = keys.first();
	if (!obj.contains(key))
		return defaultValue;

	if (keys.size() == 1)
		return obj[key].toVariant();

	if (!obj[key].isObject())
		return defaultValue;

	QStringList remainingKeys = keys;
	remainingKeys.removeFirst();
	return getNestedValue(obj[key].toObject(), remainingKeys, defaultValue);
}



void ConfigManager::setFilePath(const QString& path)
{
	{
		QMutexLocker locker(&mutex_);
		filePath_ = path;
	} // 提前解锁

	load(); // 立即加载
}

QVariant ConfigManager::getValue(const QString& path, const QVariant& defaultValue)
{
	QMutexLocker locker(&mutex_);
	QStringList keys = path.split('.');
	return getNestedValue(config_, keys, defaultValue);
}

void ConfigManager::setValue(const QString& path, const QVariant& value)
{
	QMutexLocker locker(&mutex_);
	QStringList keys = path.split('.');
	config_ = setNestedValue(config_, keys, value);
	emit valueChanged(path, value);
}

bool ConfigManager::save()
{
	QMutexLocker locker(&mutex_);
	if (filePath_.isEmpty()) return false;

	QFile file(filePath_);
	if (!file.open(QIODevice::WriteOnly))
		return false;

	QJsonDocument doc(config_);
	file.write(doc.toJson(QJsonDocument::Indented));
	file.close();
	return true;
}

bool ConfigManager::load()
{
	QMutexLocker locker(&mutex_);
	if (filePath_.isEmpty()) return false;

	QFile file(filePath_);
	if (!file.exists()) return false;
	if (!file.open(QIODevice::ReadOnly))
		return false;

	QByteArray data = file.readAll();
	file.close();

	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (!doc.isObject()) return false;

	config_ = doc.object();
	return true;
}
