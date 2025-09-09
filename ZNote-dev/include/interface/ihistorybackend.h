#ifndef IHISTORYBACKEND_H
#define IHISTORYBACKEND_H

#include <QList>
#include <QDateTime>
#include "task.h"




class IHistoryBackend
{
public:
    virtual ~IHistoryBackend() = default;
    virtual void add(const DownloadHistoryItem &item) = 0;
    virtual QList<DownloadHistoryItem> load() = 0;
    virtual void remove(const QString& id) = 0;
    virtual void save(const QVector<DownloadHistoryItem> &items) = 0;
};

#endif // IHISTORYBACKEND_H
