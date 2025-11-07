#include "services/historyservice.h"
#include "utils/logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QCoreApplication>

HistoryService::HistoryService(const QString &historyPath, QObject *parent)
    : IHistoryService(parent)
{
    if (historyPath.isEmpty()) {
        m_historyPath = getDefaultHistoryPath();
    } else {
        m_historyPath = historyPath;
    }
    
    LOG_INFO(QString("HistoryService initialized, history file path: %1").arg(m_historyPath));
    loadHistory();
}

HistoryService::~HistoryService()
{
    // 析构时同步保存（避免数据丢失）
    saveHistory();
}

QList<DownloadHistoryItem> HistoryService::getHistory() const
{
    QMutexLocker locker(&m_mutex);
    return m_historyItems;
}

void HistoryService::addHistory(const DownloadHistoryItem &item)
{
    {
        QMutexLocker locker(&m_mutex);
        
        // 检查是否已存在相同的记录
        auto it = std::find_if(m_historyItems.begin(), m_historyItems.end(),
                              [&item](const DownloadHistoryItem &existing) {
                                  return existing.vid == item.vid && 
                                         existing.index == item.index &&
                                         existing.savePath == item.savePath;
                              });
        
        if (it != m_historyItems.end()) {
            // 更新现有记录
            *it = item;
            LOG_DEBUG(QString("Updated history item: %1").arg(item.vid));
        } else {
            // 添加新记录
            m_historyItems.append(item);
            LOG_DEBUG(QString("Added history item: %1").arg(item.vid));
        }
    }
    
    // 同步保存，确保数据不丢失（即使程序异常退出，析构函数也会保存）
    // 使用直接调用而不是异步调用，确保数据及时保存
    saveHistory();
}

void HistoryService::removeHistory(const DownloadHistoryItem &item)
{
    bool removed = false;
    {
        QMutexLocker locker(&m_mutex);
        
        auto it = std::find_if(m_historyItems.begin(), m_historyItems.end(),
                              [&item](const DownloadHistoryItem &existing) {
                                  return existing.vid == item.vid && 
                                         existing.index == item.index &&
                                         existing.savePath == item.savePath;
                              });
        
        if (it != m_historyItems.end()) {
            m_historyItems.erase(it);
            removed = true;
        }
    }
    
    if (removed) {
        // 同步保存，确保数据不丢失
        saveHistory();
        LOG_DEBUG(QString("Removed history item: %1").arg(item.vid));
        emit historyRemoved(item);
    }
}

void HistoryService::removeHistory(const QList<DownloadHistoryItem> &items)
{
    {
        QMutexLocker locker(&m_mutex);
        
        for (const auto &item : items) {
            auto it = std::find_if(m_historyItems.begin(), m_historyItems.end(),
                                  [&item](const DownloadHistoryItem &existing) {
                                      return existing.vid == item.vid && 
                                             existing.index == item.index &&
                                             existing.savePath == item.savePath;
                                  });
            
            if (it != m_historyItems.end()) {
                m_historyItems.erase(it);
                emit historyRemoved(item);
            }
        }
    }
    
    // 同步保存，确保数据不丢失
    saveHistory();
    LOG_INFO(QString("Removed %1 history items").arg(items.size()));
}

void HistoryService::clearHistory()
{
    int count = 0;
    {
        QMutexLocker locker(&m_mutex);
        count = m_historyItems.size();
        m_historyItems.clear();
    }
    
    // 同步保存，确保数据不丢失
    saveHistory();
    
    LOG_INFO(QString("Cleared %1 history items").arg(count));
    emit historyCleared();
}

