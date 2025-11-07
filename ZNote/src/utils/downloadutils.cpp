#include "utils/downloadutils.h"
#include "core/interfaces/iconfigservice.h"
#include <QDebug>
#include <QDir>

namespace znote {
namespace utils {

QList<QString> buildDownloadCommand(const DownloadTask &task, IConfigService *configService)
{
    Q_UNUSED(configService);
    QList<QString> command;
    
    // 输出路径
    QString outputPath = QDir(task.savePath).absolutePath();
    command << "-o" << QString("%1/%(title)s.%(ext)s").arg(outputPath);
    
    // 视频质量 - 优先选择视频格式
    if (!task.video.formatId.isEmpty()) {
        // 使用指定的格式ID
        command << "-f" << task.video.formatId;
    } else {
        // 默认选择最佳视频+音频格式（合并为mp4）
        // bestvideo+bestaudio 会下载最佳视频和最佳音频，然后合并
        command << "-f" << "bestvideo+bestaudio/best";
        // 确保输出格式为mp4
        command << "--merge-output-format" << "mp4";
    }
    
    // 其他选项
    command << "--no-playlist"; // 不下载播放列表
    command << "--no-warnings"; // 不显示警告信息
    command << "--progress"; // 显示进度条
    
    // URL
    command << task.video.url;
    
    return command;
}

void printCommand(const QList<QString> &command)
{
    QString cmdStr = command.join(" ");
    qDebug() << "Executing command:" << cmdStr;
}

} // namespace utils
} // namespace znote
