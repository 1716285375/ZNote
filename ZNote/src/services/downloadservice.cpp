#include "services/downloadservice.h"
#include "core/download/taskqueue.h"
#include "core/download/urlparser.h"
#include "utils/logger.h"
#include <QTimer>
#include <QDebug>
#include <algorithm>

DownloadService::DownloadService(IConfigService *configService, 
                               IHistoryService *historyService, 
                               QObject *parent)
    : IDownloadService(parent)
    , m_configService(configService)
    , m_historyService(historyService)
    , m_taskQueue(nullptr)
    , m_urlParser(nullptr)
    , m_progressTimer(new QTimer(this))
    , m_totalTasks(0)
    , m_completedCount(0)
    , m_totalEverAdded(0)
    , m_isRunning(false)
    , m_isPaused(false)
    , m_parseTotal(0)
    , m_parseSuccess(0)
    , m_parseFailed(0)
{
    if (!m_configService) {
        LOG_ERROR("ConfigService is null");
        return;
    }
    
    // 创建任务队列
    int threadCount = m_configService->getValue("download.threadCount", 4).toInt();
    m_taskQueue = std::make_unique<TaskQueue>(threadCount, this);
    
    // 创建URL解析器
    m_urlParser = std::make_unique<UrlParser>(this);
    
    setupConnections();
    
    // 设置进度更新定时器
    m_progressTimer->setInterval(1000); // 每秒更新一次
    connect(m_progressTimer, &QTimer::timeout, this, &DownloadService::updateProgress);
}

DownloadService::~DownloadService()
{
    if (m_isRunning) {
        stopDownload();
    }
}

void DownloadService::parseUrl(const QString &url, const QString &savePath)
{
    if (url.isEmpty()) {
        emit taskError("", "URL is empty");
        return;
    }
    
    if (savePath.isEmpty()) {
        emit taskError("", "Save path is empty");
        return;
    }
    
    // 保存当前解析的保存路径
    m_currentSavePath = savePath;
    
    // 重置解析统计
    {
        QMutexLocker locker(&m_mutex);
        m_parseTotal = 0;
        m_parseSuccess = 0;
        m_parseFailed = 0;
    }
    
    LOG_INFO(QString("Parsing URL: %1").arg(url));
    emit logMessage(QString("⏳ 正在解析URL，请稍候..."));
    m_urlParser->parse(url);
}

void DownloadService::addTask(const DownloadTask &task)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_taskQueue) {
        m_taskQueue->enqueue(task);
        m_pendingTasks.append(task);
        m_totalTasks++;
        m_totalEverAdded++;  // 增加进入过下载列表的总数
        
        LOG_INFO(QString("Task added: %1").arg(task.id));
        emit taskReady(task);
    }
}

void DownloadService::addTasks(const QList<DownloadTask> &tasks)
{
    QMutexLocker locker(&m_mutex);
    
    for (const auto &task : tasks) {
        if (m_taskQueue) {
            m_taskQueue->enqueue(task);
            m_pendingTasks.append(task);
            m_totalTasks++;
            m_totalEverAdded++;  // 增加进入过下载列表的总数
        }
    }
    
    LOG_INFO(QString("Added %1 tasks").arg(tasks.size()));
}

void DownloadService::removeTask(const QString &taskId)
{
    QMutexLocker locker(&m_mutex);
    
    // 从待处理任务中移除
    auto it = std::find_if(m_pendingTasks.begin(), m_pendingTasks.end(),
                          [&taskId](const DownloadTask &task) {
                              return task.id == taskId;
                          });
    
    if (it != m_pendingTasks.end()) {
        m_pendingTasks.erase(it);
        m_totalTasks--;
        LOG_INFO(QString("Task removed: %1").arg(taskId));
    }
}

void DownloadService::clearTasks()
{
    QMutexLocker locker(&m_mutex);
    
    m_pendingTasks.clear();
    m_completedTasks.clear();
    m_failedTasks.clear();
    m_totalTasks = 0;
    m_completedCount = 0;
    
    if (m_taskQueue) {
        // Note: TaskQueue doesn't have a clear method yet
        // Tasks will be cleared when queue is stopped
    }
    
    LOG_INFO("All tasks cleared");
}

