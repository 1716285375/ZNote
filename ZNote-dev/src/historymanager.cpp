#include "historymanager.h"
#include "implement/jsonhistorybackend.h"
#include "implement/sqlitehisotrybackend.h"


HistoryManager::HistoryManager(HistoryStorageType type, const QString &path, QObject *parent)
    : QObject(parent)
	, backend(nullptr)
{
    if (type == HistoryStorageType::JSON)
    {
        backend = new JSONHistoryBackend(path);
    }
    else if (type == HistoryStorageType::SQLite)
    {
        backend = new SQLiteHistoryBackend(path);
    }

}

void HistoryManager::addHistory(const DownloadHistoryItem &item)
{
    backend->add(item);
}

QVector<DownloadHistoryItem> HistoryManager::getHistory()
{
    return backend->load();
}
