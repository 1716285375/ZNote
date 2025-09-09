#include "downloadmanager.h"
#include "videodownloader.h"
#include "historymanager.h"
#include "configmanager.h"
#include "resolvethreadpool.h"
#include "misc.h"

#include <QMessageBox>
#include <QDebug>

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
    , history(new HistoryManager(HistoryStorageType::SQLite, "history.db", this))
{
    //queue = new TaskQueue(4, this);
    queue = new TaskQueue(ConfigManager::instance().getValue("download.threadCount", 4).toInt(), this);
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
		QMutexLocker locker(&mutex);
		totalTasks += entries.size();

		QMessageBox::information(
			nullptr,
			tr("解析情况"),
			tr("本次成功解析了 %1 个视频").arg(entries.size())
		);

		// 更新任务进度
		emit taskProgress(doneTasks, 1.0f * doneTasks / totalTasks);

        //qDebug() << "download.threadCount: " << ConfigManager::instance().getValue("download.threadCount").toInt();

        ResolveThreadPool* resolvePool = new ResolveThreadPool(ConfigManager::instance().getValue("download.threadCount").toInt(), this);
        connect(resolvePool, &ResolveThreadPool::taskFinished, [this](const DownloadTask &task) {

            emit taskReady(task);
            
        });

		connect(resolvePool, &ResolveThreadPool::taskError, [this](const QString &err) {
            qDebug() << "task resolve error: " << err;
            emit taskError(err);
		});

        for (const ParsedEntry &entry : entries) {
			resolvePool->addResolveTask(entry);
			//qDebug() << "resolvePool->addResolveTask(entry) ";
			//znote::utils::printParsedEntry(entry);
        }
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

void DownloadManager::addTask(const DownloadTask& task) {
    queue->enqueue(task);
}

void DownloadManager::addTasks(const QList<DownloadTask>& tasks)
{
    for (const auto& t : tasks) {
        znote::utils::printDownloadTask(t);
        addTask(t);
    }
}

void DownloadManager::start() {

    queue->startQueue();
}

void DownloadManager::pause() {
    queue->pauseQueue();
}


QList<DownloadHistoryItem> DownloadManager::getHistory() const
{
	return history->getHistory();
}

void DownloadManager::addHistory(const DownloadHistoryItem& task)
{
    history->addHistory(task);
}

bool DownloadManager::removeHistory(const QList<DownloadHistoryItem> &items)
{
    try {
		for (const auto& item : items) {
			history->removeHistory(item);
		}

		return true;
    } catch(const std::exception& e) {
		qWarning() << "Exception occurred:" << e.what();  // 打印异常信息
        return false;
    } catch (...) {
		qWarning() << "Unknown exception occurred.";  // 捕获其他未知异常
		return false;
	}

}

void DownloadManager::onTaskFinished(const DownloadTask &task) {

    QMutexLocker locker(&mutex);
    doneTasks += 1;
    emit taskFinished(task);
    emit taskProgress(doneTasks, 1.0f * doneTasks / totalTasks);
}