void DownloadService::startDownload()
{
    // 更新线程数（从设置中读取）
    if (m_configService && m_taskQueue) {
        int threadCount = m_configService->getValue("download.threadCount", 4).toInt();
        m_taskQueue->setMaxConcurrent(threadCount);
        LOG_INFO(QString("Thread count updated to: %1").arg(threadCount));
    }
    
    // 减少锁的持有时间
    bool shouldStart = false;
    bool hasTasks = false;
    
    {
        QMutexLocker locker(&m_mutex);
        
        if (m_isRunning) {
            LOG_WARNING("Download is already running");
            return;
        }
        
        hasTasks = !m_pendingTasks.isEmpty();
        if (!hasTasks) {
            LOG_WARNING("No tasks to download");
            return;
        }
        
        m_isRunning = true;
        m_isPaused = false;
        shouldStart = true;
    }
    
    // 在锁外执行耗时操作
    if (shouldStart && hasTasks) {
        if (m_taskQueue) {
            m_taskQueue->startQueue();
        }
        
        m_progressTimer->start();
        
        LOG_INFO("Download started");
    }
}

void DownloadService::pauseDownload()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_isRunning || m_isPaused) {
        return;
    }
    
    m_isPaused = true;
    
    if (m_taskQueue) {
        m_taskQueue->pauseQueue();
    }
    
    m_progressTimer->stop();
    
    LOG_INFO("Download paused");
}

void DownloadService::resumeDownload()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_isRunning || !m_isPaused) {
        return;
    }
    
    m_isPaused = false;
    
    if (m_taskQueue) {
        m_taskQueue->startQueue();
    }
    
    m_progressTimer->start();
    
    LOG_INFO("Download resumed");
}

void DownloadService::stopDownload()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_isRunning) {
        return;
    }
    
    m_isRunning = false;
    m_isPaused = false;
    
    if (m_taskQueue) {
        m_taskQueue->pauseQueue();
    }
    
    m_progressTimer->stop();
    
    LOG_INFO("Download stopped");
}

bool DownloadService::isRunning() const
{
    QMutexLocker locker(&m_mutex);
    return m_isRunning;
}

bool DownloadService::isPaused() const
{
    QMutexLocker locker(&m_mutex);
    return m_isPaused;
}

int DownloadService::getTaskCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_totalTasks;
}

int DownloadService::getCompletedCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_completedCount;
}

float DownloadService::getProgress() const
{
    QMutexLocker locker(&m_mutex);
    
    // 下载进度 = 已完成数 / (已完成数 + 待处理数)
    // 这样更准确地反映当前下载队列的进度
    int totalTasks = m_completedCount + m_pendingTasks.size();
    
    if (totalTasks == 0) {
        return 0.0f;
    }
    
    return static_cast<float>(m_completedCount) / static_cast<float>(totalTasks);
}

QList<DownloadHistoryItem> DownloadService::getHistory() const
{
    if (m_historyService) {
        return m_historyService->getHistory();
    }
    return QList<DownloadHistoryItem>();
}

void DownloadService::addHistory(const DownloadHistoryItem &item)
{
    if (m_historyService) {
        m_historyService->addHistory(item);
    }
}

void DownloadService::removeHistory(const QList<DownloadHistoryItem> &items)
{
    if (m_historyService) {
        m_historyService->removeHistory(items);
    }
}

void DownloadService::setupConnections()
{
    // 连接任务队列信号（使用 QueuedConnection 确保线程安全）
    if (m_taskQueue) {
        connect(m_taskQueue.get(), &TaskQueue::taskFinished,
                this, &DownloadService::onTaskFinished, Qt::QueuedConnection);
        connect(m_taskQueue.get(), &TaskQueue::allFinished,
                this, &DownloadService::onAllTasksFinished, Qt::QueuedConnection);
        connect(m_taskQueue.get(), &TaskQueue::logMessage,
                this, &DownloadService::logMessage, Qt::QueuedConnection);
    }
    
    // 连接URL解析器信号
    if (m_urlParser) {
        // 连接单个条目解析信号（生产者-消费者模式：解析一个就发送一个）
        connect(m_urlParser.get(), &UrlParser::entryParsed,
                this, &DownloadService::onEntryParsed);
        // 保留批量信号用于兼容性
        connect(m_urlParser.get(), &UrlParser::urlParsed,
                this, &DownloadService::onUrlParsed);
        connect(m_urlParser.get(), &UrlParser::errorOccurred,
                this, &DownloadService::onUrlParseError);
        connect(m_urlParser.get(), &UrlParser::logMessage,
                this, &DownloadService::logMessage);
    }
}

void DownloadService::updateTaskProgress()
{
    emit taskProgress("", getProgress());
}

DownloadTask DownloadService::createDownloadTask(const ParsedEntry &entry, const QString &savePath)
{
    DownloadTask task;
    task.id = entry.id;
    task.index = entry.index;
    task.playlistCount = entry.playlistCount;
    task.type = entry.type;
    task.video.title = entry.title;
    task.video.url = entry.url;
    task.video.formatId = entry.formatId;  // 传递格式ID
    task.video.ext = entry.ext;  // 传递扩展名
    task.savePath = savePath;
    task.resolveTime = QDateTime::currentDateTime();
    
    addTask(task);
    return task;
}

