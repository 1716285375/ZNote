#include "downloadmanager.h"
#include "videodownloader.h"
#include "historymanager.h"

#include <QMessageBox>

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
    ,queue(new TaskQueue(this, 2)) // 默认 2 并发
    ,history(new HistoryManager(HistoryStorageType::SQLite, "history.db", this))
{
    connect(queue, &TaskQueue::logMessage, this, &DownloadManager::logMessage);
    connect(queue, &TaskQueue::allFinished, this, &DownloadManager::allFinished);
    connect(queue, &TaskQueue::taskFinished, this, &DownloadManager::onTaskFinished);
}

void DownloadManager::parseUrl(const QString &url, const QString &savePath)
{
    // 每次调用时，创建一个新的 UrlParser
    auto *parser = new UrlParser(this);

    // 解析成功
    connect(parser, &UrlParser::urlParsed, this, [this, savePath, parser](const QList<ParsedEntry> &entries) {
        for (const ParsedEntry &entry : entries) {
            DownloadTask task;
            task.url = entry.url;
            task.savePath = savePath;
            task.playlistTitle = entry.playlistTitle;
            task.index = entry.index;
            // 应用配置参数
            task.resolution = cfg.getResolution();
            task.audioFormat = cfg.getAudioFormat();
            task.subtitles = cfg.isSubtitlesEnabled();

            emit logMessage("解析成功: " + task.url + " 加入下载队列");
            addTask(task);
        }

        QMutexLocker locker(&mutex);
        totalTasks += entries.size();

        QMessageBox::information(
            nullptr,
            tr("解析情况"),
            tr("本次成功解析了 %1 个视频").arg(entries.size())
            );



        emit taskProgress(doneTasks, 1.0f * doneTasks / totalTasks);

        parser->deleteLater(); // 解析完成后销毁
    });

    // 日志输出
    connect(parser, &UrlParser::logMessage, this, &DownloadManager::logMessage);

    // 出错也要释放资源
    connect(parser, &UrlParser::errorOccurred, this, [this, url, parser](const QString &err) {
        emit logMessage("解析失败: " + url + " 错误: " + err);
        parser->deleteLater();
    });

    // 启动解析
    parser->parse(url);

}

void DownloadManager::addTask(const DownloadTask task) {
    queue->enqueue(task);
}

void DownloadManager::start() {
    queue->startQueue();
}

void DownloadManager::pause() {
    queue->pauseQueue();
}

//QVector<DownloadHistoryItem> DownloadManager::getHistoryTasks() const
//{
//    return history->getHistory();
//}

void DownloadManager::onTaskFinished(const DownloadTask &task) {
    DownloadHistoryItem item;
    item.url = task.url;
    item.filePath = task.savePath;
    item.startTime = task.startTime;
    item.endTime = task.endTime;

    history->addHistory(item);  // 保存到历史记录
    QMutexLocker locker(&mutex);
    doneTasks += 1;
    emit taskProgress(doneTasks, 1.0f * doneTasks / totalTasks);
}

