#include "core/download/videodownloader.h"
#include "utils/downloadutils.h"
#include <QStringList>
#include <QOverload>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFile>



VideoDownloader::VideoDownloader(QObject *parent)
    : QObject(parent),
    process(nullptr),
    program(QString("yt-dlp.exe"))
{
    // process 将在 start() 方法中创建，确保在正确的线程中创建
    // 先查找yt-dlp可执行文件路径
    QString executable = QStandardPaths::findExecutable(program);
    if (executable.isEmpty()) {
        // 尝试在项目bin目录中查找
        QString projectPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../bin/yt-dlp.exe");
        if (QFile::exists(projectPath)) {
            program = projectPath;
        } else {
            // 尝试在应用程序目录中查找
            QString appDir = QCoreApplication::applicationDirPath();
            QString binPath = QDir(appDir).filePath("yt-dlp.exe");
            if (QFile::exists(binPath)) {
                program = binPath;
            }
        }
    } else {
        program = executable;
    }
}

void VideoDownloader::start(const DownloadTask &task)
{
    currentTask = task;

    // 在线程中创建 QProcess，确保在正确的线程中
    if (process == nullptr)
    {
        process = new QProcess(this);
        
        // 查找yt-dlp可执行文件
        QString executable = QStandardPaths::findExecutable(program);
        if (executable.isEmpty()) {
            // 尝试在项目bin目录中查找
            QString projectPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../bin/yt-dlp.exe");
            if (QFile::exists(projectPath)) {
                program = projectPath;
            } else {
                // 尝试在应用程序目录中查找
                QString appDir = QCoreApplication::applicationDirPath();
                QString binPath = QDir(appDir).filePath("yt-dlp.exe");
                if (QFile::exists(binPath)) {
                    program = binPath;
                } else {
                    emit logMessage("❌ Cannot find yt-dlp.exe in PATH or project bin directory");
                }
            }
        } else {
            program = executable;
        }
        
        // 连接信号
        connect(process, &QProcess::readyReadStandardOutput, this, &VideoDownloader::handleStdOutput);
        connect(process, &QProcess::finished, this, &VideoDownloader::handleFinished);
        connect(process, &QProcess::readyReadStandardError, [this](){
            emit logMessage(QString::fromUtf8(process->readAllStandardError()));
        });
    }

    QStringList args = znote::utils::buildDownloadCommand(task);

    znote::utils::printCommand(args);

    // QProcess::start 是异步的，不会阻塞
    process->start(program, args);

    currentTask.startTime = QDateTime::currentDateTime();

    emit taskStarted(currentTask);
}

void VideoDownloader::cancel()
{
    if (process && process->state() == QProcess::Running)
    {
        process->kill();
        // 不等待完成，避免阻塞UI线程
        // process->waitForFinished();  // 移除阻塞调用
    }
}

void VideoDownloader::check()
{
    program = QStandardPaths::findExecutable(program);
    if (program.isEmpty()) {
        emit logMessage(QString("❌ Cannot find %1 in PATH").arg(program));
    }
}

void VideoDownloader::handleStdOutput()
{
    while (process->canReadLine())
    {
        QString line = QString::fromLocal8Bit(process->readLine()).trimmed();
        if (!line.isEmpty()) emit logMessage(line);
    }
}

void VideoDownloader::handleFinished()
{
    currentTask.endTime = QDateTime::currentDateTime();
    emit taskFinished(this, currentTask);
}
