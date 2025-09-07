#pragma once

#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QQueue>
#include <QObject>


class ResolveTask;
struct ParsedEntry;
struct DownloadTask;

class ResolveThreadPool : public QObject
{
	Q_OBJECT
public:
	ResolveThreadPool(int maxThreads = 4, QObject *parent = nullptr);

	~ResolveThreadPool();

	void addResolveTask(const ParsedEntry &entry);

signals:
	void taskFinished(const DownloadTask &task);
	void taskError(const QString& err);
private slots:
	void onTaskFinished(const DownloadTask& task);
	void onTaskError(const QString& error);
private:
	void startNextTask();

private:
	int maxThreads_;
	int activeThreads_ = 0;
	QMutex mutex_;
	QQueue<ParsedEntry*> taskQueue_;
	QList<QThread*> threads_;
};