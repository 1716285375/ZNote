#include "implement/jsonhistorybackend.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>



JSONHistoryBackend::JSONHistoryBackend(const QString &filePath)
    : filePath(filePath)
{
    cache = load();
}

void JSONHistoryBackend::add(const DownloadHistoryItem &item)
{
    cache.append(item);
    save(cache);
}

void JSONHistoryBackend::remove(const QString& id)
{

}

QList<DownloadHistoryItem> JSONHistoryBackend::load()
{
    QVector<DownloadHistoryItem> items;
    QFile file(filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
    {
        return items;
    }

    auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray())
    {
        return items;
    }

    for (auto val : doc.array())
    {
        QJsonObject obj = val.toObject();
        DownloadHistoryItem item;
		// 解析基本信息
		item.vid = obj["vid"].toString();
		item.title = obj["title"].toString();
		item.savePath = obj["savePath"].toString();
		item.startTime = QDateTime::fromString(obj["startTime"].toString(), Qt::ISODate);
		item.endTime = QDateTime::fromString(obj["endTime"].toString(), Qt::ISODate);

		// 解析 UrlType (Single / PlayList)
		QString typeStr = obj["type"].toString();
		if (typeStr == "Single") {
			item.type = UrlType::Single;
		}
		else if (typeStr == "PlayList") {
			item.type = UrlType::Lists;
		}
		else {
			item.type = UrlType::Unknown; // 默认值
		}

		// 解析 DownloadStatus (success / failed / canceled)
		QString statusStr = obj["status"].toString();
		if (statusStr == "success") {
			item.status = DownloadStatus::Success;
		}
		else if (statusStr == "failed") {
			item.status = DownloadStatus::Failed;
		}
		else if (statusStr == "canceled") {
			item.status = DownloadStatus::Canceled;
		}
		else {
			item.status = DownloadStatus::Failed; // 默认值
		}
    }

    return items;
}

void JSONHistoryBackend::save(const QVector<DownloadHistoryItem> &items)
{
    QJsonArray arr;
	for (auto& item : items)
	{
		QJsonObject obj;

		// 将 DownloadHistoryItem 的属性转换为 QJsonObject
		obj["vid"] = item.vid;
		obj["title"] = item.title;
		obj["index"] = item.index;
		obj["playlistCount"] = item.playlistCount;

		// Convert UrlType (item.type) to string
		switch (item.type) {
		case UrlType::Single:
			obj["type"] = "Single";
			break;
		case UrlType::Lists:
			obj["type"] = "PlayList";
			break;
		default:
			obj["type"] = "Unknown";  // 默认值
			break;
		}

		obj["savePath"] = item.savePath;
		obj["startTime"] = item.startTime.toString(Qt::ISODate);
		obj["endTime"] = item.endTime.toString(Qt::ISODate);

		// Convert DownloadStatus (item.status) to string
		switch (item.status) {
		case DownloadStatus::Success:
			obj["status"] = "success";
			break;
		case DownloadStatus::Failed:
			obj["status"] = "failed";
			break;
		case DownloadStatus::Canceled:
			obj["status"] = "canceled";
			break;
		default:
			obj["status"] = "failed";  // 默认值
			break;
		}

		arr.append(obj);  // 将 QJsonObject 添加到 QJsonArray
	}


    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(QJsonDocument(arr).toJson());
    }
}
