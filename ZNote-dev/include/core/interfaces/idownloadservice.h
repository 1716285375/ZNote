/**
 * @file idownloadservice.h
 * @brief Interface for download service
 * 
 * Defines the interface for video download functionality.
 */

#ifndef IDOWNLOADSERVICE_H
#define IDOWNLOADSERVICE_H

#include "core/download/task.h"
#include <QObject>
#include <QList>

/**
 * @class IDownloadService
 * @brief Interface for managing video downloads
 * 
 * This interface provides methods for:
 * - Parsing video URLs
 * - Managing download tasks
 * - Controlling download process
 * - Querying download status
 * - Managing download history
 */
class IDownloadService : public QObject
{
    Q_OBJECT

public:
    explicit IDownloadService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IDownloadService() = default;

    /**
     * @brief Parse a video URL and extract video information
     * @param url The video URL to parse
     * @param savePath The directory to save downloaded videos
     */
    virtual void parseUrl(const QString &url, const QString &savePath) = 0;
    
    /**
     * @brief Add a single download task
     * @param task The download task to add
     */
    virtual void addTask(const DownloadTask &task) = 0;
    
    /**
     * @brief Add multiple download tasks
     * @param tasks List of download tasks to add
     */
    virtual void addTasks(const QList<DownloadTask> &tasks) = 0;
    
    /**
     * @brief Remove a download task
     * @param taskId The ID of the task to remove
     */
    virtual void removeTask(const QString &taskId) = 0;
    
    /**
     * @brief Clear all download tasks
     */
    virtual void clearTasks() = 0;
    
    /**
     * @brief Start downloading all queued tasks
     */
    virtual void startDownload() = 0;
    
    /**
     * @brief Pause the current download
     */
    virtual void pauseDownload() = 0;
    
    /**
     * @brief Resume a paused download
     */
    virtual void resumeDownload() = 0;
    
    /**
     * @brief Stop the current download
     */
    virtual void stopDownload() = 0;
    
    /**
     * @brief Check if download is currently running
     * @return true if downloading, false otherwise
     */
    virtual bool isRunning() const = 0;
    
    /**
     * @brief Check if download is paused
     * @return true if paused, false otherwise
     */
    virtual bool isPaused() const = 0;
    
    /**
     * @brief Get the total number of tasks
     * @return Number of tasks
     */
    virtual int getTaskCount() const = 0;
    
    /**
     * @brief Get the number of completed tasks
     * @return Number of completed tasks
     */
    virtual int getCompletedCount() const = 0;
    
    /**
     * @brief Get overall download progress (0.0 to 1.0)
     * @return Progress value between 0.0 and 1.0
     */
    virtual float getProgress() const = 0;
    
    /**
     * @brief Get download history
     * @return List of download history items
     */
    virtual QList<DownloadHistoryItem> getHistory() const = 0;
    
    /**
     * @brief Add an item to download history
     * @param item The history item to add
     */
    virtual void addHistory(const DownloadHistoryItem &item) = 0;
    
    /**
     * @brief Remove items from download history
     * @param items List of history items to remove
     */
    virtual void removeHistory(const QList<DownloadHistoryItem> &items) = 0;

signals:
    void taskReady(const DownloadTask &task);
    void taskStarted(const DownloadTask &task);
    void taskProgress(const QString &taskId, float progress);
    void taskFinished(const DownloadTask &task);
    void taskError(const QString &taskId, const QString &error);
    void allTasksFinished();
    void logMessage(const QString &message);
    void parseStatsUpdated(int total, int success, int failed);  // 解析统计更新
};

#endif // IDOWNLOADSERVICE_H
