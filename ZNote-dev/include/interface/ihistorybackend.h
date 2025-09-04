#ifndef IHISTORYBACKEND_H
#define IHISTORYBACKEND_H

#include <QVector>
#include <QDateTime>

struct DownloadHistoryItem
{
    QString url;
    QString title;
    QString filePath;
    QDateTime startTime;
    QDateTime endTime;
    QString status; // success, failed, canceled
};


class IHistoryBackend
{
public:
    virtual ~IHistoryBackend() = default;
    virtual void add(const DownloadHistoryItem &item) = 0;
    virtual QVector<DownloadHistoryItem> load() = 0;
    virtual void save(const QVector<DownloadHistoryItem> &items) = 0;
};

#endif // IHISTORYBACKEND_H
