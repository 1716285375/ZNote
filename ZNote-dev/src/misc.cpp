#include "misc.h"
#include "task.h"
#include "configmanager.h"

#include <QRandomGenerator>
#include <QString>
#include <QChar>
#include <QRegularExpression>
#include <QFileInfo>
#include <QDir>

#include <QDebug>

namespace znote::utils {
    QString generateRandomString(int length) {
        const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
        QString randomString;
        for(int i=0; i<length; ++i) {
            int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
            QChar nextChar = possibleCharacters.at(index);
            randomString.append(nextChar);
        }
        return randomString;
    }

    bool isValidPath(const QString &path)
    {
        if (path.isEmpty() || path.length() > 240) return false;

#ifdef Q_OS_WIN
        // 驱动器号开头
        QRegularExpression drivePattern("^[a-zA-Z]:[\\\\/]");
        if (!drivePattern.match(path).hasMatch()) return false;

        // 合法字符
        QRegularExpression allowed(R"(^[a-zA-Z0-9_\-./\\\x{4e00}-\x{9fa5} :]+$)");
        if (!allowed.match(path).hasMatch()) return false;

        // 禁止字符
        QRegularExpression invalid(R"([<>|?*])");
        if (invalid.match(path).hasMatch()) return false;
#else
        QRegularExpression allowed(R"(^[a-zA-Z0-9_\-./\x{4e00}-\x{9fa5} :]+$)");
        if (!allowed.match(path).hasMatch()) return false;
#endif

        return true;
    }

    QStringList buildCommand(const QString& url, CommandType type)
    {
        switch (type)
        {
            case CommandType::Resolve:
            {
                return buildResolveCommand(url);
            }

            case CommandType::Download:
            {
                return buildDownloadCommand(url);
            }

            case CommandType::ExtractPlaylist:
            {
                return buildExtractPlaylistCommand(url);
            }

            default:
            {
                return QStringList();
            }
        }
    }

    QStringList buildResolveCommand(const QString& url)
    {
        QStringList args;
        args << "-j" << url;

        return args;
    }

    QStringList buildDownloadCommand(const QString& url)
    {

        return QStringList();
    }

	QStringList buildDownloadCommand(const DownloadTask& task)
	{
		QString targetFile = "";
		if (task.type == UrlType::Single) {
			targetFile = QDir::toNativeSeparators(
				QString("%1/%2/%3.%4")
				.arg(task.savePath,
				task.video.playlistTitle,
				task.video.title,
				task.video.ext)
			);
		}
		else if (task.type == UrlType::Lists)
		{
			targetFile = QDir::toNativeSeparators(
				QString("%1/%2/%3-%4.%5")
				.arg(task.savePath,
				task.video.playlistTitle,
				QString::number(task.index),
				task.video.title,
				task.video.ext)
			);
		}

		QStringList args;
		args << "-o" << QDir::toNativeSeparators(targetFile) << task.video.url;

		return args;
	}

	QStringList buildExtractPlaylistCommand(const QString& url)
	{
		QStringList args;
		args << "--flat-playlist" << "-J" << url;

        return args;
	}


	

	void printCommand(const QStringList& args)
	{
        qDebug() << "------------ command args -------------";
		for (const QString& arg : args) {
			// 去掉两端的空白字符（包括换行符、空格、制表符等）
			QString trimmedArg = arg.trimmed();

			// 去掉字符串两端的换行符（如果有的话）
			trimmedArg.replace("\n", "").replace("\r", "");

			qDebug() << trimmedArg;
		}
        qDebug() << "------------ command args -------------";
	}



	// 假设你没有提供对 UrlType 的 qDebug 实现，这里提供一个实现：
	QDebug operator<<(QDebug dbg, UrlType type)
	{
		switch (type) {
		case UrlType::Unknown: dbg << "Unknown"; break;
		case UrlType::Lists: dbg << "Lists"; break;
		case UrlType::Single: dbg << "Single"; break;
		default: dbg << "Unknown"; break;
		}
		return dbg;
	}

