#include "urlparser.h"
#include "misc.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

UrlParser::UrlParser(QObject *parent)
    : QObject(parent),
    process(new QProcess(this))
{
    connect(process, &QProcess::readyReadStandardOutput, this, [this]() {
        buffer.append(process->readAllStandardOutput());
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int exitCode, QProcess::ExitStatus /*status*/) {
                if (exitCode == 0)
                {
                    QJsonParseError err;
                    QJsonDocument doc = QJsonDocument::fromJson(buffer, &err);

                    if (err.error != QJsonParseError::NoError)
                    {
                        emit errorOccurred("JSON parse error: " + err.errorString());
                        return;
                    }

                    if (!doc.isObject())
                    {
                        emit errorOccurred("Invalid JSON from yt-dlp");
                        return;
                    }

                    QJsonObject root = doc.object();
                    QString playlistTitle = root.value("title").toString("UnknownTitle" + znote::utils::generateRandomString(6));
                    QString id = root.value("id").toString();
                    int playlistCount = root.value("playlist_count").toInt();
                    QString webpageUrlBasename = root.value("webpage_url_basename").toString();

                    QList<ParsedEntry> results;
                    QJsonArray entries = root.value("entries").toArray();
                    int index = 1;
                    for (const QJsonValue &v : entries)
                    {
                        QJsonObject obj = v.toObject();
                        ParsedEntry entry;
                        entry.url = obj.value("url").toString();
                        entry.playlistTitle = playlistTitle;
                        entry.id = id;
                        entry.index = index;
                        entry.playlistCount = playlistCount;
                        entry.wepagebUrlBasename = webpageUrlBasename;
                        entry.type = UrlType::Lists;
                        index += 1;
                        results.append(entry);
                    }

                    if (results.isEmpty())
                    {
                        // single video
                        ParsedEntry single;
                        single.id = root.value("id").toString();
                        single.playlistTitle = playlistTitle;
                        single.url = root.value("webpage_url").toString();
                        single.wepagebUrlBasename = root.value("webpage_url_basename").toString();
                        single.playlistCount = 1;
                        single.index = 1;
                        single.type = UrlType::Single;
                        results.append(single);
                    }

                    emit urlParsed(results);
                }
                else
                {
                    emit errorOccurred(QString("yt-dlp exit code %1").arg(exitCode));
                }


                buffer.clear();
                process->deleteLater();

            });
}

void UrlParser::parse(const QString &url)
{
    currentUrl = url;

    QStringList args;
    // args << "--skip-download";
    // args << "--print";
    // args << "%(webpage_url)s";
    args << "--flat-playlist";
    args << "-J";
    args << url;

    process->start("yt-dlp.exe", args);

    emit logMessage("▶ Running: yt-dlp.exe " + args.join(" "));
}
