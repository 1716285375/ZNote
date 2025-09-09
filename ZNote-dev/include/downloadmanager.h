#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "taskqueue.h"
#include "task.h"
#include "urlparser.h"

#include <QObject>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>

struct DownloadHistoryItem;
struct ParsedEntry;
class HistoryManager;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);

    void parseUrl(const QString &url, const QString &savePath);

    void addTask(const DownloadTask& task);

    void addTasks(const QList<DownloadTask> &tasks);

    void start();

    void pause();

    QList<DownloadHistoryItem> getHistory() const;
    void addHistory(const DownloadHistoryItem&task);
    bool removeHistory(const QList<DownloadHistoryItem> &items);

signals:
    void logMessage(const QString &msg);
    void taskProgress(const int done, const float percent);
    void allFinished();

    void taskReady(const DownloadTask &task);
    void taskFinished(const DownloadTask &task);
    void taskError(const QString &err);

private slots:
    void onTaskFinished(const DownloadTask &task);

private:

    TaskQueue *queue;
    HistoryManager *history;
    QMutex mutex;
    int totalTasks = 0;
    int doneTasks = 0;
};

#endif // DOWNLOADMANAGER_H
