#ifndef JSONHISTORYBACKEND_H
#define JSONHISTORYBACKEND_H


#include "interface/ihistorybackend.h"

#include <QString>

class JSONHistoryBackend : public IHistoryBackend
{
public:
    explicit JSONHistoryBackend(const QString &filePath);

    void add(const DownloadHistoryItem &item) override;
    QVector<DownloadHistoryItem> load() override;
    void save(const QVector<DownloadHistoryItem> &items) override;

private:
    QString filePath;
    QVector<DownloadHistoryItem> cache;
};

#endif // JSONHISTORYBACKEND_H