void HistoryService::loadHistory()
{
    QMutexLocker locker(&m_mutex);
    
    // 确保目录存在
    QDir().mkpath(QFileInfo(m_historyPath).absolutePath());
    
    QFile file(m_historyPath);
    if (!file.exists()) {
        // 文件不存在，创建空的历史记录
        m_historyItems.clear();
        LOG_INFO(QString("History file does not exist, starting with empty history: %1").arg(m_historyPath));
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        LOG_WARNING(QString("Failed to open history file: %1, starting with empty history").arg(m_historyPath));
        m_historyItems.clear();
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    if (data.isEmpty()) {
        // 文件为空，创建空的历史记录
        m_historyItems.clear();
        LOG_INFO("History file is empty, starting with empty history");
        return;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        LOG_ERROR(QString("Failed to parse history file: %1, starting with empty history").arg(error.errorString()));
        m_historyItems.clear();
        return;
    }
    
    if (!doc.isArray()) {
        LOG_ERROR("History file format is invalid, starting with empty history");
        m_historyItems.clear();
        return;
    }
    
    QJsonArray array = doc.array();
    m_historyItems.clear();
    
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            DownloadHistoryItem item = jsonToHistoryItem(value.toObject());
            if (!item.vid.isEmpty()) {
                m_historyItems.append(item);
            }
        }
    }
    
    LOG_INFO(QString("Loaded %1 history items from %2").arg(m_historyItems.size()).arg(m_historyPath));
}

void HistoryService::saveHistory()
{
    // 复制数据，减少锁的持有时间
    QList<DownloadHistoryItem> itemsToSave;
    {
        QMutexLocker locker(&m_mutex);
        itemsToSave = m_historyItems;
    }
    
    // 在锁外执行文件I/O操作，避免阻塞
    // 确保目录存在
    QString dirPath = QFileInfo(m_historyPath).absolutePath();
    QDir().mkpath(dirPath);
    LOG_DEBUG(QString("Ensuring history directory exists: %1").arg(dirPath));
    
    QFile file(m_historyPath);
    if (!file.open(QIODevice::WriteOnly)) {
        LOG_ERROR(QString("Failed to open history file for writing: %1").arg(m_historyPath));
        LOG_ERROR(QString("Directory exists: %1, Writable: %2").arg(QDir(dirPath).exists()).arg(QFileInfo(dirPath).isWritable()));
        return;
    }
    
    QJsonArray array;
    for (const auto &item : itemsToSave) {
        array.append(historyItemToJson(item));
    }
    
    QJsonDocument doc(array);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
    
    // 写入文件并确保刷新到磁盘
    qint64 bytesWritten = file.write(jsonData);
    file.flush();  // 刷新缓冲区
    file.close();  // 关闭文件，确保数据写入磁盘
    
    if (bytesWritten == -1) {
        LOG_ERROR(QString("Failed to write history file: %1").arg(m_historyPath));
    } else {
        // 验证文件是否真的存在
        if (QFile::exists(m_historyPath)) {
            LOG_INFO(QString("Saved %1 history items to %2 (%3 bytes)").arg(itemsToSave.size()).arg(m_historyPath).arg(bytesWritten));
        } else {
            LOG_ERROR(QString("History file was written but does not exist: %1").arg(m_historyPath));
        }
    }
}

QString HistoryService::getDefaultHistoryPath() const
{
    // 优先使用应用程序目录，与 config.json 保持一致
    QString appDir = QCoreApplication::applicationDirPath();
    QString appHistoryPath = QDir(appDir).filePath("download_history.json");
    
    // 如果应用程序目录可写，使用它
    QDir appDirObj(appDir);
    if (appDirObj.exists() && QFileInfo(appDir).isWritable()) {
        LOG_INFO(QString("Using history file in application directory: %1").arg(appHistoryPath));
        return appHistoryPath;
    }
    
    // 否则使用 AppDataLocation
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    QString dataHistoryPath = QDir(appDataPath).filePath("download_history.json");
    LOG_INFO(QString("Using history file in AppDataLocation: %1").arg(dataHistoryPath));
    return dataHistoryPath;
}

DownloadHistoryItem HistoryService::jsonToHistoryItem(const QJsonObject &json) const
{
    DownloadHistoryItem item;
    
    item.vid = json["vid"].toString();
    item.title = json["title"].toString();
    item.index = json["index"].toInt(1);
    item.playlistCount = json["playlistCount"].toInt(1);
    item.type = static_cast<UrlType>(json["type"].toInt(0));
    item.savePath = json["savePath"].toString();
    item.startTime = QDateTime::fromString(json["startTime"].toString(), Qt::ISODate);
    item.endTime = QDateTime::fromString(json["endTime"].toString(), Qt::ISODate);
    item.status = static_cast<DownloadStatus>(json["status"].toInt(0));
    
    return item;
}

