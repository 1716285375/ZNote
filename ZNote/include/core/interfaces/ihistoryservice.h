#ifndef IHISTORYSERVICE_H
#define IHISTORYSERVICE_H

#include "core/download/task.h"
#include <QObject>
#include <QList>

class IHistoryService : public QObject
{
    Q_OBJECT

public:
    explicit IHistoryService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IHistoryService() = default;

    // 历史记录管理
    virtual QList<DownloadHistoryItem> getHistory() const = 0;
    virtual QList<DownloadHistoryItem> getHistoryByStatus(DownloadStatus status) const = 0;
    virtual QList<DownloadHistoryItem> getHistoryByDateRange(const QDateTime &start, const QDateTime &end) const = 0;
    
    // 添加/删除历史记录
    virtual void addHistory(const DownloadHistoryItem &item) = 0;
    virtual void addHistory(const QList<DownloadHistoryItem> &items) = 0;
    virtual void removeHistory(const DownloadHistoryItem &item) = 0;
    virtual void removeHistory(const QList<DownloadHistoryItem> &items) = 0;
    virtual void clearHistory() = 0;
    
    // 查询
    virtual DownloadHistoryItem findHistoryById(const QString &id) const = 0;
    virtual QList<DownloadHistoryItem> searchHistory(const QString &keyword) const = 0;
    
    // 统计
    virtual int getHistoryCount() const = 0;
    virtual int getSuccessCount() const = 0;
    virtual int getFailedCount() const = 0;

signals:
    void historyAdded(const DownloadHistoryItem &item);
    void historyRemoved(const DownloadHistoryItem &item);
    void historyCleared();
};

#endif // IHISTORYSERVICE_H
