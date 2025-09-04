#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>

#include "videodownloader.h"
#include "taskqueue.h"
#include "core/filebrowser.h"

class QButtonGroup;
class DownloadManager;
class ConfigManager;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupConnection();
    void init();

	void checkState();

	void initConfig();

    void initUI();

private slots:
    // 自动connect
    void on_btnDownload_clicked();
    void on_btnCrap_clicked();


    // 手动connect
    void appendLog(const QString &msg);
    void onDownloadFinished(bool success);
    void onCrapFinished(QStringList videos);

private:
    Ui::MainWindow *ui;
    DownloadManager *manager;

    /** 统一管理左侧导航栏按钮 */
    QButtonGroup *btngLeft;
    QButtonGroup *btngNote;

    /** 上一次合法文件保存目录 */
    ConfigManager &config;

    /** 文件浏览器 */
	FileBrowser* fileBrowser;


};
#endif // MAINWINDOW_H
