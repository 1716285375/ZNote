#include "core/filebrowser.h"
#include "core/fileresolve.h"
#include "qmarkdowntextedit.h"
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTextStream>
#include <QHeaderView>

FileBrowser::FileBrowser(QObject* parent)
	: QObject(parent)
	, treeView_(nullptr)
	, textEdit_(nullptr)
	, textBrowser_(nullptr)
	, isPreviewMode_(false)
{
	model_ = new QFileSystemModel(this);
	model_->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
	model_->setRootPath(QDir::rootPath());

	// ÓÒ¼ü²Ëµ¥
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

FileBrowser::~FileBrowser() {}

void FileBrowser::setTreeView(QTreeView* view)
{
	if (!view) return;
	treeView_ = view;
	treeView_->setModel(model_);
	treeView_->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	treeView_->setContextMenuPolicy(Qt::CustomContextMenu);

	for (int i = 1; i < model_->columnCount(); ++i)
		treeView_->hideColumn(i);

	connect(treeView_, &QTreeView::clicked, this, &FileBrowser::onClicked);
	connect(treeView_, &QTreeView::customContextMenuRequested, this, &FileBrowser::onContextMenuRequested);
}

void FileBrowser::setTextEdit(QMarkdownTextEdit* edit) { textEdit_ = edit; }
void FileBrowser::setTextBrowser(QTextBrowser* browser) { textBrowser_ = browser; }

void FileBrowser::setRootPath(const QString& path)
{
	if (!treeView_ || !model_) return;

	QModelIndex rootIndex = model_->setRootPath(path);
	treeView_->setRootIndex(rootIndex);
}

void FileBrowser::setNameFilters(const QStringList& filters)
{
	model_->setNameFilters(filters);
	model_->setNameFilterDisables(false);
}

void FileBrowser::setDisplayMode(bool preview)
{
	isPreviewMode_ = preview;
	displayCurrentFile();
}

void FileBrowser::displayCurrentFile()
{
	if (currentFilePath_.isEmpty()) return;
	if (isPreviewMode_)
		openFileInTextBrowser(currentFilePath_);
	else
		openFileInTextEdit(currentFilePath_);
}

void FileBrowser::onClicked(const QModelIndex& index)
{
	if (!index.isValid()) return;

	QString path = model_->filePath(index);
	if (model_->isDir(index)) return;

	currentFilePath_ = path;
	displayCurrentFile();
}

void FileBrowser::openFileInTextEdit(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

	QString content = file.readAll();
	if (textEdit_) textEdit_->setPlainText(content);
	currentFilePath_ = filePath;
}

void FileBrowser::openFileInTextBrowser(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

	QString content = file.readAll();
	if (textBrowser_) textBrowser_->setHtml(FileResolve::markdownToHtml(content));
	currentFilePath_ = filePath;
}

void FileBrowser::onContextMenuRequested(const QPoint& pos)
{
	if (!treeView_) return;

	QModelIndex index = treeView_->indexAt(pos);
	if (!index.isValid()) return;

	rightClickedPath_ = model_->filePath(index);
	contextMenu_->exec(treeView_->viewport()->mapToGlobal(pos));
}

void FileBrowser::onActionOpen() { QDesktopServices::openUrl(QUrl::fromLocalFile(rightClickedPath_)); }
void FileBrowser::onActionOpenDir()
{
	QFileInfo info(rightClickedPath_);
	QString dirPath = info.isDir() ? rightClickedPath_ : info.absolutePath();
	QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
}

void FileBrowser::onActionDelete()
{
	QFileInfo info(rightClickedPath_);
	QString name = info.fileName();

	auto reply = QMessageBox::question(nullptr, tr("Delete"),
		tr("Are you sure you want to delete \"%1\"?").arg(name),
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		bool success = false;
		if (info.isDir())
			success = QDir(rightClickedPath_).removeRecursively();
		else
			success = QFile(rightClickedPath_).remove();

		if (!success)
			QMessageBox::warning(nullptr, tr("Error"), tr("Failed to delete \"%1\"").arg(name));
	}
}

void FileBrowser::onActionRename()
{
	QFileInfo info(rightClickedPath_);
	QString oldName = info.fileName();
	bool ok;
	QString newName = QInputDialog::getText(nullptr, tr("Rename"),
		tr("Enter new name:"), QLineEdit::Normal, oldName, &ok);

	if (ok && !newName.isEmpty() && newName != oldName)
	{
		QString newPath = info.absolutePath() + "/" + newName;
		if (QFile::exists(newPath))
		{
			QMessageBox::warning(nullptr, tr("Error"),
				tr("A file or directory with the name \"%1\" already exists.").arg(newName));
			return;
		}
		if (!QFile::rename(rightClickedPath_, newPath))
			QMessageBox::warning(nullptr, tr("Error"),
				tr("Failed to rename \"%1\" to \"%2\"").arg(oldName).arg(newName));
	}
}

bool FileBrowser::saveCurrentFile()
{
	if (currentFilePath_.isEmpty() || !textEdit_) return false;

	QFile file(currentFilePath_);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

	QTextStream out(&file);
	out << textEdit_->toPlainText();
	file.close();
	return true;
}
