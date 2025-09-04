#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>

struct VideoFormat {
    QString formatId;
    QString ext;
    QString vcodec;
    QString acodec;
    int width;
    int height;
    qint64 filesize;
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
    int index;
    QString savePath;
    QString playlistTitle;
    QString resolution;
    QString audioFormat;
    QDateTime startTime;
    QDateTime endTime;
    bool subtitles;
    VideoEntry video;
};



#endif // TASK_H
