#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "core/interfaces/iconfigservice.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QTabWidget>

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(IConfigService *configService, QWidget *parent = nullptr);
    ~SettingsWidget() override;

    void loadSettings();
    void saveSettings();

private slots:
    void onBrowseDownloadPath();
    void onBrowseYtDlpPath();
    void onResetToDefaults();
    void onApplySettings();
    void onCancelChanges();

private:
    void setupUI();
    void setupConnections();
    void setupDownloadTab();
    void setupAdvancedTab();
    void setupAboutTab();
    void updatePathValidation();

    IConfigService *m_configService;
    
    // 主布局
    QVBoxLayout *m_mainLayout;
    QTabWidget *m_tabWidget;
    QHBoxLayout *m_buttonLayout;
    
    // 下载设置
    QGroupBox *m_downloadGroup;
    QFormLayout *m_downloadLayout;
    QLineEdit *m_downloadPathEdit;
    QPushButton *m_browseDownloadButton;
    QLineEdit *m_ytDlpPathEdit;
    QPushButton *m_browseYtDlpButton;
    QSpinBox *m_threadCountSpinBox;
    QSpinBox *m_retryCountSpinBox;
    QSpinBox *m_timeoutSpinBox;
    QComboBox *m_qualityComboBox;
    QCheckBox *m_subtitleCheckBox;
    QCheckBox *m_thumbnailCheckBox;
    QCheckBox *m_playSoundCheckBox;  // 下载完成后播放提示音
    QCheckBox *m_autoOpenDirCheckBox;  // 下载完成后自动打开目录
    
    // 高级设置
    QGroupBox *m_advancedGroup;
    QFormLayout *m_advancedLayout;
    QCheckBox *m_autoStartCheckBox;
    QCheckBox *m_minimizeToTrayCheckBox;
    QCheckBox *m_showNotificationsCheckBox;
    QCheckBox *m_autoCleanupCheckBox;
    QSpinBox *m_cleanupDaysSpinBox;
    QComboBox *m_logLevelComboBox;
    
    // 按钮
    QPushButton *m_resetButton;
    QPushButton *m_applyButton;
    QPushButton *m_cancelButton;
    
    // 状态标签
    QLabel *m_statusLabel;
};

#endif // SETTINGSWIDGET_H