	void printParsedEntry(const ParsedEntry& entry)
	{
		qDebug() << "------------ ParsedEntry -------------";

		qDebug() << "ID:" << entry.id;
		qDebug() << "URL:" << entry.url;
		qDebug() << "Playlist Title:" << entry.playlistTitle;
		qDebug() << "Type:" << entry.type;
		qDebug() << "Index:" << entry.index;
		qDebug() << "Playlist Count:" << entry.playlistCount;

		qDebug() << "------------ ParsedEntry -------------";
	}


	void printVideoFormat(const VideoFormat& format)
	{
		qDebug() << "VideoFormat:";
		qDebug() << "  formatId:" << format.formatId;
		qDebug() << "  ext:" << format.ext;
		qDebug() << "  vcodec:" << format.vcodec;
		qDebug() << "  acodec:" << format.acodec;
		qDebug() << "  width:" << format.width;
		qDebug() << "  height:" << format.height;
		qDebug() << "  filesize:" << format.filesize;
	}

	void printVideoEntry(const VideoEntry& entry)
	{
		
		qDebug() << "VideoEntry:";
		qDebug() << "  title:" << entry.title;
		qDebug() << "  url:" << entry.url;
		qDebug() << "  playlistTitle:" << entry.playlistTitle;

		// 打印 videoFormats
		qDebug() << "  videoFormats:";
		for (const VideoFormat& format : entry.videoFormats) {
			printVideoFormat(format);
		}

		// 打印字幕
		qDebug() << "  subtitles:";
		for (auto it = entry.subtitles.cbegin(); it != entry.subtitles.cend(); ++it) {
			qDebug() << "    Language:" << it.key() << "URL:" << it.value();
		}
	}

	void printDownloadTask(const DownloadTask& task)
	{
		qDebug() << "------------ printDownloadTask -------------";
		qDebug() << "DownloadTask:";
		qDebug() << "  id:" << task.id;
		qDebug() << "  index:" << task.index;
		qDebug() << "  playlistCount:" << task.playlistCount;
		qDebug() << "  resolveTime:" << task.resolveTime.toString();
		qDebug() << "  startTime:" << task.startTime.toString();
		qDebug() << "  endTime:" << task.endTime.toString();

		if (task.type == UrlType::Single)
		{
			qDebug() << "  type: " << task.type;
		}
		else if (task.type == UrlType::Lists)
		{
			qDebug() << "  type: " << task.type;
		}

		qDebug() << "  savePath:" << task.savePath;
		qDebug() << "  startTime:" << task.startTime.toString();
		qDebug() << "  endTime:" << task.endTime.toString();

		// 打印 video 信息
		printVideoEntry(task.video);

		qDebug() << "------------ printDownloadTask -------------";
	}

	void printParsedEntries(const QList<ParsedEntry>& entries)
	{
		for (auto entry : entries)
		{
			printParsedEntry(entry);
		}
	}


	void printDownloadHistoryItem(const DownloadHistoryItem& item)
	{
		qDebug() << "------------ DownloadHistoryItem -------------";

		qDebug() << "VID:" << item.vid;
		qDebug() << "Title:" << item.title;
		qDebug() << "Index:" << item.index;
		qDebug() << "Playlist Count:" << item.playlistCount;

		// 打印类型（根据 `UrlType` 枚举值来判断）
		QString typeStr;
		switch (item.type) {
		case UrlType::Single:
			typeStr = "Single";
			break;
		case UrlType::Lists:
			typeStr = "PlayList";
			break;
		default:
			typeStr = "Unknown";
			break;
		}
		qDebug() << "Type:" << typeStr;

		qDebug() << "Save Path:" << item.savePath;
		qDebug() << "Start Time:" << item.startTime.toString(Qt::ISODate);
		qDebug() << "End Time:" << item.endTime.toString(Qt::ISODate);

		// 打印状态（根据 `DownloadStatus` 枚举值来判断）
		QString statusStr;
		switch (item.status) {
		case DownloadStatus::Success:
			statusStr = "Success";
			break;
		case DownloadStatus::Failed:
			statusStr = "Failed";
			break;
		case DownloadStatus::Canceled:
			statusStr = "Canceled";
			break;
		default:
			statusStr = "Unknown";
			break;
		}
		qDebug() << "Status:" << statusStr;

		qDebug() << "------------ DownloadHistoryItem ---------";
	}


}
