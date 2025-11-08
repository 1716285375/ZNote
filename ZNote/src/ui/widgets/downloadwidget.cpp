#include "ui/widgets/downloadwidget.h"
#include "utils/logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QCheckBox>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QStandardPaths>

DownloadWidget::DownloadWidget(IDownloadService *downloadService, 
                             VideoModel *videoModel, 
                             QWidget *parent)
    : QWidget(parent)
    , m_downloadService(downloadService)
    , m_videoModel(videoModel)
    , m_isDownloading(false)
    , m_totalTasks(0)
    , m_completedTasks(0)
{
    setupUI();
    setupConnections();
    updateUI();
}

void DownloadWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // URL输入区域
    m_urlGroup = new QGroupBox("Video URL", this);
    m_urlLayout = new QHBoxLayout(m_urlGroup);
    m_urlEdit = new QLineEdit(this);
    m_urlEdit->setPlaceholderText("Enter Bilibili video URL...");
    m_parseButton = new QPushButton("Parse", this);
    m_urlLayout->addWidget(m_urlEdit);
    m_urlLayout->addWidget(m_parseButton);
    
    // 保存路径区域
    m_pathGroup = new QGroupBox("Save Path", this);
    m_pathLayout = new QHBoxLayout(m_pathGroup);
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    m_browseButton = new QPushButton("Browse", this);
    m_pathLayout->addWidget(m_pathEdit);
    m_pathLayout->addWidget(m_browseButton);
    
    // 任务列表区域
    m_taskGroup = new QGroupBox("Download Tasks", this);
    m_taskLayout = new QVBoxLayout(m_taskGroup);
    m_taskView = new QTableView(this);
    m_taskView->setModel(m_videoModel);
    m_taskView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_taskView->setAlternatingRowColors(true);
    m_taskView->horizontalHeader()->setStretchLastSection(true);
    
    m_taskButtonLayout = new QHBoxLayout();
    m_selectAllButton = new QPushButton("Select All", this);
    m_clearButton = new QPushButton("Clear", this);
    m_startButton = new QPushButton("Start", this);
    m_pauseButton = new QPushButton("Pause", this);
    m_stopButton = new QPushButton("Stop", this);
    
    m_taskButtonLayout->addWidget(m_selectAllButton);
    m_taskButtonLayout->addWidget(m_clearButton);
    m_taskButtonLayout->addStretch();
    m_taskButtonLayout->addWidget(m_startButton);
    m_taskButtonLayout->addWidget(m_pauseButton);
    m_taskButtonLayout->addWidget(m_stopButton);
    
    m_taskLayout->addWidget(m_taskView);
    m_taskLayout->addLayout(m_taskButtonLayout);
    
    // 进度区域
    m_progressGroup = new QGroupBox("Progress", this);
    m_progressLayout = new QVBoxLayout(m_progressGroup);
    m_progressBar = new QProgressBar(this);
    m_progressLabel = new QLabel("Ready", this);
    m_progressLayout->addWidget(m_progressBar);
    m_progressLayout->addWidget(m_progressLabel);
    
    // 日志区域
    m_logGroup = new QGroupBox("Log", this);
    m_logLayout = new QVBoxLayout(m_logGroup);
    m_logText = new QTextEdit(this);
    m_logText->setReadOnly(true);
    m_logText->setMaximumHeight(150);
    m_clearLogButton = new QPushButton("Clear Log", this);
    m_logLayout->addWidget(m_logText);
    m_logLayout->addWidget(m_clearLogButton);
    
    // 添加到主布局
    m_mainLayout->addWidget(m_urlGroup);
    m_mainLayout->addWidget(m_pathGroup);
    m_mainLayout->addWidget(m_taskGroup, 1);
    m_mainLayout->addWidget(m_progressGroup);
    m_mainLayout->addWidget(m_logGroup);
}

void DownloadWidget::setupConnections()
{
    // 按钮连接
    connect(m_parseButton, &QPushButton::clicked, this, &DownloadWidget::onParseUrl);
    connect(m_browseButton, &QPushButton::clicked, this, &DownloadWidget::onBrowseSavePath);
    connect(m_selectAllButton, &QPushButton::clicked, this, &DownloadWidget::onSelectAll);
    connect(m_clearButton, &QPushButton::clicked, this, &DownloadWidget::onClearTasks);
    connect(m_startButton, &QPushButton::clicked, this, &DownloadWidget::onStartDownload);
    connect(m_pauseButton, &QPushButton::clicked, this, &DownloadWidget::onPauseDownload);
    connect(m_stopButton, &QPushButton::clicked, this, &DownloadWidget::onStopDownload);
    connect(m_clearLogButton, &QPushButton::clicked, this, [this]() {
        m_logText->clear();
    });
    
    // 下载服务连接
    if (m_downloadService) {
        connect(m_downloadService, &IDownloadService::taskReady,
                this, &DownloadWidget::onTaskReady);
        connect(m_downloadService, &IDownloadService::taskProgress,
                this, &DownloadWidget::onTaskProgress);
        connect(m_downloadService, &IDownloadService::taskFinished,
                this, &DownloadWidget::onTaskFinished);
        connect(m_downloadService, &IDownloadService::logMessage,
                this, &DownloadWidget::onLogMessage);
    }
}

