#include "core/filebrowser.h"
#include "core/fileresolve.h"

#include <QDesktopServices>
#include <QInputDialog>
#include <QMessageBox>
#include <QUrl>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QHeaderView>


#include <QDebug>

FileBrowser::FileBrowser(QObject *parent)
	: QObject(parent)
	, treeView_(nullptr)
	, webView_(nullptr)
	, textEdit_(nullptr)
{
	model_ = new QFileSystemModel(this);
	model_->setRootPath(QDir::rootPath());
	model_->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

	// 右键菜单
	contextMenu_ = new QMenu;
	actionOpen_ = new QAction(tr("Open"), this);
	actionOpenDir_ = new QAction(tr("Open Containing Folder"), this);
	actionDelete_ = new QAction(tr("Delete"), this);
	actionRename_ = new QAction(tr("Rename"), this);
	
	contextMenu_->addAction(actionOpen_);
	contextMenu_->addAction(actionOpenDir_);
	contextMenu_->addSeparator();
	contextMenu_->addAction(actionRename_);
	contextMenu_->addAction(actionDelete_);

	connect(actionOpen_, &QAction::triggered, this, &FileBrowser::onActionOpen);
	connect(actionOpenDir_, &QAction::triggered, this, &FileBrowser::onActionOpenDir);
	connect(actionDelete_, &QAction::triggered, this, &FileBrowser::onActionDelete);
	connect(actionRename_, &QAction::triggered, this, &FileBrowser::onActionRename);
}

FileBrowser::~FileBrowser()
{

}

void FileBrowser::setTreeView(QTreeView * view)
{
	if (!view)
	{
		qDebug() << "FileBrowser::setTreeView: view is null";
		return;
	}

	treeView_ = view;
	treeView_->setModel(model_);
	treeView_->setRootIndex(model_->index(QDir::homePath()));
	treeView_->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	treeView_->setContextMenuPolicy(Qt::CustomContextMenu);

	for (int i = 1; i < model_->columnCount(); ++i) {
		treeView_->hideColumn(i);
	}

	connect(treeView_, &QTreeView::doubleClicked, this, &FileBrowser::onDoubleClicked);
	connect(treeView_, &QTreeView::clicked, this, &FileBrowser::onClicked);
	connect(treeView_, &QTreeView::customContextMenuRequested, this, &FileBrowser::onContextMenuRequested);
}

void FileBrowser::setWebView(QWebEngineView* view)
{
	if (!view)
	{
		qDebug() << "FileBrowser::setWebView: view is null";
		return;
	}
	webView_ = view;

}

void FileBrowser::setTextEdit(QMarkdownTextEdit* edit)
{
	if (!edit)
	{
		qDebug() << "FileBrowser::setTextEdit: edit is null";
		return;
	}
	textEdit_ = edit;

	connect(textEdit_, &QMarkdownTextEdit::textChanged, this, [this]() {
		// 延迟保存，避免频繁写文件
		QTimer::singleShot(1000, this, [this]() { saveCurrentFile(); });
		});


}

void FileBrowser::setRootPath(const QString& path)
{
	if (!model_)
	{
		qDebug() << "FileBrowser::setRootPath: model is null";
		return;
	}
	
	treeView_->setRootIndex(model_->setRootPath(path));
}

void FileBrowser::setNameFilters(const QStringList& filters)
{
	model_->setNameFilters(filters);
	model_->setNameFilterDisables(false);
}

void FileBrowser::setFilter(QDir::Filters filters)
{
	model_->setFilter(filters);
}

void FileBrowser::onDoubleClicked(const QModelIndex& index)
{
	if (!index.isValid())
	{
		qDebug() << "FileBrowser::onDoubleClicked: index is invalid";
		return;
	}
	
	QString path = model_->filePath(index);

	QDesktopServices::openUrl(QUrl::fromLocalFile(path));

}

void FileBrowser::onClicked(const QModelIndex& index)
{
	// 获取文件或目录的路径
	QString filePath = model_->filePath(index);

	// 检查该路径是否是文件夹
	if (model_->isDir(index)) {
		qDebug() << "Clicked directory:" << filePath;
		// 如果是文件夹，可以展开或做其他操作
	}
	else {
		qDebug() << "Clicked file:" << filePath;
		// 如果是文件，可以在 WebView 中显示或处理文件
		openFileInTextEdit(filePath);
	}
}

