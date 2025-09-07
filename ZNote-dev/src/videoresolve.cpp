#include "videoresolve.h"
#include "misc.h"

#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


ResolveTask::ResolveTask(ParsedEntry *entry, QObject* prarent)
	: QObject(prarent)
	, entry_(entry)
{

}

void ResolveTask::start()
{
	qDebug() << "Start method invoked";  // 确保这一行被执行
	resolve(*entry_);
}

void ResolveTask::resolve(const ParsedEntry &entry)
{
	auto* process = new QProcess(this);

	process->start("yt-dlp.exe", znote::utils::buildCommand(entry.url, CommandType::Resolve));

	if (!process->waitForStarted()) {
		qDebug() << "Failed to start yt-dlp process.";
		emit resolvedError("Failed to start yt-dlp process.");
		return;
	}

	connect(process, &QProcess::readyReadStandardOutput, this, [this, process, entry]() {

		QByteArray buffer = process->readAllStandardOutput();

		//qDebug() << "Output from yt-dlp:" << QString::fromUtf8(buffer);

		QJsonParseError err;
		QJsonDocument doc = QJsonDocument::fromJson(buffer, &err);

		if (err.error != QJsonParseError::NoError) {
			emit resolvedError("JSON parse error: " + err.errorString());
			return;
		}

		if (!doc.isObject()) {
			emit resolvedError("Invalid JSON from yt-dlp");
			return;
		}

		QJsonObject root = doc.object();

		// 解析视频的标题、URL、格式、大小等信息
		QString videoTitle = root.value("title").toString();
		QString videoUrl = root.value("webpage_url").toString();
		QString videoId = root.value("id").toString();
		int filesize = root.value("filesize_approx").toInt();

		// 创建 DownloadTask
		DownloadTask task;
		task.url = videoUrl;
		task.playlistTitle = entry.playlistTitle;
		//task.savePath = entry.
		task.type = entry.type;
		task.playlistCount = entry.playlistCount;

		// 填充 VideoEntry
		VideoEntry videoEntry;
		videoEntry.title = videoTitle;
		videoEntry.url = videoUrl;

		// 解析格式列表
		QJsonArray formats = root.value("formats").toArray();
		QVector<VideoFormat> videoFormats;
		for (const QJsonValue& v : formats) {
			QJsonObject format = v.toObject();

			// 创建 VideoFormat 并填充
			VideoFormat videoFormat;
			videoFormat.formatId = format["format"].toString();
			videoFormat.ext = format["ext"].toString();
			videoFormat.vcodec = format["vcodec"].toString();
			videoFormat.acodec = format["acodec"].toString();
			videoFormat.width = format["width"].toInt();
			videoFormat.height = format["height"].toInt();
			videoFormat.filesize = format["filesize_approx"].toInt();

			videoFormats.append(videoFormat);
		}

		// 将视频格式添加到 VideoEntry
		videoEntry.videoFormats = videoFormats;

		// 将 VideoEntry 添加到 DownloadTask
		task.video = videoEntry;

		// 最后发出 resolved 信号，返回 DownloadTask
		emit entryResolved(task);
		
		// 在此处可以进一步处理获取到的数据，如传递回 UI 层
		qDebug() << "Video Title:" << videoTitle;
		qDebug() << "Video URL:" << videoUrl;
	});

	connect(process, &QProcess::finished, process, &QProcess::deleteLater);  // 进程结束后清理
}
