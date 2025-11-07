/**
 * @file taskqueue.h
 * @brief Download task queue management
 * 
 * Manages concurrent download tasks with configurable thread limits.
 */

#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <QObject>
#include <QQueue>
#include <QList>
#include <QMutex>

class VideoDownloader;
struct DownloadTask;

/**
 * @class TaskQueue
 * @brief Thread-safe task queue for managing concurrent downloads
 * 
 * Features:
 * - Configurable maximum concurrent downloads
 * - Thread-safe operations
 * - Pause/resume support
 * - Automatic task scheduling
 */
class TaskQueue : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct TaskQueue
     * @param max Maximum concurrent downloads (default: 2)
     * @param parent Parent QObject
     */
    explicit TaskQueue(int max = 2, QObject *parent = nullptr);

    /**
     * @brief Add a task to the queue
     * @param task Download task to enqueue
     */
    void enqueue(const DownloadTask &task);

    /**
     * @brief Start processing the queue
     */
    void startQueue();

    /**
     * @brief Pause queue processing
     */
    void pauseQueue();

    /**
     * @brief Check if queue is paused
     * @return true if paused
     */
    bool isPaused() const;

    /**
     * @brief Get number of pending tasks
     * @return Number of tasks in queue
     */
    int getTaskSize() const;
    
    /**
     * @brief Set maximum concurrent downloads
     * @param max Maximum concurrent count
     */
    void setMaxConcurrent(int max);

signals:
    /**
     * @brief Emitted when a log message is generated
     * @param msg Log message
     */
    void logMessage(const QString &msg);
    
    /**
     * @brief Emitted when a task finishes
     * @param task Completed task
     */
    void taskFinished(const DownloadTask &task);
    
    /**
     * @brief Emitted when all tasks are finished
     */
    void allFinished();

private slots:
    /**
     * @brief Handle task completion
     * @param downloader Completed downloader instance
     * @param task Completed task
     */
    void onTaskFinished(VideoDownloader *downloader, const DownloadTask &task);
    
    /**
     * @brief Start next task in queue
     */
    void startNext();

private:
    QQueue<DownloadTask> pending;      ///< Pending tasks queue
    QList<VideoDownloader*> running;   ///< Currently running downloaders
    int maxConcurrent;                  ///< Maximum concurrent downloads
    bool paused;                        ///< Pause state flag
    mutable QMutex m_mutex;             ///< Mutex for thread safety
};

#endif // TASKQUEUE_H
