#ifndef MISC_H
#define MISC_H

#include <QString>
#include <QVector>

class VideoEntry;
class DownloadTask;

namespace znote::utils {
    QString generateRandomString(int length);
    bool isValidPath(const QString &path);
    QVector<VideoEntry> parseVideoMetadata(const QByteArray &jsonData);
    QString buildDownloadCommand(const DownloadTask &task);

}


#endif // MISC_H
