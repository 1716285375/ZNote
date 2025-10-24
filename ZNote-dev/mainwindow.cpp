#include "mainwindow.h"
#include "downloadmanager.h"
#include "ui_mainwindow.h"
#include "misc.h"
#include "configmanager.h"
#include "component/checkboxdelegate.h"
#include "historymanager.h"

#include <QOverload>
#include <QStringList>
#include <QIcon>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , btngLeft(new QButtonGroup(this))
    , btngNote(new QButtonGroup(this))
    , chkDownload(new QButtonGroup(this))
    , manager(new DownloadManager(this))
	, config(ConfigManager::instance())
    , fileBrowser(new FileBrowser(this))
    , model_(new VideoModel(this))
    , historyModel_(new HistoryModel(this))
{
    ui->setupUi(this);

    this->init();

    this->setWindowTitle(QString("ZNote"));

    this->setupConnection();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnection()
{
    connect(ui->btnDownload, &QPushButton::clicked, this, &MainWindow::startDownload);
    connect(ui->btnPause, &QPushButton::clicked, manager, &DownloadManager::pause);

    connect(manager, &DownloadManager::logMessage, this, [this](const QString &msg){
        QString line = QDateTime::currentDateTime().toString("[hh:mm:ss] ") + msg;
        ui->tbwLog->append(line);
    });

    connect(manager, &DownloadManager::taskProgress, this, [this](const int done, const float percent){
        QString p =  QString("%1%").arg(QString::number(qRound(percent * 100)));
        ui->lblProgress->setText(p);
        ui->pbarDownload->setValue(int(100 * percent));
    });

    connect(manager, &DownloadManager::taskReady, this, &MainWindow::onTaskReady);
    connect(manager, &DownloadManager::taskFinished, this, &MainWindow::onTaskFinished);


    connect(ui->chkSelectAll, &QCheckBox::clicked, this, [this](bool checked) {
        //qDebug() << checked;

        if (checked) {
              this->selectAll();
        }
        else
        {
            this->cancelAll();
        }

	});

    connect(ui->edtSaveDir, &QLineEdit::textChanged, this, [this](const QString &text) {
        // 定义非法字符 (Windows 文件/目录名限制)
        if (znote::utils::isValidPath(text))
        {
            config.setValue("download.defaultPath", text);
        }
        else
        {
            // 恢复上次合法输
            QSignalBlocker blocker(ui->edtSaveDir);
            ui->edtSaveDir->setText(config.getValue("download.defaultPath").toString());
            QMessageBox::warning(this, tr("非法目录"), tr("输入的目录名包含非法字符或格式！"));
        }

    });

    connect(ui->btnClearHistory, &QPushButton::clicked, this, &MainWindow::clearHistory);

    connect(ui->btnBrowseDir, &QPushButton::clicked, this, [this]() {
        QString saveDir = QFileDialog::getExistingDirectory(this, "Select Save Directory");
        if (saveDir.isEmpty())
        {
            return;
        }

        ui->edtSaveDir->setText(saveDir);
    });

    connect(ui->btnResolve, &QPushButton::clicked, this, [this]() {
        ui->stwMain->setCurrentWidget(ui->pageResolve);
    });

    connect(ui->btnDownloadList, &QPushButton::clicked, this, [this]() {
        ui->stwMain->setCurrentWidget(ui->pageDownloadList);
    });

    connect(ui->btnDownloadStatus, &QPushButton::clicked, this, [this]() {
        ui->stwMain->setCurrentWidget(ui->pageDownloadStatus);
    });

    connect(ui->btnNote, &QPushButton::clicked, this, [this]() {
        ui->stwMain->setCurrentWidget(ui->pageNote);
    });

    connect(ui->btnVideo, &QPushButton::clicked, this, [this]() {
        ui->stwMain->setCurrentWidget(ui->pageVideo);
    });

    connect(ui->btnSetting, &QPushButton::clicked, this, [this]() {
        ui->stwMain->setCurrentWidget(ui->pageSetting);
    });

    connect(ui->btnAbout, &QPushButton::clicked, this, [this]() {
        ui->stwMain->setCurrentWidget(ui->pageAbout);
    });

    connect(fileBrowser, &FileBrowser::onFileChanged, this, [this](QString file) {
		QFileInfo f(file);

        ui->lblNoteFilename->setText(ui->lblNoteFilename->text().split(":")[0] + ": " + f.baseName().left(10) + " ..." + f.fileName().right(10));
        ui->lblNoteFilename->setToolTip(f.baseName());
    });

    connect(ui->btnOpenNoteDir, &QPushButton::clicked, this, [this]() {
		QString rootPath = QFileDialog::getExistingDirectory(this, "Select Note Directory");
		if (rootPath.isEmpty())
		{
			return;
		}
        QFileInfo f(rootPath);
        QString dirPath = f.absolutePath();

        ui->edtNoteDir->setText(dirPath);
        fileBrowser->setRootPath(rootPath);
    });

    connect(ui->btnNoteEdit, &QPushButton::clicked, this, [this]() {
        ui->stwNote->setCurrentWidget(ui->pageNoteEdit);
        fileBrowser->setDisplayMode(false);
    });

    connect(ui->btnNotePreview, &QPushButton::clicked, this, [this]() {
        ui->stwNote->setCurrentWidget(ui->pageNotePreview);
        fileBrowser->setDisplayMode(true);

    });

    connect(ui->cmbThreads, QOverload<int>::of(&QComboBox::currentIndexChanged), &config, [this](int index) {
		int threads = ui->cmbThreads->currentIndex() + 1; // index 从 0 开始
        config.setValue("download.threadCount", threads);
        config.save();
		qDebug() << "Set thread count to" << threads;
	});

    connect(ui->btnSaveSetting, &QPushButton::clicked, this, [this]() {
        config.save();
    });


}

void MainWindow::init()
{
	this->initConfig();

	this->initUI();

    this->initData();
}

void MainWindow::on_btnDownload_clicked()
{

}

void MainWindow::on_btnCrap_clicked()
{
    QString url = ui->edtUrl->text();
    if (url.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please enter a video URL");
        return;
    }

    QString saveDir = ui->edtSaveDir->text();
    if (saveDir.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please choose a output Directory");
        return;
    }

    manager->parseUrl(url, saveDir);
}

void MainWindow::appendLog(const QString &msg)
{
    ui->tbwLog->append(msg);
}

void MainWindow::onDownloadFinished(bool success)
{
    if (success)
    {
        QMessageBox::information(this, "Done", "Download completed successfully!");
    }
    else
    {
        QMessageBox::critical(this, "Failed", "Download failed");
    }
}

void MainWindow::onCrapFinished(QStringList videos)
{
}

void MainWindow::checkState()
{
    // TODO: 检查执行前的状态
}

void MainWindow::initConfig()
{
	// 可执行文件目录下的 config.json
	QString configPath = QDir(QCoreApplication::applicationDirPath())
		.absoluteFilePath("config.json");

	QFile file(configPath);
	if (!file.exists())
	{
        qDebug() << "config.json not found! Creating default config...";

		// 默认下载设置
		config.setValue("download.defaultPath", "C:/Users/Jie/Downloads/ZNote");
		config.setValue("download.threadCount", 4);
		config.setValue("download.filePrefix", "ZN_");
		config.setValue("download.fileSuffix", "_video");
		config.setValue("download.onComplete.playSound", true);
		config.setValue("download.onComplete.autoOpenDir", false);

		if (config.save())
			qDebug() << "Default config.json created at:" << configPath;
		else
			qDebug() << "Failed to create default config.json!";
	}
	else
	{
		config.setFilePath(configPath); // 安全加载，内部 load() 会加锁

		qDebug() << "Loaded config from:" << configPath;
	}

	// 读取文件默认保存路径
	QString defaultPath = config.getValue("download.defaultPath", "C:/Users/Jie/Downloads/ZNote").toString();
    qDebug() << "Download default save dir: " << defaultPath;
    ui->edtSaveDir->setText(defaultPath);

	// 读取下载线程数作为示例
	int threadCount = config.getValue("download.threadCount", 4).toInt();
	qDebug() << "Download threads: " << threadCount;
	ui->cmbThreads->setCurrentIndex(threadCount - 1);

	// 可根据实际需要加载更多配置
}



void MainWindow::initUI()
{
	btngLeft->addButton(ui->btnResolve);
	btngLeft->addButton(ui->btnDownloadList);
	btngLeft->addButton(ui->btnDownloadStatus);
	btngLeft->addButton(ui->btnNote);
	btngLeft->addButton(ui->btnVideo);
	btngLeft->addButton(ui->btnSetting);
	btngLeft->addButton(ui->btnAbout);

	btngNote->addButton(ui->btnNoteEdit);
	btngNote->addButton(ui->btnNotePreview);

	// 设置按钮组为互斥模式
	btngLeft->setExclusive(true);

	ui->edtSaveDir->setText(config.getValue("download.defaultPath").toString());

	// 下载列表

	ui->tblDownloadList->setModel(model_);

	CheckBoxDelegate* chkDelegate = new CheckBoxDelegate(this);

	ui->tblDownloadList->setItemDelegateForColumn(6, chkDelegate);
	ui->tblDownloadList->resizeColumnsToContents();


    // 历史下载记录
    ui->tblDownloadHistory->setModel(historyModel_);
    ui->tblDownloadHistory->resizeColumnsToContents();

    // 设置
    ui->tabSetting->setTabText(0, tr("下载设置"));

    // markdown edit
    ui->tedtNotePreview->setReadOnly(true);
    ui->stwNote->setCurrentWidget(ui->pageNoteEdit);

    ui->edtNoteDir->setText(config.getValue("download.defaultPath").toString());

	// 文件浏览器
	ui->tvwNoteFile->setHeaderHidden(true);         // 隐藏表头，像 IDE 文件树一样
	ui->tvwNoteFile->setAnimated(true);             // 展开/折叠有动画
	ui->tvwNoteFile->setIndentation(18);            // 缩进大小
	ui->tvwNoteFile->setExpandsOnDoubleClick(true); // 双击目录展开

	fileBrowser->setTreeView(ui->tvwNoteFile);
    fileBrowser->setRootPath(config.getValue("download.defaultPath").toString());
    fileBrowser->setNameFilters({ "*.txt", "*.md" }); // 可选 
    fileBrowser->setTextEdit(ui->tedtNote);
    fileBrowser->setTextBrowser(ui->tedtNotePreview);

}

void MainWindow::initData()
{
    QList<DownloadHistoryItem> historyItems = manager->getHistory();
    if (historyItems.isEmpty()) qDebug() << "null";
    for (const auto& item : historyItems) {
        //znote::utils::printDownloadHistoryItem(item);
        historyModel_->addhistory(item);
    }
}

void MainWindow::onTaskReady(const DownloadTask& task)
{
    //znote::utils::printDownloadTask(task);
    model_->addTask(task);
}

void MainWindow::onTaskFinished(const DownloadTask& task)
{
	DownloadHistoryItem item;
	item.vid = task.id;
	item.title = task.video.title;
	item.index = task.index;
	item.playlistCount = task.playlistCount;
	item.savePath = task.savePath;
	item.type = task.type;
	item.startTime = task.startTime;
	item.endTime = task.endTime;
	item.status = DownloadStatus::Success;

	this->manager->addHistory(item);  // 保存到历史记录

    historyModel_->addhistory(item);
}

void MainWindow::selectAll()
{
	// 获取模型
	VideoModel* model = qobject_cast<VideoModel*>(ui->tblDownloadList->model());

	// 遍历模型中的所有行，设置复选框为勾选状态
	for (int row = 0; row < model->rowCount(); ++row) {
		QModelIndex index = model->index(row, 6);  // 获取第6列（复选框列）的索引
		model->setData(index, Qt::Checked, Qt::CheckStateRole);  // 设置复选框为勾选状态
	}
}

void MainWindow::cancelAll()
{
	// 获取模型
	VideoModel* model = qobject_cast<VideoModel*>(ui->tblDownloadList->model());

	// 遍历模型中的所有行，设置复选框为未勾选状态
	for (int row = 0; row < model->rowCount(); ++row) {
		QModelIndex index = model->index(row, 6);  // 获取第6列（复选框列）的索引
		model->setData(index, Qt::Unchecked, Qt::CheckStateRole);  // 设置复选框为未勾选状态
	}
}

void MainWindow::clearHistory()
{
    QList<DownloadHistoryItem> historyItems;
    QList<int> selectedRows;  // 存储选中的行号
    HistoryModel* model = qobject_cast<HistoryModel*>(ui->tblDownloadHistory->model());

    for (int row = 0; row < model->rowCount(); ++row) {
        
        DownloadHistoryItem item = model->getHistortyItems().at(row);
        historyItems.append(item);
        selectedRows.append(row);
    }
    historyModel_->removehistorys(selectedRows);
    this->manager->removeHistory(historyItems);
}

void MainWindow::startDownload()
{
    QList<DownloadTask> downloadTasks;
    QList<int> selectedRows;  // 存储选中的行号
	VideoModel* model = qobject_cast<VideoModel*>(ui->tblDownloadList->model());

	for (int row = 0; row < model->rowCount(); ++row) {
		QModelIndex index = model->index(row, 6);  // 获取第6列（复选框列）的索引

		// 获取勾选状态，正确的方式是直接使用 Qt::CheckState
		Qt::CheckState checkState = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());


		if (checkState == Qt::Checked) {
			// 如果该行的复选框被勾选
			qDebug() << "Row " << row << " is selected.";

			DownloadTask* task = model->getTasks().at(row);  // 获取模型中对应行的 DownloadTask
			downloadTasks.append(*task);

			// 将勾选行加入到删除列表
			selectedRows.append(row);
           }
	}

	// 批量删除勾选的行
	model->removeTasks(selectedRows);

    this->manager->addTasks(downloadTasks);
    this->manager->start();
    ui->chkSelectAll->setChecked(false);
}

