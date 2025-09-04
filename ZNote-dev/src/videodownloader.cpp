#include "VideoDownloader.h"
#include <QStringList>
#include <QOverload>
#include <QStandardPaths>
#include <QFileInfo>
#include <QStringList>
#include <QDir>
#include <QDateTime>
#include <QDebug>



VideoDownloader::VideoDownloader(QObject *parent)
    : QObject(parent),
    process(new QProcess(this)),
    program(QString("yt-dlp.exe"))
{
    connect(process, &QProcess::readyReadStandardOutput, this, &VideoDownloader::handleStdOutput);
    connect(process, &QProcess::finished, this, &VideoDownloader::handleFinished);

    connect(process, &QProcess::readyReadStandardError, [this](){
        emit logMessage(QString::fromUtf8(process->readAllStandardError()));
    });
}

void VideoDownloader::start(const DownloadTask &task)
{
    currentTask = task;

    if (process == nullptr)
    {
        process = new QProcess(this);
    }

    QString targetFile = QDir::toNativeSeparators(
        QString("%1/%2/%3 - %(title)s.%(ext)s")
            .arg(task.savePath, task.playlistTitle, QString::number(task.index))
        );


    QStringList args;
    args << "-o" << QDir::toNativeSeparators(targetFile);
    args << task.url;

    process->start(program, args);

    currentTask.startTime = QDateTime::currentDateTime();
    emit taskStarted(currentTask);
}

void VideoDownloader::cancel()
{
    if (process && process->state() == QProcess::Running)
    {
        process->kill();
        process->waitForFinished();
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
