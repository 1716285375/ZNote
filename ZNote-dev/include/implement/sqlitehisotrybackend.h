#ifndef SQLITEHISOTRYBACKEND_H
#define SQLITEHISOTRYBACKEND_H

#include "interface/ihistorybackend.h"
#include <QString>
#include <QSqlDatabase>

class SQLiteHistoryBackend : public IHistoryBackend {
public:
    explicit SQLiteHistoryBackend(const QString &dbPath);

    void add(const DownloadHistoryItem &item) override;
    QList<DownloadHistoryItem> load() override;
    void remove(const QString& vid) override;
    void save(const QVector<DownloadHistoryItem> &items) override;

private:
    QSqlDatabase db;
};

#endif // SQLITEHISOTRYBACKEND_H
