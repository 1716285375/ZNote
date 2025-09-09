#include "resolvethreadpool.h"
#include "videoresolve.h"
#include "misc.h"

#include <QThread>


//#include <QDebug>

ResolveThreadPool::ResolveThreadPool(int maxThreads, QObject* parent)
	: QObject(parent), maxThreads_(maxThreads)
{
}

ResolveThreadPool::~ResolveThreadPool()
{
	// 确保所有线程被清理
	for (auto* thread : threads_)
	{
		thread->quit();
		thread->wait();
		delete thread;
	}
}

void ResolveThreadPool::addResolveTask(const ParsedEntry& entry)
{
	// 将任务添加到队列
	QMutexLocker locker(&mutex_);
	ParsedEntry* entryPtr = new ParsedEntry(entry);
	taskQueue_.enqueue(entryPtr);  // 直接使用原始指针

	// 如果当前线程数小于最大线程数，尝试启动任务
	if (activeThreads_ < maxThreads_) {
		startNextTask();
	}
}

void ResolveThreadPool::startNextTask()
{
	if (!taskQueue_.isEmpty()) {
		// 获取下一个任务
		ParsedEntry* entryPtr = taskQueue_.dequeue();
		ResolveTask* resolveTask = new ResolveTask(entryPtr);

		//qDebug() << "startNextTask: ";
		//znote::utils::printParsedEntry(*entryPtr);

		// 创建一个新的线程来处理该任务
		QThread* thread = new QThread;

		// 将 ResolveTask 移动到新的线程
		resolveTask->moveToThread(thread);

		// 将线程添加到线程列表中
		threads_.append(thread);
		activeThreads_++;  // 增加活动线程数

		// 连接线程的 started 信号到 ResolveTask 的 start 方法
		connect(thread, &QThread::started, resolveTask, &ResolveTask::start);

		// 线程结束后，自动删除自己
		connect(thread, &QThread::finished, thread, &QThread::deleteLater);

		// 连接 ResolveTask 完成后的信号，回调线程池中的任务完成处理函数
		connect(resolveTask, &ResolveTask::entryResolved, this, &ResolveThreadPool::onTaskFinished);

		// 连接任务发生错误时的信号，处理错误
		connect(resolveTask, &ResolveTask::resolvedError, this, &ResolveThreadPool::onTaskError);


		// 启动线程
		//qDebug() << "Starting thread";
		thread->start(); // 启动线程
		//qDebug() << "Thread started"; // 确保线程启动

	}
}

void ResolveThreadPool::onTaskFinished(const DownloadTask& task)
{
	// 任务完成，减少活动线程数
	activeThreads_--;

	emit taskFinished(task);
	// 启动下一个任务（如果有）
	startNextTask();
}

void ResolveThreadPool::onTaskError(const QString& err)
{
	// 任务发生错误，减少活动线程数
	activeThreads_--;

	qDebug() << "Task Error: " << err;
	emit taskError(err);
	// 启动下一个任务（如果有）
	startNextTask();
}
