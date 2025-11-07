#include "core/download/urlparser.h"
#include "utils/logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

UrlParser::UrlParser(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_isRunning(false)
    , m_hasParsedEntries(false)
{
    // 先连接信号（必须在所有return之前）
    connect(m_process, &QProcess::finished, this, &UrlParser::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred, this, &UrlParser::onProcessError);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &UrlParser::onStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &UrlParser::onStandardError);
    
    // 查找yt-dlp可执行文件
    QStringList possiblePaths;
    
    // 1. 检查项目bin目录
    QString appDir = QCoreApplication::applicationDirPath();
    QString binPath = QDir(appDir).filePath("yt-dlp.exe");
    if (QFile::exists(binPath)) {
        m_program = binPath;
        LOG_INFO(QString("Found yt-dlp at: %1").arg(m_program));
        return;
    }
    
    // 2. 检查系统PATH
    QString systemPath = QStandardPaths::findExecutable("yt-dlp.exe");
    if (!systemPath.isEmpty()) {
        m_program = systemPath;
        LOG_INFO(QString("Found yt-dlp in PATH: %1").arg(m_program));
        return;
    }
    
    // 3. 尝试不带.exe后缀（Linux/Mac）
    systemPath = QStandardPaths::findExecutable("yt-dlp");
    if (!systemPath.isEmpty()) {
        m_program = systemPath;
        LOG_INFO(QString("Found yt-dlp in PATH: %1").arg(m_program));
        return;
    }
    
    // 默认值
    m_program = "yt-dlp.exe";
    LOG_WARNING("yt-dlp not found, will try to use: yt-dlp.exe");
}

UrlParser::~UrlParser()
{
    cancel();
}

void UrlParser::parse(const QString &url)
{
    if (m_isRunning) {
        LOG_WARNING("Parser is already running, canceling previous request");
        cancel();
    }
    
    if (!isValidUrl(url)) {
        emit errorOccurred("Invalid URL format");
        return;
    }
    
    m_isRunning = true;
    m_hasParsedEntries = false;  // 重置标志
    
    QStringList arguments;
    arguments << "--dump-json";
    // 移除 --no-playlist，允许解析播放列表
    // 如果URL是播放列表，yt-dlp会返回JSON数组
    arguments << url;
    
    log(QString("Starting yt-dlp parse: %1").arg(url));
    m_process->start(m_program, arguments);
    
    // 不等待启动，避免阻塞UI线程
    // QProcess::start() 是异步的，错误会通过 errorOccurred 信号通知
}

void UrlParser::cancel()
{
    if (m_isRunning && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        // 不等待完成，避免阻塞UI线程
        // m_process->waitForFinished(3000);  // 移除阻塞调用
        m_isRunning = false;
        log("Parser cancelled");
    }
}

void UrlParser::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_isRunning = false;
    
    log(QString("Process finished with exit code: %1, status: %2").arg(exitCode).arg(exitStatus));
    
    if (exitStatus == QProcess::CrashExit) {
        QString errorMsg = "yt-dlp process crashed";
        log(errorMsg);
        emit errorOccurred(errorMsg);
        return;
    }
    
    if (exitCode != 0) {
        // 读取错误输出
        QByteArray errorOutput = m_process->readAllStandardError();
        QString errorMsg = QString("yt-dlp exited with code: %1").arg(exitCode);
        if (!errorOutput.isEmpty()) {
            errorMsg += QString("\nError output: %1").arg(QString::fromUtf8(errorOutput));
        }
        log(errorMsg);
        emit errorOccurred(errorMsg);
        return;
    }
    
    // 读取标准输出（可能包含多行JSON，每行一个JSON对象，用于播放列表）
    QByteArray output = m_process->readAllStandardOutput();
    
    // 如果已经通过流式读取解析了条目，且输出为空，说明已经处理完毕，不需要报错
    if (output.isEmpty() && m_hasParsedEntries) {
        log("Process finished: All entries were already parsed via stream reading");
        return;  // 正常完成，不报错
    }
    
    if (output.isEmpty()) {
        // 也检查错误输出，可能信息在那里
        QByteArray errorOutput = m_process->readAllStandardError();
        if (!errorOutput.isEmpty()) {
            log(QString("No stdout, but stderr: %1").arg(QString::fromUtf8(errorOutput)));
            emit errorOccurred(QString("yt-dlp error: %1").arg(QString::fromUtf8(errorOutput)));
        } else {
            emit errorOccurred("No output from yt-dlp");
        }
        return;
    }
    
    log(QString("Received %1 bytes from yt-dlp").arg(output.size()));
    
    // yt-dlp 对于播放列表，可能返回多行JSON（每行一个JSON对象）
    // 或者返回单个JSON数组
    QString outputStr = QString::fromUtf8(output);
    parseOutput(outputStr);
}

