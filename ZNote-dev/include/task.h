#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QDateTime>
#include <QMetaType> // 必须包含

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
	QVector<VideoFormat> videoFormats;
	QVector<VideoFormat> audioFormats;
	QMap<QString, QString> subtitles; // lang -> url
};

struct DownloadTask
{
	QString url;
	int index = 0;
	QString savePath;
	QString playlistTitle;
	QString resolution;
	QString audioFormat;
	QDateTime startTime;
	QDateTime endTime;
	bool subtitles = false;
	VideoEntry video;
};
// 在结构体定义之后，添加 Q_DECLARE_METATYPE
Q_DECLARE_METATYPE(VideoFormat)
Q_DECLARE_METATYPE(VideoEntry)
Q_DECLARE_METATYPE(DownloadTask)


#endif // TASK_H
