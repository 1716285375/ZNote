#ifndef URLPARSER_H
#define URLPARSER_H

#include <QProcess>
#include <QByteArray>
#include <QList>

#include "task.h"

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
    void parseOutput(const QByteArray& output);
    QProcess *process;
    QString currentUrl;
    QByteArray buffer;
};
#endif // URLPARSER_H
