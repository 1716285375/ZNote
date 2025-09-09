#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QDateTime>
#include <QMetaType> // 必须包含

enum class UrlType
{
	Unknown = -1,
	Lists = 0,
	Single
};

enum class DownloadStatus
{
	Success = 0,
	Failed,
	Canceled
};

struct DownloadHistoryItem
{
	QString vid;				// 唯一标识符
	QString title;
	int index = 1;			// 当前集数
	int playlistCount = 1;		// 总集数
	UrlType type;
	QString savePath;
	QDateTime startTime;
	QDateTime endTime;
	DownloadStatus status; // success, failed, canceled
};


struct VideoFormat {
	QString formatId;
	QString ext;
	QString vcodec;
	QString acodec;
	int width = 0;
	int height = 0;
	qint64 filesize = 0;
};

struct VideoEntry {
	QString title;
	QString url;
	QString playlistTitle;
	QString ext = "mp4";
	QVector<VideoFormat> videoFormats;
	QMap<QString, QString> subtitles; // lang -> url
};

struct DownloadTask
{
	QString id;				// 唯一标识符
	int index = 1;			// 当前集数
	int playlistCount = 1;		// 总集数
	UrlType type;
	VideoEntry video;
	QString savePath;
	QDateTime resolveTime;
	QDateTime startTime;
	QDateTime endTime;

	// 用于视频下载列表视图代理
	bool isSelected = false;
	bool isFinished = false;
};


struct ParsedEntry
{
	QString id;
	QString url;
	QString playlistTitle;
	UrlType type;
	int index = 1;
	int playlistCount;
};

// 在结构体定义之后，添加 Q_DECLARE_METATYPE
Q_DECLARE_METATYPE(DownloadTask)


#endif // TASK_H
