#ifndef DOWNLOADUTILS_H
#define DOWNLOADUTILS_H

#include "core/download/task.h"
#include <QList>
#include <QString>

class IConfigService;

namespace znote {
namespace utils {

// 构建下载命令
QList<QString> buildDownloadCommand(const DownloadTask &task, IConfigService *configService = nullptr);

// 打印命令
void printCommand(const QList<QString> &command);

} // namespace utils
} // namespace znote

#endif // DOWNLOADUTILS_H

