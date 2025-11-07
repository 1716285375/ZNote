#include "ui/widgets/settingswidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

SettingsWidget::SettingsWidget(IConfigService *configService, QWidget *parent)
    : QWidget(parent)
    , m_configService(configService)
{
    setupUI();
    setupConnections();
    loadSettings();
}

SettingsWidget::~SettingsWidget()
{
}

void SettingsWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // 创建标签页
    m_tabWidget = new QTabWidget(this);
    
    setupDownloadTab();
    setupAdvancedTab();
    setupAboutTab();
    
    m_mainLayout->addWidget(m_tabWidget);
    
    // 按钮布局
    m_buttonLayout = new QHBoxLayout();
    m_resetButton = new QPushButton("恢复默认", this);
    m_applyButton = new QPushButton("应用", this);
    m_cancelButton = new QPushButton("取消", this);
    
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_resetButton);
    m_buttonLayout->addWidget(m_applyButton);
    m_buttonLayout->addWidget(m_cancelButton);
    
    m_mainLayout->addLayout(m_buttonLayout);
    
    // 状态标签
    m_statusLabel = new QLabel("就绪", this);
    m_mainLayout->addWidget(m_statusLabel);
}

void SettingsWidget::setupDownloadTab()
{
    QWidget *downloadTab = new QWidget();
    QVBoxLayout *downloadLayout = new QVBoxLayout(downloadTab);
    
    // 下载设置组
    m_downloadGroup = new QGroupBox("下载设置", downloadTab);
    m_downloadLayout = new QFormLayout(m_downloadGroup);
    
    // 下载路径
    QHBoxLayout *pathLayout = new QHBoxLayout();
    m_downloadPathEdit = new QLineEdit(downloadTab);
    m_browseDownloadButton = new QPushButton("浏览...", downloadTab);
    pathLayout->addWidget(m_downloadPathEdit);
    pathLayout->addWidget(m_browseDownloadButton);
    m_downloadLayout->addRow("下载路径:", pathLayout);
    
    // yt-dlp路径
    QHBoxLayout *ytDlpLayout = new QHBoxLayout();
    m_ytDlpPathEdit = new QLineEdit(downloadTab);
    m_browseYtDlpButton = new QPushButton("浏览...", downloadTab);
    ytDlpLayout->addWidget(m_ytDlpPathEdit);
    ytDlpLayout->addWidget(m_browseYtDlpButton);
    m_downloadLayout->addRow("yt-dlp路径:", ytDlpLayout);
    
    // 线程数
    m_threadCountSpinBox = new QSpinBox(downloadTab);
    m_threadCountSpinBox->setRange(1, 16);
    m_threadCountSpinBox->setValue(4);
    m_downloadLayout->addRow("下载线程数:", m_threadCountSpinBox);
    
    // 重试次数
    m_retryCountSpinBox = new QSpinBox(downloadTab);
    m_retryCountSpinBox->setRange(0, 10);
    m_retryCountSpinBox->setValue(3);
    m_downloadLayout->addRow("重试次数:", m_retryCountSpinBox);
    
    // 超时时间
    m_timeoutSpinBox = new QSpinBox(downloadTab);
    m_timeoutSpinBox->setRange(10, 300);
    m_timeoutSpinBox->setValue(30);
    m_timeoutSpinBox->setSuffix(" 秒");
    m_downloadLayout->addRow("超时时间:", m_timeoutSpinBox);
    
    // 质量选择
    m_qualityComboBox = new QComboBox(downloadTab);
    m_qualityComboBox->addItems({"最佳", "1080p", "720p", "480p", "360p", "最差"});
    m_downloadLayout->addRow("视频质量:", m_qualityComboBox);
    
    // 选项
    m_subtitleCheckBox = new QCheckBox("下载字幕", downloadTab);
    m_thumbnailCheckBox = new QCheckBox("下载缩略图", downloadTab);
    m_downloadLayout->addRow(m_subtitleCheckBox);
    m_downloadLayout->addRow(m_thumbnailCheckBox);
    
    // 下载完成后操作
    m_playSoundCheckBox = new QCheckBox("下载完成后播放提示音", downloadTab);
    m_autoOpenDirCheckBox = new QCheckBox("下载完成后自动打开目录", downloadTab);
    m_downloadLayout->addRow(m_playSoundCheckBox);
    m_downloadLayout->addRow(m_autoOpenDirCheckBox);
    
    downloadLayout->addWidget(m_downloadGroup);
    downloadLayout->addStretch();
    
    m_tabWidget->addTab(downloadTab, "下载");
}

