#include "taskqueue.h"

#include "videodownloader.h"

TaskQueue::TaskQueue(int max, QObject *parent)
    : QObject(parent), maxConcurrent(max)
{
    qDebug() << "Max concurrent threads set to:" << maxConcurrent;
}

void TaskQueue::enqueue(const DownloadTask &task)
{
    pending.enqueue(task);
}

void TaskQueue::startQueue()
{
    paused = false;
    startNext();
}

void TaskQueue::pauseQueue()
{
    paused = true;

    for (auto *downloader : running)
    {
        if (downloader)
        {
            downloader->cancel();
        }
    }
}

bool TaskQueue::isPaused() const
{
    return paused;
}

void TaskQueue::onTaskFinished(VideoDownloader *downloader, const DownloadTask &task)
{
    running.removeOne(downloader);
    downloader->deleteLater();

    emit taskFinished(task);

    if (!paused)
    {
        startNext();
    }

    if (pending.isEmpty() && running.isEmpty())
    {
        emit allFinished();
    }
}

void TaskQueue::startNext()
{
    while (!paused && !pending.isEmpty() && running.size() < maxConcurrent)
    {
        DownloadTask task = pending.dequeue();
        auto *downloader = new VideoDownloader(this);

        connect(downloader, &VideoDownloader::logMessage, this, &TaskQueue::logMessage);
        connect(downloader, &VideoDownloader::taskFinished, this, &TaskQueue::onTaskFinished);

        running.append(downloader);
        downloader->start(task);
    }
}
