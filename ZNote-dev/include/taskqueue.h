#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <QObject>
#include <QQueue>
#include <QList>

class VideoDownloader;
class DownloadTask;

class TaskQueue : public QObject
{
    Q_OBJECT
public:
    explicit TaskQueue(QObject *parent = nullptr, int max = 2);

    void enqueue(const DownloadTask &task);

    void startQueue();

    void pauseQueue();

    bool isPaused() const;

signals:
    void logMessage(const QString &msg);
    void taskFinished(const DownloadTask &task);
    void allFinished();

private slots:
    void onTaskFinished(VideoDownloader *downloader, const DownloadTask &task);

private:
    void startNext();

    QQueue<DownloadTask> pending;
    QList<VideoDownloader*> running;
    int maxConcurrent;
    bool paused;
};

#endif // TASKQUEUE_H
