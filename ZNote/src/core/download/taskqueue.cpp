#include "core/download/taskqueue.h"
#include "core/download/videodownloader.h"
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

TaskQueue::TaskQueue(int max, QObject *parent)
    : QObject(parent), maxConcurrent(max), paused(false)
{
    qDebug() << "Max concurrent threads set to:" << maxConcurrent;
}

void TaskQueue::enqueue(const DownloadTask &task)
{
    QMutexLocker locker(&m_mutex);
    pending.enqueue(task);
}

void TaskQueue::startQueue()
{
    {
        QMutexLocker locker(&m_mutex);
        paused = false;
    }
    startNext();
}

void TaskQueue::pauseQueue()
{
    {
        QMutexLocker locker(&m_mutex);
        paused = true;
    }

    // 使用 QMetaObject::invokeMethod 在线程中安全调用 cancel
    QMutexLocker locker(&m_mutex);
    for (auto *downloader : running)
    {
        if (downloader)
        {
            // 使用 QMetaObject::invokeMethod 确保在线程中调用
            QMetaObject::invokeMethod(downloader, "cancel", Qt::QueuedConnection);
        }
    }
}

bool TaskQueue::isPaused() const
{
    QMutexLocker locker(&m_mutex);
    return paused;
}

int TaskQueue::getTaskSize() const
{
    QMutexLocker locker(&m_mutex);
    return pending.size();
}

void TaskQueue::setMaxConcurrent(int max)
{
    QMutexLocker locker(&m_mutex);
    maxConcurrent = max;
    qDebug() << "Max concurrent threads updated to:" << maxConcurrent;
}

void TaskQueue::onTaskFinished(VideoDownloader *downloader, const DownloadTask &task)
{
    // 从运行列表中移除（线程安全）
    {
        QMutexLocker locker(&m_mutex);
        running.removeOne(downloader);
    }
    
    // downloader 会在线程结束时自动删除（通过 connect(thread->finished, downloader->deleteLater)）
    // 不需要在这里手动删除

    // 发射信号（使用 QueuedConnection 确保在主线程中处理）
    emit taskFinished(task);

    // 在锁外调用 startNext，避免死锁
    bool shouldStartNext = false;
    bool shouldEmitAllFinished = false;
    
    {
        QMutexLocker locker(&m_mutex);
        if (!paused)
        {
            shouldStartNext = true;
        }

        if (pending.isEmpty() && running.isEmpty())
        {
            shouldEmitAllFinished = true;
        }
    }
    
    // 使用 QMetaObject::invokeMethod 确保在主线程中执行
    if (shouldStartNext) {
        QMetaObject::invokeMethod(this, "startNext", Qt::QueuedConnection);
    }
    
    if (shouldEmitAllFinished) {
        emit allFinished();  // 信号本身就是线程安全的
    }
}

void TaskQueue::startNext()
{
    // 减少锁的持有时间，避免阻塞
    while (true)
    {
        DownloadTask task;
        bool shouldStart = false;
        int currentRunning = 0;
        
        {
            QMutexLocker locker(&m_mutex);
            if (paused || pending.isEmpty() || running.size() >= maxConcurrent) {
                break;
            }
            
            task = pending.dequeue();
            shouldStart = true;
            currentRunning = running.size();
        }
        
        if (!shouldStart) {
            break;
        }
        
        // 在锁外创建线程和下载器，避免长时间持有锁
        QThread *thread = new QThread(this);
        VideoDownloader *downloader = new VideoDownloader();
        
        // 将下载器移动到工作线程
        downloader->moveToThread(thread);
        
        // 连接信号和槽
        connect(thread, &QThread::started, [downloader, task]() {
            downloader->start(task);
        });
        
        // 使用 QueuedConnection 确保信号在主线程中处理
        connect(downloader, &VideoDownloader::logMessage, this, &TaskQueue::logMessage, Qt::QueuedConnection);
        connect(downloader, &VideoDownloader::taskFinished, this, [this, downloader, thread](VideoDownloader*, const DownloadTask &task) {
            // 先通知任务完成（在主线程中处理）
            onTaskFinished(downloader, task);
            // 然后退出线程（在线程中调用）
            QMetaObject::invokeMethod(thread, "quit", Qt::QueuedConnection);
        }, Qt::QueuedConnection);
        
        // 线程结束时清理（只清理线程，downloader 由线程清理）
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(thread, &QThread::finished, downloader, &VideoDownloader::deleteLater);
        
        // 在锁外启动线程
        thread->start();
        
        // 快速添加 running 列表
        {
            QMutexLocker locker(&m_mutex);
            running.append(downloader);
        }
    }
}
