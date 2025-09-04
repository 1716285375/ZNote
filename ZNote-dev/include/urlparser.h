#ifndef URLPARSER_H
#define URLPARSER_H

#include <QProcess>
#include <QByteArray>
#include <QList>
enum class UrlType
{
    Lists = 0,
    Single
};

struct ParsedEntry
{
    QString id;
    QString url;
    QString wepagebUrlBasename;
    QString playlistTitle;
    UrlType type;
    int index;
    int playlistCount;
};

class UrlParser : public QObject
{
    Q_OBJECT
public:
    explicit UrlParser(QObject *parent = nullptr);
    virtual ~UrlParser() {}

    void parse(const QString &url);

signals:
    void urlParsed(const QList<ParsedEntry> &entires);
    void logMessage(const QString &msg);
    void errorOccurred(const QString &error);

private:
    QProcess *process;
    QString currentUrl;
    QByteArray buffer;
};
#endif // URLPARSER_H
