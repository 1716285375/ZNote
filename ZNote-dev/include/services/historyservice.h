/**
 * @file historyservice.h
 * @brief History service implementation
 * 
 * Manages download history with JSON-based persistent storage.
 */

#ifndef HISTORYSERVICE_H
#define HISTORYSERVICE_H

#include "core/interfaces/ihistoryservice.h"
#include "core/download/task.h"
#include <QObject>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMutex>
#include <QStandardPaths>
#include <QDir>

/**
 * @class HistoryService
 * @brief Thread-safe download history service with JSON storage
 * 
 * Features:
 * - Thread-safe operations
 * - Automatic persistence
 * - Search and filter capabilities
 * - Statistics tracking
 * - Graceful error handling
 */
class HistoryService : public IHistoryService
{
    Q_OBJECT

public:
    /**
     * @brief Construct a HistoryService
     * @param historyPath Path to history file (empty = use default location)
     * @param parent Parent QObject
     */
    explicit HistoryService(const QString &historyPath = QString(), QObject *parent = nullptr);
    ~HistoryService() override;

    // IHistoryService interface
    /**
     * @brief Get all history items
     * @return List of all history items
     */
    QList<DownloadHistoryItem> getHistory() const override;
    
    /**
     * @brief Get history items filtered by status
     * @param status Download status to filter by
     * @return Filtered history items
     */
    QList<DownloadHistoryItem> getHistoryByStatus(DownloadStatus status) const override;
    
    /**
     * @brief Get history items within date range
     * @param start Start date
     * @param end End date
     * @return Filtered history items
     */
    QList<DownloadHistoryItem> getHistoryByDateRange(const QDateTime &start, const QDateTime &end) const override;
    
    /**
     * @brief Add a history item
     * @param item History item to add
     */
    void addHistory(const DownloadHistoryItem &item) override;
    
    /**
     * @brief Add multiple history items
     * @param items List of history items to add
     */
    void addHistory(const QList<DownloadHistoryItem> &items) override;
    
    /**
     * @brief Remove a history item
     * @param item History item to remove
     */
    void removeHistory(const DownloadHistoryItem &item) override;
    
    /**
     * @brief Remove multiple history items
     * @param items List of history items to remove
     */
    void removeHistory(const QList<DownloadHistoryItem> &items) override;
    
    /**
     * @brief Clear all history
     */
    void clearHistory() override;
    
    /**
     * @brief Find history item by ID
     * @param id History item ID
     * @return History item or empty item if not found
     */
    DownloadHistoryItem findHistoryById(const QString &id) const override;
    
    /**
     * @brief Search history by keyword
     * @param keyword Search keyword
     * @return Matching history items
     */
    QList<DownloadHistoryItem> searchHistory(const QString &keyword) const override;
    
    /**
     * @brief Get total history count
     * @return Number of history items
     */
    int getHistoryCount() const override;
    
    /**
     * @brief Get count of successful downloads
     * @return Number of successful downloads
     */
    int getSuccessCount() const override;
    
    /**
     * @brief Get count of failed downloads
     * @return Number of failed downloads
     */
    int getFailedCount() const override;
    
    /**
     * @brief Force save history to disk (synchronous)
     * 
     * Use this method when you need to ensure history is saved immediately,
     * such as during application shutdown.
     */
    void forceSave();

private slots:
    void saveHistory();  // 改为槽函数，可以异步调用

private:
    void loadHistory();
    QString getDefaultHistoryPath() const;
    DownloadHistoryItem jsonToHistoryItem(const QJsonObject &json) const;
    QJsonObject historyItemToJson(const DownloadHistoryItem &item) const;

    QString m_historyPath;
    QList<DownloadHistoryItem> m_historyItems;
    mutable QMutex m_mutex;
};

#endif // HISTORYSERVICE_H
