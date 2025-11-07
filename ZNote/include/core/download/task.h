#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QDateTime>
#include <QMetaType>

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
	QString vid;				
	QString title;
	int index = 1;			
	int playlistCount = 1;		
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
	QString formatId;  // 格式ID，用于下载时指定格式
	QVector<VideoFormat> videoFormats;
	QMap<QString, QString> subtitles; // lang -> url
};

struct DownloadTask
{
	QString id;				
	int index = 1;			
	int playlistCount = 1;	
	UrlType type = UrlType::Unknown;
	VideoEntry video;
	QString savePath;
	QDateTime resolveTime;
	QDateTime startTime;
	QDateTime endTime;


	bool isSelected = false;
	bool isFinished = false;
};


struct ParsedEntry
{
	QString id;
	QString vid;              // 视频ID（与id相同，用于兼容）
	QString url;
	QString title;
	QString playlistTitle;
	UrlType type;
	int index = 1;
	int playlistCount;
	int duration = 0;          // 视频时长（秒）
	QString thumbnail;        // 缩略图URL
	QString formatId;         // 格式ID
	QString ext;              // 文件扩展名
};


Q_DECLARE_METATYPE(DownloadTask)


#endif // TASK_H
