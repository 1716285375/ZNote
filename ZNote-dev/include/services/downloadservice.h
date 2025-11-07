/**
 * @file downloadservice.h
 * @brief Download service implementation
 * 
 * Coordinates URL parsing, task queue management, and download execution.
 */

#ifndef DOWNLOADSERVICE_H
#define DOWNLOADSERVICE_H

#include "core/interfaces/idownloadservice.h"
#include "core/interfaces/ihistoryservice.h"
#include "core/interfaces/iconfigservice.h"
#include "core/download/taskqueue.h"
#include "core/download/urlparser.h"
#include <QObject>
#include <QTimer>
#include <QMutex>
#include <memory>

/**
 * @class DownloadService
 * @brief Main download service coordinating all download operations
 * 
 * Responsibilities:
 * - URL parsing and video information extraction
 * - Task queue management
 * - Download progress tracking
 * - History management
 * - Error handling and recovery
 */
class DownloadService : public IDownloadService
{
    Q_OBJECT

public:
    /**
     * @brief Construct DownloadService
     * @param configService Configuration service instance
     * @param historyService History service instance
     * @param parent Parent QObject
     */
    explicit DownloadService(IConfigService *configService, 
                           IHistoryService *historyService, 
                           QObject *parent = nullptr);
    ~DownloadService() override;

    // IDownloadService interface
    void parseUrl(const QString &url, const QString &savePath) override;
    void addTask(const DownloadTask &task) override;
    void addTasks(const QList<DownloadTask> &tasks) override;
    void removeTask(const QString &taskId) override;
    void clearTasks() override;
    
    void startDownload() override;
    void pauseDownload() override;
    void resumeDownload() override;
    void stopDownload() override;
    
    bool isRunning() const override;
    bool isPaused() const override;
    int getTaskCount() const override;
    int getCompletedCount() const override;
    float getProgress() const override;
    
    QList<DownloadHistoryItem> getHistory() const override;
    void addHistory(const DownloadHistoryItem &item) override;
    void removeHistory(const QList<DownloadHistoryItem> &items) override;

private slots:
    void onTaskFinished(const DownloadTask &task);
    void onAllTasksFinished();
    void onTaskError(const QString &taskId, const QString &error);
    void onEntryParsed(const ParsedEntry &entry);  // 新增：单个条目解析完成（生产者-消费者模式）
    void onUrlParsed(const QList<ParsedEntry> &entries);  // 保留用于兼容性
    void onUrlParseError(const QString &error);
    void updateProgress();

private:
    void setupConnections();
    void updateTaskProgress();
    DownloadTask createDownloadTask(const ParsedEntry &entry, const QString &savePath);
    void logMessage(const QString &message);

    IConfigService *m_configService;
    IHistoryService *m_historyService;
    std::unique_ptr<TaskQueue> m_taskQueue;
    std::unique_ptr<UrlParser> m_urlParser;
    
    QList<DownloadTask> m_pendingTasks;
    QList<DownloadTask> m_completedTasks;
    QList<DownloadTask> m_failedTasks;
    
    mutable QMutex m_mutex;
    QTimer *m_progressTimer;
    
    QString m_currentSavePath;  // 当前解析的保存路径
    
    int m_totalTasks;
    int m_completedCount;
    int m_totalEverAdded;  // 进入过下载列表的总数（用于计算下载进度）
    bool m_isRunning;
    bool m_isPaused;
    
    // 解析统计
    int m_parseTotal;  // 解析总数
    int m_parseSuccess;  // 解析成功数
    int m_parseFailed;  // 解析失败数
};

#endif // DOWNLOADSERVICE_H
