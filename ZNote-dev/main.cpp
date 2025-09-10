#include "mainwindow.h"

#include <QApplication>
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

    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/app/logo.ico"));
    MainWindow w;
    w.show();
    return a.exec();
}
