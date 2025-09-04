#ifndef VIDEODOWNLOADER_H
#define VIDEODOWNLOADER_H


#include "task.h"

#include <QObject>
#include <QProcess>
#include <QDebug>


class VideoDownloader : public QObject
{
    Q_OBJECT
public:
    explicit VideoDownloader(QObject *parent = nullptr);

    void start(const DownloadTask &task);
    void cancel();

signals:
    void logMessage(const QString& msg);
    void taskStarted(const DownloadTask &task);
    void taskFinished(VideoDownloader *self, const DownloadTask &task);

private:
    void check();
    void handleStdOutput();
    void handleFinished();

private:
    QProcess *process;
    QString program;
    DownloadTask currentTask;
};

#endif // VIDEODOWNLOADER_H