QJsonObject HistoryService::historyItemToJson(const DownloadHistoryItem &item) const
{
    QJsonObject json;
    
    json["vid"] = item.vid;
    json["title"] = item.title;
    json["index"] = item.index;
    json["playlistCount"] = item.playlistCount;
    json["type"] = static_cast<int>(item.type);
    json["savePath"] = item.savePath;
    json["startTime"] = item.startTime.toString(Qt::ISODate);
    json["endTime"] = item.endTime.toString(Qt::ISODate);
    json["status"] = static_cast<int>(item.status);
    
    return json;
}

QList<DownloadHistoryItem> HistoryService::getHistoryByStatus(DownloadStatus status) const
{
    QMutexLocker locker(&m_mutex);
    
    QList<DownloadHistoryItem> result;
    for (const auto &item : m_historyItems) {
        if (item.status == status) {
            result.append(item);
        }
    }
    
    return result;
}

QList<DownloadHistoryItem> HistoryService::getHistoryByDateRange(const QDateTime &start, const QDateTime &end) const
{
    QMutexLocker locker(&m_mutex);
    
    QList<DownloadHistoryItem> result;
    for (const auto &item : m_historyItems) {
        if (item.startTime >= start && item.startTime <= end) {
            result.append(item);
        }
    }
    
    return result;
}

void HistoryService::addHistory(const QList<DownloadHistoryItem> &items)
{
    {
        QMutexLocker locker(&m_mutex);
        
        for (const auto &item : items) {
            // 检查是否已存在相同的记录
            auto it = std::find_if(m_historyItems.begin(), m_historyItems.end(),
                                  [&item](const DownloadHistoryItem &existing) {
                                      return existing.vid == item.vid && 
                                             existing.index == item.index &&
                                             existing.savePath == item.savePath;
                                  });
            
            if (it != m_historyItems.end()) {
                // 更新现有记录
                *it = item;
                LOG_DEBUG(QString("Updated history item: %1").arg(item.vid));
            } else {
                // 添加新记录
                m_historyItems.append(item);
                LOG_DEBUG(QString("Added history item: %1").arg(item.vid));
            }
            
            emit historyAdded(item);
        }
    }
    
    // 同步保存，确保数据不丢失
    saveHistory();
}

DownloadHistoryItem HistoryService::findHistoryById(const QString &id) const
{
    QMutexLocker locker(&m_mutex);
    
    for (const auto &item : m_historyItems) {
        if (item.vid == id) {
            return item;
        }
    }
    
    return DownloadHistoryItem(); // 返回空对象
}

QList<DownloadHistoryItem> HistoryService::searchHistory(const QString &keyword) const
{
    QMutexLocker locker(&m_mutex);
    
    QList<DownloadHistoryItem> result;
    QString lowerKeyword = keyword.toLower();
    
    for (const auto &item : m_historyItems) {
        if (item.title.toLower().contains(lowerKeyword) ||
            item.vid.contains(keyword)) {
            result.append(item);
        }
    }
    
    return result;
}

int HistoryService::getHistoryCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_historyItems.size();
}

int HistoryService::getSuccessCount() const
{
    QMutexLocker locker(&m_mutex);
    
    int count = 0;
    for (const auto &item : m_historyItems) {
        if (item.status == DownloadStatus::Success) {
            count++;
        }
    }
    
    return count;
}

int HistoryService::getFailedCount() const
{
    QMutexLocker locker(&m_mutex);
    
    int count = 0;
    for (const auto &item : m_historyItems) {
        if (item.status == DownloadStatus::Failed) {
            count++;
        }
    }
    
    return count;
}

void HistoryService::forceSave()
{
    // 强制同步保存，确保数据写入磁盘
    saveHistory();
    LOG_INFO("History force saved");
}
