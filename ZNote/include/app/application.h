/**
 * @file application.h
 * @brief Main application class for ZNote
 * 
 * This class manages the application lifecycle, services, and main window.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QObject>
#include <memory>

class IConfigService;
class IHistoryService;
class IDownloadService;
class MainWindow;

/**
 * @class Application
 * @brief Main application class that initializes and manages all services
 * 
 * The Application class is responsible for:
 * - Initializing Qt application
 * - Setting up services (Config, History, Download)
 * - Creating and managing the main window
 * - Handling application lifecycle
 */
class Application : public QApplication
{
    Q_OBJECT

public:
    explicit Application(int argc, char *argv[]);
    ~Application() override;

    /**
     * @brief Initialize the application
     * @return true if initialization successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Shutdown the application gracefully
     */
    void shutdown();

    /**
     * @brief Get the configuration service
     * @return Pointer to IConfigService
     */
    IConfigService* configService() const;
    
    /**
     * @brief Get the history service
     * @return Pointer to IHistoryService
     */
    IHistoryService* historyService() const;
    
    /**
     * @brief Get the download service
     * @return Pointer to IDownloadService
     */
    IDownloadService* downloadService() const;
    
    /**
     * @brief Get the main window
     * @return Pointer to MainWindow
     */
    MainWindow* mainWindow() const;

private:
    void setupStyle();
    void setupServices();
    void setupMainWindow();
    void loadSettings();
    void saveSettings();

    std::unique_ptr<IConfigService> m_configService;
    std::unique_ptr<IHistoryService> m_historyService;
    std::unique_ptr<IDownloadService> m_downloadService;
    std::unique_ptr<MainWindow> m_mainWindow;
    
    bool m_initialized;
};

#endif // APPLICATION_H
