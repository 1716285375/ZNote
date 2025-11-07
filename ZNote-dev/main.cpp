#include "app/application.h"
#include "utils/logger.h"
#include "core/download/task.h"
#include <QIcon>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    #ifdef _DEBUG
        AllocConsole(); // 打开控制台
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    #endif
#endif

    // 注册元类型，用于跨线程信号传递
    qRegisterMetaType<DownloadTask>("DownloadTask");
    qRegisterMetaType<DownloadTask>("DownloadTask&");

    Application app(argc, argv);
    
    // 设置应用程序图标
    app.setWindowIcon(QIcon(":/app/logo.ico"));
    
    // 初始化应用程序
    if (!app.initialize()) {
        LOG_CRITICAL("Failed to initialize application");
        return -1;
    }
    
    LOG_INFO("Application started successfully");
    
    int result = app.exec();
    
    LOG_INFO("Application exiting");
    return result;
}
