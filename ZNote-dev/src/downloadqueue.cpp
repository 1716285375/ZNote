#include "downloadqueue.h"



DownloadQueue::DownloadQueue(QObject *parent)
{

}

void DownloadQueue::addTask(const QString &url)
{
    tasks.enqueue(url);
}

bool DownloadQueue::hasNext() const
{
    return !tasks.isEmpty();
}

QString DownloadQueue::takeNext()
{
    if (tasks.isEmpty()) return {};
    return tasks.dequeue();
}

void DownloadQueue::clear()
{
    tasks.clear();
}

