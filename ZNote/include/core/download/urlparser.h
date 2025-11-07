#ifndef URLPARSER_H
#define URLPARSER_H

#include "core/download/task.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QProcess>

class UrlParser : public QObject
{
    Q_OBJECT

public:
    explicit UrlParser(QObject *parent = nullptr);
    ~UrlParser() override;

    void parse(const QString &url);
    void cancel();

signals:
    void urlParsed(const QList<ParsedEntry> &entries);  // 保留用于兼容性
    void entryParsed(const ParsedEntry &entry);  // 新增：单个条目解析完成
    void logMessage(const QString &message);
    void errorOccurred(const QString &error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onStandardOutput();
    void onStandardError();

private:
    void parseOutput(const QString &output);
    QList<ParsedEntry> parseJsonOutput(const QString &jsonOutput);
    ParsedEntry parseSingleEntry(const QJsonObject &json);
    void log(const QString &message);
    bool isValidUrl(const QString &url);

    QProcess *m_process;
    QString m_program;
    bool m_isRunning;
    bool m_hasParsedEntries;  // 标记是否已经通过流式读取解析了条目
};

#endif // URLPARSER_H

