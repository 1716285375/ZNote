#include "misc.h"
#include "task.h"

#include <QRandomGenerator>
#include <QString>
#include <QChar>
#include <QRegularExpression>

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

	QStringList buildExtractPlaylistCommand(const QString& url)
	{
		QStringList args;
		args << "--flat-playlist" << "-j" << url;

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
		qDebug() << "Webpage URL Basename:" << entry.wepagebUrlBasename;
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
		qDebug() << "  url:" << task.url;
		qDebug() << "  index:" << task.index;
		qDebug() << "  savePath:" << task.savePath;
		qDebug() << "  playlistTitle:" << task.playlistTitle;
		qDebug() << "  resolution:" << task.resolution;
		qDebug() << "  audioFormat:" << task.audioFormat;
		qDebug() << "  startTime:" << task.startTime.toString();
		qDebug() << "  endTime:" << task.endTime.toString();
		qDebug() << "  subtitles:" << task.subtitles;

		// 打印 video 信息
		printVideoEntry(task.video);

		qDebug() << "------------ printDownloadTask -------------";
	}



}
