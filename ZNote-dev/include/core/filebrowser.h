#pragma once

#include "qmarkdowntextedit.h"

#include <QObject>
#include <QFileSystemModel>
#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QPlainTextEdit>
#include <QWebEngineView>


class FileBrowser  : public QObject
{
	Q_OBJECT

public:
	explicit FileBrowser(QObject *parent = nullptr);
	~FileBrowser();

	void setTreeView(QTreeView* view);
	void setWebView(QWebEngineView* view);
	void setTextEdit(QMarkdownTextEdit * edit);

	
	void setRootPath(const QString& path);

	void setNameFilters(const QStringList& filters);

	void setFilter(QDir::Filters filters);

private slots:
	void onDoubleClicked(const QModelIndex& index);
	void onClicked(const QModelIndex& index);
	void openFileInWebView(const QString& filePath);

	void openFileInTextEdit(const QString& filePath);

	void onContextMenuRequested(const QPoint& pos);
	void onActionOpen();
	void onActionOpenDir();
	void onActionDelete();
	void onActionRename();

private:
	bool saveCurrentFile();

	QFileSystemModel* model_;
	QTreeView* treeView_;
	QWebEngineView* webView_;
	QMarkdownTextEdit* textEdit_;
	QMenu* contextMenu_;
	QAction* actionOpen_;
	QAction* actionOpenDir_;
	QAction* actionDelete_;
	QAction* actionRename_;
	QString rootPath_;
	QString rightClickedPath;


	QString currentFilePath_;
};

