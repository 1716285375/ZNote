#ifndef MISC_H
#define MISC_H

#include <QString>
#include <QVector>
#include <QStringList>

#include "task.h"

enum class CommandType {
    None = -1,
    Download = 0,
    Resolve,
    ExtractPlaylist


};

namespace znote::utils {
    QString generateRandomString(int length);

    bool isValidPath(const QString &path);

    QStringList buildCommand(const QString &url, CommandType type);
    QStringList buildResolveCommand(const QString& url);
    QStringList buildDownloadCommand(const QString& url);
    QStringList buildExtractPlaylistCommand(const QString& url);

    QStringList buildDownloadCommand(const DownloadTask& task);


    // Debug

    void printCommand(const QStringList& args);

    void printParsedEntry(const ParsedEntry& entry);
    void printParsedEntries(const QList<ParsedEntry> &entries);

    void printVideoFormat(const VideoFormat& format);
    void printVideoEntry(const VideoEntry& entry);
    void printDownloadTask(const DownloadTask& task);

    void printDownloadHistoryItem(const DownloadHistoryItem& item);
}


#endif // MISC_H