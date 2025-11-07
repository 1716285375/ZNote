#include "app/application.h"
#include "services/configservice.h"
#include "services/downloadservice.h"
#include "services/historyservice.h"
#include "ui/mainwindow.h"
#include "utils/logger.h"
#include "utils/stylemanager.h"
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QFile>

Application::Application(int argc, char *argv[])
    : QApplication(argc, argv)
    , m_initialized(false)
{
    setApplicationName("ZNote");
    setApplicationVersion("2.0.0");
    setOrganizationName("ZNote Team");
    
    // 设置应用程序属性
    setAttribute(Qt::AA_EnableHighDpiScaling);
    setAttribute(Qt::AA_UseHighDpiPixmaps);
}

Application::~Application()
{
    shutdown();
}

bool Application::initialize()
{
    if (m_initialized) {
        return true;
    }
    
    LOG_INFO("Initializing application...");
    
    try {
        setupServices();
        setupStyle();
        setupMainWindow();
        loadSettings();
        
        m_initialized = true;
        LOG_INFO("Application initialized successfully");
        return true;
    } catch (const std::exception &e) {
        LOG_ERROR(QString("Failed to initialize application: %1").arg(e.what()));
        return false;
    }
}

void Application::shutdown()
{
    if (!m_initialized) {
        return;
    }
    
    LOG_INFO("Shutting down application...");
    
    // 保存设置
    saveSettings();
    
    // 显式保存历史记录，确保数据不丢失
    if (m_historyService) {
        // 直接调用 forceSave 方法，确保同步保存
        HistoryService* historyService = dynamic_cast<HistoryService*>(m_historyService.get());
        if (historyService) {
            historyService->forceSave();
            LOG_INFO("History saved during shutdown");
        }
    }
    
    // 按顺序释放资源：先释放依赖其他服务的对象
    m_mainWindow.reset();
    m_downloadService.reset();
    // HistoryService 的析构函数会再次保存（双重保险）
    m_historyService.reset();
    m_configService.reset();
    
    m_initialized = false;
    LOG_INFO("Application shutdown complete");
}

IConfigService* Application::configService() const
{
    return m_configService.get();
}

IHistoryService* Application::historyService() const
{
    return m_historyService.get();
}

IDownloadService* Application::downloadService() const
{
    return m_downloadService.get();
}

MainWindow* Application::mainWindow() const
{
    return m_mainWindow.get();
}

void Application::setupStyle()
{
    LOG_INFO("Setting up style manager...");
    
    // 初始化样式管理器
    StyleManager* styleManager = StyleManager::instance();
    
    // 从配置加载主题设置，默认为浅色主题
    if (m_configService) {
        int theme = m_configService->getValue("ui.theme", 0).toInt();
        styleManager->setTheme(static_cast<StyleManager::Theme>(theme));
    } else {
        // 如果配置服务还未初始化，使用默认主题
        styleManager->setTheme(StyleManager::Light);
    }
    
    LOG_INFO("Style manager setup complete");
}

void Application::setupServices()
{
    LOG_INFO("Setting up services...");
    
    // 确定配置文件路径：优先使用应用程序目录下的 config.json
    QString configPath;
    QString appDir = QCoreApplication::applicationDirPath();
    QString projectConfigPath = QDir(appDir).filePath("config.json");
    
    // 如果应用程序目录下存在 config.json，使用它
    if (QFile::exists(projectConfigPath)) {
        configPath = projectConfigPath;
        LOG_INFO(QString("Using config file from application directory: %1").arg(configPath));
    } else {
        // 否则使用默认路径（AppDataLocation）
        LOG_INFO("Config file not found in application directory, using default location");
    }
    
    // 创建配置服务
    m_configService = std::make_unique<ConfigService>(configPath);
    
    // 创建历史服务
    m_historyService = std::make_unique<HistoryService>();
    
    // 创建下载服务
    m_downloadService = std::make_unique<DownloadService>(
        m_configService.get(), 
        m_historyService.get()
    );
    
    LOG_INFO("Services setup complete");
}

void Application::setupMainWindow()
{
    LOG_INFO("Creating main window...");
    
    m_mainWindow = std::make_unique<MainWindow>(
        m_downloadService.get(),
        m_configService.get(),
        m_historyService.get()
    );
    m_mainWindow->show();
    
    LOG_INFO("Main window created");
}

void Application::loadSettings()
{
    LOG_INFO("Loading settings...");
    
    if (m_configService) {
        m_configService->load();
    }
    
    LOG_INFO("Settings loaded");
}

void Application::saveSettings()
{
    LOG_INFO("Saving settings...");
    
    if (m_configService) {
        m_configService->save();
    }
    
    LOG_INFO("Settings saved");
}

