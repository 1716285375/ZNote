#include "mainwindow.h"
#include "downloadmanager.h"
#include "ui_mainwindow.h"
#include "misc.h"

#include <QStringList>
#include <QIcon>
#include <QFileDialog>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QStandardItemModel>
#include <QAbstractItemView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , btngLeft(new QButtonGroup(this))
    , btngNote(new QButtonGroup(this))
    , manager(new DownloadManager(this))
{
    ui->setupUi(this);
    this->init();
    this->setWindowIcon(QIcon(":/app/logo.ico"));
    this->setWindowTitle(QString("ZNote"));

    this->setupConnection();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnection()
{
    connect(ui->btnDownload, &QPushButton::clicked, manager, &DownloadManager::start);
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

    connect(ui->edtSaveDir, &QLineEdit::textChanged, this, [this](const QString &text) {
        // 定义非法字符 (Windows 文件/目录名限制)
        if (znote::utils::isValidPath(text)) {
            lastValidDir = text; // 保存最新合法输入
        }
        else {
            // 恢复上次合法输
            QSignalBlocker blocker(ui->edtSaveDir);
            ui->edtSaveDir->setText(lastValidDir);

            QMessageBox::warning(this, tr("非法目录"), tr("输入的目录名包含非法字符或格式！"));
        }

    });

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

    connect(ui->btnNoteEdit, &QPushButton::clicked, this, [this]() {
        ui->stwNote->setCurrentWidget(ui->pageNoteEdit);
    });

    connect(ui->btnNoteSplit, &QPushButton::clicked, this, [this]() {
        ui->stwNote->setCurrentWidget(ui->pageNoteSplit);
    });

    connect(ui->btnNotePreview, &QPushButton::clicked, this, [this]() {
        ui->stwNote->setCurrentWidget(ui->pageNotePreview);
    });

}

void MainWindow::init()
{
    btngLeft->addButton(ui->btnResolve);
    btngLeft->addButton(ui->btnDownloadList);
    btngLeft->addButton(ui->btnDownloadStatus);
    btngLeft->addButton(ui->btnNote);
    btngLeft->addButton(ui->btnVideo);
    btngLeft->addButton(ui->btnSetting);
    btngLeft->addButton(ui->btnAbout);

    btngNote->addButton(ui->btnNoteEdit);
    btngNote->addButton(ui->btnNoteSplit);
    btngNote->addButton(ui->btnNotePreview);

    // 设置按钮组为互斥模式
    btngLeft->setExclusive(true);

    lastValidDir = QString("C:\\C\\Qt\\ZNote\\tmp");
    ui->edtSaveDir->setText(lastValidDir);

    // 下载列表
    QStandardItemModel *modelDownloadList = new QStandardItemModel(this);
    modelDownloadList->setColumnCount(5);
    modelDownloadList->setHorizontalHeaderLabels({"选择", "标题", "分辨率", "音频", "字幕"});
    ui->tblDownloadList->setModel(modelDownloadList);
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


