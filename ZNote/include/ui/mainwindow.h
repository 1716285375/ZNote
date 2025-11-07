/**
 * @file mainwindow.h
 * @brief Main application window
 * 
 * The main UI window that coordinates all user interactions and displays.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core/interfaces/idownloadservice.h"
#include "core/interfaces/iconfigservice.h"
#include "core/interfaces/ihistoryservice.h"
#include "core/download/task.h"
#include "component/videomodel.h"
#include "component/historymodel.h"

#include <QMainWindow>
#include <QButtonGroup>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QSoundEffect;
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Main application window class
 * 
 * This class manages:
 * - UI initialization and layout
 * - Service connections (Download, Config, History)
 * - User interactions and event handling
 * - Status bar updates
 * - Settings management
 * - Sound notifications
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Construct MainWindow
     * @param downloadService Download service instance
     * @param configService Configuration service instance
     * @param historyService History service instance
     * @param parent Parent widget
     */
    explicit MainWindow(IDownloadService *downloadService = nullptr,
                       IConfigService *configService = nullptr,
                       IHistoryService *historyService = nullptr,
                       QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // Download service signal handlers
    /**
     * @brief Handle task ready signal
     * @param task Ready download task
     */
    void onTaskReady(const DownloadTask &task);
    
    /**
     * @brief Handle download progress update
     * @param taskId Task ID
     * @param progress Progress value (0.0-1.0)
     */
    void onDownloadProgress(const QString &taskId, float progress);
    
    /**
     * @brief Handle task finished signal
     * @param task Completed task
     */
    void onTaskFinished(const DownloadTask &task);
    
    /**
     * @brief Handle all tasks finished signal
     */
    void onAllTasksFinished();
    
    /**
     * @brief Handle log message
     * @param message Log message
     */
    void onLogMessage(const QString &message);
    
    /**
     * @brief Handle task error
     * @param taskId Task ID
     * @param error Error message
     */
    void onTaskError(const QString &taskId, const QString &error);
    
    /**
     * @brief Handle parse statistics update
     * @param total Total parsed count
     * @param success Successfully parsed count
     * @param failed Failed parse count
     */
    void onParseStatsUpdated(int total, int success, int failed);
    
    // UI button handlers
    void on_btnResolve_clicked();          ///< Parse URL button
    void on_btnDownloadList_clicked();     ///< Show download list
    void on_btnDownloadStatus_clicked();   ///< Show download status
    void on_btnSetting_clicked();          ///< Show settings
    void on_btnAbout_clicked();            ///< Show about dialog
    void on_btnCrap_clicked();             ///< Clear/reset action
    void on_btnDownload_clicked();         ///< Start download
    void on_btnPause_clicked();            ///< Pause download
    void on_btnClearLog_clicked();         ///< Clear log
    void on_btnClearHistory_clicked();     ///< Clear history
    void on_btnBrowseDir_clicked();        ///< Browse save directory
    void on_btnSaveSetting_clicked();      ///< Save settings
    void on_btnResetSetting_clicked();     ///< Reset settings
    void on_chkSelectAll_toggled(bool checked);  ///< Select all checkbox

protected:
    /**
     * @brief Handle window close event
     * @param event Close event
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    /**
     * @brief Update status bar with current progress
     */
    void updateStatusBar();

private:
    /**
     * @brief Initialize UI components
     */
    void setupUI();
    
    /**
     * @brief Connect signals and slots
     */
    void setupConnections();
    
    /**
     * @brief Load settings from configuration
     */
    void loadSettings();
    
    /**
     * @brief Save settings to configuration
     */
    void saveSettings();
    
    /**
     * @brief Load history records into UI
     */
    void loadHistory();
    
    /**
     * @brief Initialize sound effect for notifications
     */
    void initializeSoundEffect();
    
    /**
     * @brief Play download complete sound notification
     */
    void playDownloadCompleteSound();

    Ui::MainWindow *ui;
    
    // 按钮组
    QButtonGroup *btngLeft;
    QButtonGroup *chkDownload;
    
    // 服务
    IDownloadService *m_downloadService;
    IConfigService *m_configService;
    IHistoryService *m_historyService;
    
    // 模型
    std::unique_ptr<VideoModel> m_videoModel;
    std::unique_ptr<HistoryModel> m_historyModel;
    
    // 状态标志
    bool m_isFirstTaskInBatch;  // 标记是否是批次中的第一个任务
    
    // 解析统计
    int m_parseTotal;
    int m_parseSuccess;
    int m_parseFailed;
    
    // 声音效果
    QSoundEffect *m_soundEffect;
};

#endif // MAINWINDOW_H