void SettingsWidget::setupAdvancedTab()
{
    QWidget *advancedTab = new QWidget();
    QVBoxLayout *advancedLayout = new QVBoxLayout(advancedTab);
    
    // 高级设置组
    m_advancedGroup = new QGroupBox("高级设置", advancedTab);
    m_advancedLayout = new QFormLayout(m_advancedGroup);
    
    // 启动选项
    m_autoStartCheckBox = new QCheckBox("开机自启动", advancedTab);
    m_minimizeToTrayCheckBox = new QCheckBox("最小化到系统托盘", advancedTab);
    m_showNotificationsCheckBox = new QCheckBox("显示通知", advancedTab);
    
    m_advancedLayout->addRow(m_autoStartCheckBox);
    m_advancedLayout->addRow(m_minimizeToTrayCheckBox);
    m_advancedLayout->addRow(m_showNotificationsCheckBox);
    
    // 自动清理
    QHBoxLayout *cleanupLayout = new QHBoxLayout();
    m_autoCleanupCheckBox = new QCheckBox("自动清理", advancedTab);
    m_cleanupDaysSpinBox = new QSpinBox(advancedTab);
    m_cleanupDaysSpinBox->setRange(1, 365);
    m_cleanupDaysSpinBox->setValue(30);
    m_cleanupDaysSpinBox->setSuffix(" 天");
    cleanupLayout->addWidget(m_autoCleanupCheckBox);
    cleanupLayout->addWidget(m_cleanupDaysSpinBox);
    cleanupLayout->addStretch();
    m_advancedLayout->addRow("清理设置:", cleanupLayout);
    
    // 日志级别
    m_logLevelComboBox = new QComboBox(advancedTab);
    m_logLevelComboBox->addItems({"调试", "信息", "警告", "错误"});
    m_advancedLayout->addRow("日志级别:", m_logLevelComboBox);
    
    advancedLayout->addWidget(m_advancedGroup);
    advancedLayout->addStretch();
    
    m_tabWidget->addTab(advancedTab, "高级");
}

void SettingsWidget::setupAboutTab()
{
    QWidget *aboutTab = new QWidget();
    QVBoxLayout *aboutLayout = new QVBoxLayout(aboutTab);
    
    QLabel *aboutLabel = new QLabel("ZNote - B站视频下载工具\n\n版本: 1.0.0\n作者: jiezcode\n\n这是一个高性能的B站视频下载工具，支持批量下载、多线程下载等功能。", aboutTab);
    aboutLabel->setAlignment(Qt::AlignCenter);
    aboutLabel->setWordWrap(true);
    
    aboutLayout->addStretch();
    aboutLayout->addWidget(aboutLabel);
    aboutLayout->addStretch();
    
    m_tabWidget->addTab(aboutTab, "关于");
}

void SettingsWidget::setupConnections()
{
    connect(m_browseDownloadButton, &QPushButton::clicked, this, &SettingsWidget::onBrowseDownloadPath);
    connect(m_browseYtDlpButton, &QPushButton::clicked, this, &SettingsWidget::onBrowseYtDlpPath);
    connect(m_resetButton, &QPushButton::clicked, this, &SettingsWidget::onResetToDefaults);
    connect(m_applyButton, &QPushButton::clicked, this, &SettingsWidget::onApplySettings);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsWidget::onCancelChanges);
    
    connect(m_downloadPathEdit, &QLineEdit::textChanged, this, &SettingsWidget::updatePathValidation);
    connect(m_ytDlpPathEdit, &QLineEdit::textChanged, this, &SettingsWidget::updatePathValidation);
}

void SettingsWidget::loadSettings()
{
    if (!m_configService) {
        qDebug() << "SettingsWidget::loadSettings: ConfigService is null";
        return;
    }
    
    // 加载下载设置
    QString defaultPath = m_configService->getValue("download.defaultPath", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).toString();
    m_downloadPathEdit->setText(defaultPath);
    qDebug() << "Loaded defaultPath:" << defaultPath;
    
    m_ytDlpPathEdit->setText(m_configService->getValue("download.ytdlpPath", "").toString());
    m_threadCountSpinBox->setValue(m_configService->getValue("download.threadCount", 4).toInt());
    m_retryCountSpinBox->setValue(m_configService->getValue("download.retryCount", 3).toInt());
    m_timeoutSpinBox->setValue(m_configService->getValue("download.timeout", 30).toInt());
    m_qualityComboBox->setCurrentText(m_configService->getValue("download.quality", "最佳").toString());
    m_subtitleCheckBox->setChecked(m_configService->getValue("download.subtitle", false).toBool());
    m_thumbnailCheckBox->setChecked(m_configService->getValue("download.thumbnail", false).toBool());
    
    // 加载播放声音设置
    bool playSound = m_configService->getValue("download.onComplete.playSound", false).toBool();
    qDebug() << "SettingsWidget::loadSettings: playSound value from config:" << playSound;
    m_playSoundCheckBox->setChecked(playSound);
    qDebug() << "SettingsWidget::loadSettings: playSoundCheckBox checked:" << m_playSoundCheckBox->isChecked();
    
    // 加载自动打开目录设置
    bool autoOpenDir = m_configService->getValue("download.onComplete.autoOpenDir", false).toBool();
    qDebug() << "SettingsWidget::loadSettings: autoOpenDir value from config:" << autoOpenDir;
    m_autoOpenDirCheckBox->setChecked(autoOpenDir);
    qDebug() << "SettingsWidget::loadSettings: autoOpenDirCheckBox checked:" << m_autoOpenDirCheckBox->isChecked();
    
    // 加载高级设置
    m_autoStartCheckBox->setChecked(m_configService->getValue("advanced.autoStart", false).toBool());
    m_minimizeToTrayCheckBox->setChecked(m_configService->getValue("advanced.minimizeToTray", true).toBool());
    m_showNotificationsCheckBox->setChecked(m_configService->getValue("advanced.showNotifications", true).toBool());
    m_autoCleanupCheckBox->setChecked(m_configService->getValue("advanced.autoCleanup", false).toBool());
    m_cleanupDaysSpinBox->setValue(m_configService->getValue("advanced.cleanupDays", 30).toInt());
    m_logLevelComboBox->setCurrentText(m_configService->getValue("advanced.logLevel", "信息").toString());
    
    updatePathValidation();
}