void FileBrowser::openFileInWebView(const QString& filePath)
{
	// 使用 QWebEngineView 打开文件
	if (filePath.endsWith(".md", Qt::CaseInsensitive)) {
		// 如果是 Markdown 文件
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QString content = file.readAll();
			QString html = FileResolve::markdownToHtml(content);
			webView_->setHtml(html);
		}
		else {
			qDebug() << "Failed to open file:" << filePath;
		}
	}
}

void FileBrowser::openFileInTextEdit(const QString& filePath)
{
	// 使用 QTextEdit 打开文件
	if (filePath.endsWith(".md", Qt::CaseInsensitive)) {
		// 如果是 Markdown 文件
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QString content = file.readAll();
			QString html = FileResolve::markdownToHtml(content);
			textEdit_->setPlainText(content);
			currentFilePath_ = filePath;        // 保存路径
		}
		else {
			qDebug() << "Failed to open file:" << filePath;
		}
	}
}

void FileBrowser::onContextMenuRequested(const QPoint& pos)
{
	if (!treeView_)
	{
		qDebug() << "FileBrowser::onContextMenuRequested: treeView_ is null";
		return;
	}
		
	QModelIndex index = treeView_->indexAt(pos);
	if (!index.isValid())
	{
		qDebug() << "FileBrowser::onContextMenuRequested: index is invalid";
		return;
	}
		
	rightClickedPath = model_->filePath(index);
	contextMenu_->exec(treeView_->viewport()->mapToGlobal(pos));
}

void FileBrowser::onActionOpen()
{
	if (rightClickedPath.isEmpty())
	{
		qDebug() << "FileBrowser::onActionOpen: rightClickedPath is empty";
		return;
	}
	QDesktopServices::openUrl(QUrl::fromLocalFile(rightClickedPath));
}

void FileBrowser::onActionOpenDir()
{
	if (rightClickedPath.isEmpty())
	{
		qDebug() << "FileBrowser::onActionOpenDir: rightClickedPath is empty";
		return;
	}
	QFileInfo info(rightClickedPath);
	QString dirPath = info.isDir() ? rightClickedPath : info.absolutePath();
	QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
}

void FileBrowser::onActionDelete()
{
	if (rightClickedPath.isEmpty())
	{
		qDebug() << "FileBrowser::onActionDelete: rightClickedPath is empty";
		return;
	}
	QFileInfo info(rightClickedPath);
	QString name = info.fileName();
	auto reply = QMessageBox::question(nullptr, tr("Delete"), tr("Are you sure you want to delete \"%1\"?").arg(name),
		QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		bool success = false;
		if (info.isDir())
		{
			QDir dir(rightClickedPath);
			success = dir.removeRecursively();
		}
		else
		{
			QFile file(rightClickedPath);
			success = file.remove();
		}
		if (!success)
		{
			QMessageBox::warning(nullptr, tr("Error"), tr("Failed to delete \"%1\"").arg(name));
		}
	}
}

void FileBrowser::onActionRename()
{
	if (rightClickedPath.isEmpty())
	{
		qDebug() << "FileBrowser::onActionRename: rightClickedPath is empty";
		return;
	}
	QFileInfo info(rightClickedPath);
	QString oldName = info.fileName();
	bool ok;
	QString newName = QInputDialog::getText(nullptr, tr("Rename"), tr("Enter new name:"), QLineEdit::Normal, oldName, &ok);
	if (ok && !newName.isEmpty() && newName != oldName)
	{
		QString newPath = info.absolutePath() + "/" + newName;
		if (QFile::exists(newPath))
		{
			QMessageBox::warning(nullptr, tr("Error"), tr("A file or directory with the name \"%1\" already exists.").arg(newName));
			return;
		}
		bool success = QFile::rename(rightClickedPath, newPath);
		if (!success)
		{
			QMessageBox::warning(nullptr, tr("Error"), tr("Failed to rename \"%1\" to \"%2\"").arg(oldName).arg(newName));
		}
	}
}

bool FileBrowser::saveCurrentFile()
{
	if (currentFilePath_.isEmpty())
	{
		qDebug() << "currentFilePath_ is empty";
		return false;
	}

	QFile file(currentFilePath_);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Failed to save file:" << currentFilePath_;
		return false;
	}

	QString content = textEdit_->toPlainText(); // 或 toMarkdown() 如果是 QMarkdownTextEdit
	QTextStream out(&file);
	out << content;
	file.close();
	return true;
}