void DownloadWidget::updateUI()
{
    bool hasTasks = m_videoModel && m_videoModel->rowCount() > 0;
    bool isRunning = m_downloadService && m_downloadService->isRunning();
    bool isPaused = m_downloadService && m_downloadService->isPaused();
    
    m_parseButton->setEnabled(!isRunning);
    m_startButton->setEnabled(hasTasks && !isRunning);
    m_pauseButton->setEnabled(isRunning && !isPaused);
    m_stopButton->setEnabled(isRunning);
    m_clearButton->setEnabled(hasTasks && !isRunning);
    m_selectAllButton->setEnabled(hasTasks);
    
    if (isRunning) {
        m_progressLabel->setText("Downloading...");
    } else if (isPaused) {
        m_progressLabel->setText("Paused");
    } else {
        m_progressLabel->setText("Ready");
    }
}

void DownloadWidget::addLogMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss]");
    m_logText->append(QString("%1 %2").arg(timestamp).arg(message));
    
    // 自动滚动到底部
    QTextCursor cursor = m_logText->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logText->setTextCursor(cursor);
}

QList<DownloadTask> DownloadWidget::getSelectedTasks()
{
    QList<DownloadTask> selectedTasks;
    
    if (!m_videoModel) {
        return selectedTasks;
    }
    
    // 任务选择逻辑：从VideoModel中获取选中的任务
    // 当前版本通过复选框列（第6列）来判断任务是否被选中
    for (int i = 0; i < m_videoModel->rowCount(); ++i) {
        QModelIndex checkIndex = m_videoModel->index(i, 6);
        if (m_videoModel->data(checkIndex, Qt::CheckStateRole).toInt() == Qt::Checked) {
            // 获取任务数据（需要根据VideoModel的实际实现来获取）
            // 当前版本暂未实现完整逻辑，保留接口供后续扩展
        }
    }
    
    return selectedTasks;
}

void DownloadWidget::onParseUrl()
{
    QString url = m_urlEdit->text().trimmed();
    QString savePath = m_pathEdit->text().trimmed();
    
    if (url.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a video URL");
        return;
    }
    
    if (savePath.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a save path");
        return;
    }
    
    if (m_downloadService) {
        m_downloadService->parseUrl(url, savePath);
        addLogMessage(QString("Parsing URL: %1").arg(url));
    }
}

void DownloadWidget::onStartDownload()
{
    if (!m_downloadService) {
        return;
    }
    
    QList<DownloadTask> selectedTasks = getSelectedTasks();
    if (selectedTasks.isEmpty()) {
        QMessageBox::information(this, "Info", "No tasks selected for download");
        return;
    }
    
    m_downloadService->addTasks(selectedTasks);
    m_downloadService->startDownload();
    
    m_isDownloading = true;
    updateUI();
    addLogMessage("Download started");
}

void DownloadWidget::onPauseDownload()
{
    if (m_downloadService) {
        m_downloadService->pauseDownload();
        addLogMessage("Download paused");
    }
}

void DownloadWidget::onStopDownload()
{
    if (m_downloadService) {
        m_downloadService->stopDownload();
        m_isDownloading = false;
        updateUI();
        addLogMessage("Download stopped");
    }
}

void DownloadWidget::onSelectAll()
{
    // 全选功能：选中所有任务
    if (m_videoModel) {
        for (int i = 0; i < m_videoModel->rowCount(); ++i) {
            QModelIndex checkIndex = m_videoModel->index(i, 6);
            m_videoModel->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
        }
        addLogMessage("All tasks selected");
    }
}

void DownloadWidget::onClearTasks()
{
    if (m_downloadService) {
        m_downloadService->clearTasks();
        if (m_videoModel) {
            m_videoModel->clear();
        }
        updateUI();
        addLogMessage("Tasks cleared");
    }
}

void DownloadWidget::onBrowseSavePath()
{
    QString path = QFileDialog::getExistingDirectory(this, "Select Save Directory");
    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}

void DownloadWidget::onTaskReady(const DownloadTask &task)
{
    if (m_videoModel) {
        m_videoModel->addTask(task);
    }
    updateUI();
    addLogMessage(QString("Task ready: %1").arg(task.video.title));
}

void DownloadWidget::onTaskProgress(const QString &taskId, float progress)
{
    Q_UNUSED(taskId)
    
    m_progressBar->setValue(static_cast<int>(progress * 100));
    
    if (m_downloadService) {
        int total = m_downloadService->getTaskCount();
        int completed = m_downloadService->getCompletedCount();
        m_progressLabel->setText(QString("Progress: %1/%2 (%3%)")
                                .arg(completed)
                                .arg(total)
                                .arg(static_cast<int>(progress * 100)));
    }
}

void DownloadWidget::onTaskFinished(const DownloadTask &task)
{
    m_completedTasks++;
    updateUI();
    addLogMessage(QString("Task completed: %1").arg(task.video.title));
}

void DownloadWidget::onLogMessage(const QString &message)
{
    addLogMessage(message);
}