void SettingsWidget::saveSettings()
{
    if (!m_configService) return;
    
    // 保存下载设置
    m_configService->setValue("download.defaultPath", m_downloadPathEdit->text());
    m_configService->setValue("download.ytdlpPath", m_ytDlpPathEdit->text());
    m_configService->setValue("download.threadCount", m_threadCountSpinBox->value());
    m_configService->setValue("download.retryCount", m_retryCountSpinBox->value());
    m_configService->setValue("download.timeout", m_timeoutSpinBox->value());
    m_configService->setValue("download.quality", m_qualityComboBox->currentText());
    m_configService->setValue("download.subtitle", m_subtitleCheckBox->isChecked());
    m_configService->setValue("download.thumbnail", m_thumbnailCheckBox->isChecked());
    m_configService->setValue("download.onComplete.playSound", m_playSoundCheckBox->isChecked());
    m_configService->setValue("download.onComplete.autoOpenDir", m_autoOpenDirCheckBox->isChecked());
    
    // 保存高级设置
    m_configService->setValue("advanced.autoStart", m_autoStartCheckBox->isChecked());
    m_configService->setValue("advanced.minimizeToTray", m_minimizeToTrayCheckBox->isChecked());
    m_configService->setValue("advanced.showNotifications", m_showNotificationsCheckBox->isChecked());
    m_configService->setValue("advanced.autoCleanup", m_autoCleanupCheckBox->isChecked());
    m_configService->setValue("advanced.cleanupDays", m_cleanupDaysSpinBox->value());
    m_configService->setValue("advanced.logLevel", m_logLevelComboBox->currentText());
    
    m_configService->save();
    m_statusLabel->setText("设置已保存");
}

void SettingsWidget::onBrowseDownloadPath()
{
    QString path = QFileDialog::getExistingDirectory(this, "选择下载路径", m_downloadPathEdit->text());
    if (!path.isEmpty()) {
        m_downloadPathEdit->setText(path);
    }
}

void SettingsWidget::onBrowseYtDlpPath()
{
    QString path = QFileDialog::getOpenFileName(this, "选择yt-dlp程序", m_ytDlpPathEdit->text(), "可执行文件 (*.exe)");
    if (!path.isEmpty()) {
        m_ytDlpPathEdit->setText(path);
    }
}

void SettingsWidget::onResetToDefaults()
{
    if (QMessageBox::question(this, "确认", "确定要恢复默认设置吗？", 
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        // 重置为默认值
        m_downloadPathEdit->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        m_ytDlpPathEdit->setText("");
        m_threadCountSpinBox->setValue(4);
        m_retryCountSpinBox->setValue(3);
        m_timeoutSpinBox->setValue(30);
        m_qualityComboBox->setCurrentText("最佳");
        m_subtitleCheckBox->setChecked(false);
        m_thumbnailCheckBox->setChecked(false);
        
        m_autoStartCheckBox->setChecked(false);
        m_minimizeToTrayCheckBox->setChecked(true);
        m_showNotificationsCheckBox->setChecked(true);
        m_autoCleanupCheckBox->setChecked(false);
        m_cleanupDaysSpinBox->setValue(30);
        m_logLevelComboBox->setCurrentText("信息");
        
        m_statusLabel->setText("已恢复默认设置");
    }
}

void SettingsWidget::onApplySettings()
{
    saveSettings();
}

void SettingsWidget::onCancelChanges()
{
    loadSettings();
    m_statusLabel->setText("已取消更改");
}

void SettingsWidget::updatePathValidation()
{
    // 验证路径有效性
    QString downloadPath = m_downloadPathEdit->text();
    QString ytDlpPath = m_ytDlpPathEdit->text();
    
    bool downloadValid = QDir(downloadPath).exists();
    bool ytDlpValid = ytDlpPath.isEmpty() || QFile::exists(ytDlpPath);
    
    // 设置样式
    QString validStyle = "QLineEdit { border: 1px solid green; }";
    QString invalidStyle = "QLineEdit { border: 1px solid red; }";
    
    m_downloadPathEdit->setStyleSheet(downloadValid ? validStyle : invalidStyle);
    m_ytDlpPathEdit->setStyleSheet(ytDlpValid ? validStyle : invalidStyle);
    
    m_applyButton->setEnabled(downloadValid && ytDlpValid);
}