void UrlParser::onProcessError(QProcess::ProcessError error)
{
    m_isRunning = false;
    
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start yt-dlp. Please ensure it is installed and in PATH.";
            break;
        case QProcess::Crashed:
            errorMsg = "yt-dlp process crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "yt-dlp process timed out";
            break;
        case QProcess::WriteError:
            errorMsg = "Write error to yt-dlp process";
            break;
        case QProcess::ReadError:
            errorMsg = "Read error from yt-dlp process";
            break;
        default:
            errorMsg = QString("Unknown error: %1").arg(error);
    }
    
    emit errorOccurred(errorMsg);
}

void UrlParser::onStandardOutput()
{
    // 流式读取输出，每读取一行就解析并发送（生产者-消费者模式）
    while (m_process->canReadLine()) {
        QByteArray line = m_process->readLine();
        QString lineStr = QString::fromUtf8(line).trimmed();
        
        if (lineStr.isEmpty()) {
            continue;
        }
        
        // 尝试解析这一行（可能是JSON对象）
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(lineStr.toUtf8(), &error);
        
        if (error.error == QJsonParseError::NoError && doc.isObject()) {
            ParsedEntry entry = parseSingleEntry(doc.object());
            if (!entry.vid.isEmpty()) {
                // 立即发送单个条目信号（生产者-消费者模式）
                emit entryParsed(entry);
                m_hasParsedEntries = true;  // 标记已解析条目
                log(QString("Parsed entry: %1").arg(entry.title));
            }
        }
    }
}

void UrlParser::onStandardError()
{
    QByteArray error = m_process->readAllStandardError();
    QString errorStr = QString::fromUtf8(error);
    log(QString("yt-dlp stderr: %1").arg(errorStr));
}

void UrlParser::parseOutput(const QString &output)
{
    QList<ParsedEntry> entries = parseJsonOutput(output);
    
    if (entries.isEmpty()) {
        emit errorOccurred("Failed to parse yt-dlp output");
        return;
    }
    
    // 发送批量信号（保留用于兼容性）
    emit urlParsed(entries);
    log(QString("Parsed %1 entries").arg(entries.size()));
}

QList<ParsedEntry> UrlParser::parseJsonOutput(const QString &jsonOutput)
{
    QList<ParsedEntry> entries;
    
    if (jsonOutput.trimmed().isEmpty()) {
        log("JSON output is empty");
        return entries;
    }
    
    // yt-dlp 对于播放列表，可能返回：
    // 1. 多行JSON（每行一个JSON对象）- 可以逐行解析并立即发送
    // 2. 单个JSON数组 - 需要解析后逐个发送
    // 3. 单个JSON对象（单个视频）- 可以立即解析并发送
    
    QStringList lines = jsonOutput.split('\n', Qt::SkipEmptyParts);
    
    // 如果只有一行，尝试解析为JSON数组或对象
    if (lines.size() == 1) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(jsonOutput.toUtf8(), &error);
        
        if (error.error != QJsonParseError::NoError) {
            log(QString("JSON parse error: %1 at position %2").arg(error.errorString()).arg(error.offset));
            log(QString("JSON content: %1").arg(jsonOutput.left(500))); // 只显示前500字符
            return entries;
        }
        
        // 解析数组（播放列表）- 逐个解析并立即发送
        if (doc.isArray()) {
            QJsonArray array = doc.array();
            log(QString("Parsing JSON array with %1 items").arg(array.size()));
            for (const QJsonValue &value : array) {
                if (value.isObject()) {
                    ParsedEntry entry = parseSingleEntry(value.toObject());
                    if (!entry.vid.isEmpty()) {
                        entries.append(entry);
                        // 立即发送单个条目信号（生产者-消费者模式）
                        emit entryParsed(entry);
                        m_hasParsedEntries = true;  // 标记已解析条目
                    }
                }
            }
        } 
        // 解析单个对象 - 立即发送
        else if (doc.isObject()) {
            log("Parsing single JSON object");
            ParsedEntry entry = parseSingleEntry(doc.object());
            if (!entry.vid.isEmpty()) {
                entries.append(entry);
                // 立即发送单个条目信号
                emit entryParsed(entry);
                m_hasParsedEntries = true;  // 标记已解析条目
            }
        } else {
            log("JSON document is neither array nor object");
        }
    } 
    // 多行JSON（每行一个JSON对象，播放列表格式）- 逐行解析并立即发送
    else {
        log(QString("Parsing %1 lines of JSON (playlist format)").arg(lines.size()));
        for (const QString &line : lines) {
            if (line.trimmed().isEmpty()) {
                continue;
            }
            
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &error);
            
            if (error.error != QJsonParseError::NoError) {
                log(QString("JSON parse error in line: %1").arg(error.errorString()));
                continue;
            }
            
            if (doc.isObject()) {
                ParsedEntry entry = parseSingleEntry(doc.object());
                if (!entry.vid.isEmpty()) {
                    entries.append(entry);
                    // 立即发送单个条目信号（生产者-消费者模式）
                    emit entryParsed(entry);
                    m_hasParsedEntries = true;  // 标记已解析条目
                }
            }
        }
    }
    
    log(QString("Successfully parsed %1 entries").arg(entries.size()));
    return entries;
}

