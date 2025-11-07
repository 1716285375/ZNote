#include "utils/logger.h"
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger(QObject *parent)
    : QObject(parent)
    , m_logLevel(LogLevel::Info)
    , m_consoleOutput(true)
{
    // 设置默认日志文件路径
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);
    QString logPath = QDir(logDir).filePath("znote.log");
    setLogFile(logPath);
}

Logger::~Logger()
{
    if (m_logFile && m_logFile->isOpen()) {
        m_logFile->close();
    }
}

void Logger::setLogLevel(LogLevel level)
{
    QMutexLocker locker(&m_mutex);
    m_logLevel = level;
}

void Logger::setLogFile(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_logFile && m_logFile->isOpen()) {
        m_logFile->close();
    }
    
    m_logFile = std::make_unique<QFile>(filePath);
    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
        qWarning() << "Failed to open log file:" << filePath;
        m_logFile.reset();
    }
}

void Logger::setConsoleOutput(bool enabled)
{
    QMutexLocker locker(&m_mutex);
    m_consoleOutput = enabled;
}

void Logger::debug(const QString &message)
{
    log(LogLevel::Debug, message);
}

void Logger::info(const QString &message)
{
    log(LogLevel::Info, message);
}

void Logger::warning(const QString &message)
{
    log(LogLevel::Warning, message);
}

void Logger::error(const QString &message)
{
    log(LogLevel::Error, message);
}

void Logger::critical(const QString &message)
{
    log(LogLevel::Critical, message);
}

void Logger::log(LogLevel level, const QString &message, const QString &source)
{
    QMutexLocker locker(&m_mutex);
    
    if (level < m_logLevel) {
        return;
    }
    
    QString formattedMessage = formatMessage(level, message, source);
    QDateTime timestamp = QDateTime::currentDateTime();
    
    writeToFile(formattedMessage);
    if (m_consoleOutput) {
        writeToConsole(formattedMessage);
    }
    
    emit messageLogged(level, message, timestamp);
}

void Logger::writeToFile(const QString &message)
{
    if (m_logFile && m_logFile->isOpen()) {
        QTextStream stream(m_logFile.get());
        stream << message << Qt::endl;
        stream.flush();
    }
}

void Logger::writeToConsole(const QString &message)
{
    QTextStream stream(stdout);
    stream << message << Qt::endl;
    stream.flush();
}

QString Logger::formatMessage(LogLevel level, const QString &message, const QString &source)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    QString sourceStr = source.isEmpty() ? "ZNote" : source;
    
    return QString("[%1] [%2] [%3] %4")
           .arg(timestamp)
           .arg(levelStr)
           .arg(sourceStr)
           .arg(message);
}

QString Logger::levelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARN";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}