void DownloadService::logMessage(const QString &message)
{
    LOG_INFO(QString("DownloadService: %1").arg(message));
    emit IDownloadService::logMessage(message);
}

void DownloadService::onTaskFinished(const DownloadTask &task)
{
    // 减少锁的持有时间
    {
        QMutexLocker locker(&m_mutex);
        
        m_completedCount++;
        m_completedTasks.append(task);
        
        // 从待处理任务中移除
        auto it = std::find_if(m_pendingTasks.begin(), m_pendingTasks.end(),
                              [&task](const DownloadTask &t) {
                                  return t.id == task.id;
                              });
        
        if (it != m_pendingTasks.end()) {
            m_pendingTasks.erase(it);
        }
    }
    
    // 在锁外执行耗时操作
    // 添加到历史记录（异步保存，不阻塞）
    DownloadHistoryItem historyItem;
    historyItem.vid = task.id;
    historyItem.title = task.video.title;
    historyItem.index = task.index;
    historyItem.playlistCount = task.playlistCount;
    historyItem.type = task.type;
    historyItem.savePath = task.savePath;
    historyItem.startTime = task.startTime;
    historyItem.endTime = task.endTime;
    historyItem.status = DownloadStatus::Success;
    
    addHistory(historyItem);
    
    emit taskFinished(task);
    updateTaskProgress();
    
    LOG_INFO(QString("Task completed: %1").arg(task.id));
}

void DownloadService::onAllTasksFinished()
{
    QMutexLocker locker(&m_mutex);
    
    m_isRunning = false;
    m_isPaused = false;
    m_progressTimer->stop();
    
    emit allTasksFinished();
    LOG_INFO("All tasks finished");
}

void DownloadService::onTaskError(const QString &taskId, const QString &error)
{
    emit taskError(taskId, error);
    LOG_ERROR(QString("Task error [%1]: %2").arg(taskId).arg(error));
}

void DownloadService::onEntryParsed(const ParsedEntry &entry)
{
    QString savePath = m_currentSavePath.isEmpty() ? 
        m_configService->getValue("download.defaultPath").toString() : 
        m_currentSavePath;
    
    // 更新解析统计
    {
        QMutexLocker locker(&m_mutex);
        m_parseTotal++;
        m_parseSuccess++;
    }
    
    // 创建任务对象并立即发送（生产者-消费者模式：解析一个就显示一个）
    DownloadTask task;
    task.id = entry.id;
    task.index = entry.index;
    task.playlistCount = entry.playlistCount;
    task.type = entry.type;
    task.video.title = entry.title;
    task.video.url = entry.url;
    task.video.formatId = entry.formatId;
    task.video.ext = entry.ext;
    task.savePath = savePath;
    task.resolveTime = QDateTime::currentDateTime();
    
    // 立即发送 taskReady 信号，UI会立即显示
    emit taskReady(task);
    
    // 发送解析统计更新信号
    emit parseStatsUpdated(m_parseTotal, m_parseSuccess, m_parseFailed);
}

void DownloadService::onUrlParsed(const QList<ParsedEntry> &entries)
{
    // 保留用于兼容性，但主要使用 onEntryParsed
    // 如果某些情况下仍然使用批量信号，这里也会处理
    QString savePath = m_currentSavePath.isEmpty() ? 
        m_configService->getValue("download.defaultPath").toString() : 
        m_currentSavePath;
    
    for (const auto &entry : entries) {
        DownloadTask task;
        task.id = entry.id;
        task.index = entry.index;
        task.playlistCount = entry.playlistCount;
        task.type = entry.type;
        task.video.title = entry.title;
        task.video.url = entry.url;
        task.video.formatId = entry.formatId;
        task.video.ext = entry.ext;
        task.savePath = savePath;
        task.resolveTime = QDateTime::currentDateTime();
        
        emit taskReady(task);
    }
    
    LOG_INFO(QString("URL parsed successfully: %1 entries").arg(entries.size()));
    emit logMessage(QString("✅ 解析完成，共找到 %1 个视频").arg(entries.size()));
}

void DownloadService::onUrlParseError(const QString &error)
{
    // 更新解析统计
    {
        QMutexLocker locker(&m_mutex);
        m_parseTotal++;
        m_parseFailed++;
    }
    
    emit taskError("", error);
    LOG_ERROR(QString("URL parse error: %1").arg(error));
    
    // 发送解析统计更新信号
    emit parseStatsUpdated(m_parseTotal, m_parseSuccess, m_parseFailed);
}

void DownloadService::updateProgress()
{
    updateTaskProgress();
}
