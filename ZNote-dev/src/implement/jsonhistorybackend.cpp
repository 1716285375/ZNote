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

QVector<DownloadHistoryItem> JSONHistoryBackend::load()
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
        item.url = obj["url"].toString();
        item.title = obj["title"].toString();
        item.filePath = obj["filePath"].toString();
        item.startTime = QDateTime::fromString(obj["startTime"].toString(), Qt::ISODate);
        item.endTime = QDateTime::fromString(obj["endTime"].toString(), Qt::ISODate);
        items.append(item);
    }

    return items;
}

void JSONHistoryBackend::save(const QVector<DownloadHistoryItem> &items)
{
    QJsonArray arr;
    for(auto &item: items)
    {
        QJsonObject obj;
        obj["url"] = item.url;
        obj["title"] = item.title;
        obj["filePath"] = item.filePath;
        obj["startTime"] = item.startTime.toString(Qt::ISODate);
        obj["endTime"] = item.endTime.toString(Qt::ISODate);
        obj["status"] = item.status;
        arr.append(obj);
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(QJsonDocument(arr).toJson());
    }
}
