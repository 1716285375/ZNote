#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include "core/interfaces/idownloadservice.h"
#include "component/videomodel.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QCheckBox>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>
#include <QGroupBox>
#include <QFileDialog>
#include <memory>

class DownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWidget(IDownloadService *downloadService, 
                           VideoModel *videoModel, 
                           QWidget *parent = nullptr);

private slots:
    void onParseUrl();
    void onStartDownload();
    void onPauseDownload();
    void onStopDownload();
    void onSelectAll();
    void onClearTasks();
    void onBrowseSavePath();
    void onTaskReady(const DownloadTask &task);
    void onTaskProgress(const QString &taskId, float progress);
    void onTaskFinished(const DownloadTask &task);
    void onLogMessage(const QString &message);

private:
    void setupUI();
    void setupConnections();
    void updateUI();
    void addLogMessage(const QString &message);
    QList<DownloadTask> getSelectedTasks();

    // UI组件
    QVBoxLayout *m_mainLayout;
    
    // URL输入区域
    QGroupBox *m_urlGroup;
    QHBoxLayout *m_urlLayout;
    QLineEdit *m_urlEdit;
    QPushButton *m_parseButton;
    
    // 保存路径区域
    QGroupBox *m_pathGroup;
    QHBoxLayout *m_pathLayout;
    QLineEdit *m_pathEdit;
    QPushButton *m_browseButton;
    
    // 任务列表区域
    QGroupBox *m_taskGroup;
    QVBoxLayout *m_taskLayout;
    QTableView *m_taskView;
    QHBoxLayout *m_taskButtonLayout;
    QPushButton *m_selectAllButton;
    QPushButton *m_clearButton;
    QPushButton *m_startButton;
    QPushButton *m_pauseButton;
    QPushButton *m_stopButton;
    
    // 进度区域
    QGroupBox *m_progressGroup;
    QVBoxLayout *m_progressLayout;
    QProgressBar *m_progressBar;
    QLabel *m_progressLabel;
    
    // 日志区域
    QGroupBox *m_logGroup;
    QVBoxLayout *m_logLayout;
    QTextEdit *m_logText;
    QPushButton *m_clearLogButton;
    
    // 服务
    IDownloadService *m_downloadService;
    VideoModel *m_videoModel;
    
    // 状态
    bool m_isDownloading;
    int m_totalTasks;
    int m_completedTasks;
};

#endif // DOWNLOADWIDGET_H