ParsedEntry UrlParser::parseSingleEntry(const QJsonObject &json)
{
    ParsedEntry entry;
    
    entry.id = json["id"].toString();
    entry.vid = entry.id;  // vid与id相同
    entry.title = json["title"].toString();
    entry.url = json["webpage_url"].toString();
    entry.duration = json["duration"].toInt(0);
    entry.thumbnail = json["thumbnail"].toString();
    
    // 提取格式信息，优先选择视频格式
    QJsonArray formats = json["formats"].toArray();
    if (!formats.isEmpty()) {
        // 优先选择最佳视频格式（包含视频流的格式）
        QString bestVideoFormatId;
        QString bestVideoExt;
        int bestHeight = 0;
        
        for (const QJsonValue &formatValue : formats) {
            QJsonObject format = formatValue.toObject();
            QString vcodec = format["vcodec"].toString();
            QString acodec = format["acodec"].toString();
            
            // 跳过纯音频格式（vcodec == "none"）
            if (vcodec == "none" || vcodec.isEmpty()) {
                continue;
            }
            
            // 选择分辨率最高的视频格式
            int height = format["height"].toInt(0);
            if (height > bestHeight) {
                bestHeight = height;
                bestVideoFormatId = format["format_id"].toString();
                bestVideoExt = format["ext"].toString();
            }
        }
        
        if (!bestVideoFormatId.isEmpty()) {
            entry.formatId = bestVideoFormatId;
            entry.ext = bestVideoExt;
        } else {
            // 如果找不到视频格式，使用默认的 bestvideo+bestaudio/best
            entry.formatId = "";  // 空字符串表示使用默认格式选择
            entry.ext = json["ext"].toString("mp4");
        }
    } else {
        // 没有格式信息，使用默认值
        entry.formatId = "";  // 空字符串表示使用默认格式选择
        entry.ext = json["ext"].toString("mp4");
    }
    
    // 设置默认值
    if (entry.ext.isEmpty()) {
        entry.ext = "mp4";
    }
    
    // 判断类型
    if (json.contains("playlist") || json.contains("playlist_index")) {
        entry.type = UrlType::Lists;
        entry.index = json["playlist_index"].toInt(1);
        entry.playlistCount = json["playlist_count"].toInt(1);
    } else {
        entry.type = UrlType::Single;
        entry.index = 1;
        entry.playlistCount = 1;
    }
    
    return entry;
}

void UrlParser::log(const QString &message)
{
    LOG_INFO(QString("UrlParser: %1").arg(message));
    emit logMessage(message);
}

bool UrlParser::isValidUrl(const QString &url)
{
    if (url.isEmpty()) {
        return false;
    }
    
    // 简单的URL验证
    return url.startsWith("http://") || url.startsWith("https://");
}

