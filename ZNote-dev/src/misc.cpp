#include "misc.h"
#include "task.h"

#include <QRandomGenerator>
#include <QString>
#include <QChar>
#include <QRegularExpression>

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

    QString buildDownloadCommand(const DownloadTask &task)
    {

        return QString("");
    }



}
