#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H


#include "interface/ihistorybackend.h"

#include <QObject>
#include <QList>

enum class HistoryStorageType
{
    JSON = 0,
    SQLite,
    None
};

class HistoryManager : public QObject
{
    Q_OBJECT
public:
    explicit HistoryManager(HistoryStorageType type, const QString &path, QObject *parent = nullptr);

    void addHistory(const DownloadHistoryItem &item);
    void removeHistory(const DownloadHistoryItem &item);
    QList<DownloadHistoryItem> getHistory();

private:
    IHistoryBackend *backend;
};

#endif // HISTORYMANAGER_H
