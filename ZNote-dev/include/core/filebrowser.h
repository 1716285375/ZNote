#pragma once
#include <QObject>
#include <QFileSystemModel>
#include <QTreeView>
#include <QTextBrowser>
#include <QMenu>
#include "qmarkdowntextedit.h"

class FileBrowser : public QObject
{
	Q_OBJECT
public:
	explicit FileBrowser(QObject* parent = nullptr);
	~FileBrowser();

	void setTreeView(QTreeView* view);
	void setTextEdit(QMarkdownTextEdit* edit);
	void setTextBrowser(QTextBrowser* browser);

	void setRootPath(const QString& path);
	void setNameFilters(const QStringList& filters);
	void setDisplayMode(bool preview);

	bool saveCurrentFile();

private slots:
	void onClicked(const QModelIndex& index);
	void onContextMenuRequested(const QPoint& pos);
	void onActionOpen();
	void onActionOpenDir();
	void onActionDelete();
	void onActionRename();

private:
	void displayCurrentFile();
	void openFileInTextEdit(const QString& filePath);
	void openFileInTextBrowser(const QString& filePath);

private:
	QTreeView* treeView_;
	QMarkdownTextEdit* textEdit_;
	QTextBrowser* textBrowser_;
	QFileSystemModel* model_;
	QMenu* contextMenu_;
	QAction* actionOpen_;
	QAction* actionOpenDir_;
	QAction* actionDelete_;
	QAction* actionRename_;

	QString currentFilePath_;
	QString rightClickedPath_;
	bool isPreviewMode_;
};
