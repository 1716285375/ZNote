#pragma once


#include <QObject>
#include "task.h"

class ResolveTask : public QObject
{
	Q_OBJECT
public:
	explicit ResolveTask(ParsedEntry *entry, QObject *prarent = nullptr);

	void start();

signals:
	void entryResolved(DownloadTask task);
	void resolvedError(const QString &err);

private:
	void resolve(const ParsedEntry& entry);
private:

	ParsedEntry *entry_;

};