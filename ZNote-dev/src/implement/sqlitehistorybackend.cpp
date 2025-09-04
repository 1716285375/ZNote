#include "implement/sqlitehisotrybackend.h"


#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>



SQLiteHistoryBackend::SQLiteHistoryBackend(const QString &dbPath)
{
    db = QSqlDatabase::addDatabase("QSQLITE", "history_conn");
    db.setDatabaseName(dbPath);

    if (!db.open())
    {
        qWarning() << "Failed to open DB: " << db.lastError();
        return;
    }

    QSqlQuery q(db);
    q.exec("CREATE TABLE IF NOT EXISTS history ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "url TEXT,"
           "title TEXT,"
           "filePath TEXT,"
           "startTime TEXT,"
           "endTime TEXT,"
           "status TEXT"
           ")");

}

void SQLiteHistoryBackend::add(const DownloadHistoryItem &item)
{
    QSqlQuery q(db);
    q.prepare("INSERT INTO history (url, title, filePath, startTime, endTime, status) "
              "VALUES (?, ?, ?, ?, ?, ?)");
    q.addBindValue(item.url);
    q.addBindValue(item.title);
    q.addBindValue(item.filePath);
    q.addBindValue(item.startTime.toString(Qt::ISODate));
    q.addBindValue(item.endTime.toString(Qt::ISODate));
    q.addBindValue(item.status);
    if (!q.exec()) {
        qWarning() << "Insert failed:" << q.lastError();
    }
}

QVector<DownloadHistoryItem> SQLiteHistoryBackend::load()
{
    QVector<DownloadHistoryItem> items;
    QSqlQuery q("SELECT url, title, filePath, startTime, endTime, status FROM history", db);
    while (q.next()) {
        DownloadHistoryItem item;
        item.url = q.value(0).toString();
        item.title = q.value(1).toString();
        item.filePath = q.value(2).toString();
        item.startTime = QDateTime::fromString(q.value(3).toString(), Qt::ISODate);
        item.endTime = QDateTime::fromString(q.value(4).toString(), Qt::ISODate);
        item.status = q.value(5).toString();
        items.append(item);
    }
    return items;
}


void SQLiteHistoryBackend::save(const QVector<DownloadHistoryItem> &items) {
    // SQLite 不需要一次性写全体，通常只增量 add
    // 这里可以选择清空再写，或者直接忽略
}

