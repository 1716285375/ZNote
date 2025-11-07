#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QFile>
#include <QMutex>
#include <memory>

enum class LogLevel
{
    Debug = 0,
    Info,
    Warning,
    Error,
    Critical
};

class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger& instance();
    
    // 配置
    void setLogLevel(LogLevel level);
    void setLogFile(const QString &filePath);
    void setConsoleOutput(bool enabled);
    
    // 日志输出
    void debug(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);
    void critical(const QString &message);
    
    // 格式化输出
    void log(LogLevel level, const QString &message, const QString &source = QString());

signals:
    void messageLogged(LogLevel level, const QString &message, const QDateTime &timestamp);

private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();
    
    Q_DISABLE_COPY(Logger)
    
    void writeToFile(const QString &message);
    void writeToConsole(const QString &message);
    QString formatMessage(LogLevel level, const QString &message, const QString &source);
    QString levelToString(LogLevel level);
    
    LogLevel m_logLevel;
    std::unique_ptr<QFile> m_logFile;
    bool m_consoleOutput;
    mutable QMutex m_mutex;
};

// 便捷宏
#define LOG_DEBUG(msg) Logger::instance().debug(msg)
#define LOG_INFO(msg) Logger::instance().info(msg)
#define LOG_WARNING(msg) Logger::instance().warning(msg)
#define LOG_ERROR(msg) Logger::instance().error(msg)
#define LOG_CRITICAL(msg) Logger::instance().critical(msg)

#endif // LOGGER_H
