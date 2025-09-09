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
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"vid TEXT, "
		"title TEXT, "
		"`index` INTEGER, "  // 使用反引号避免潜在问题
		"playlistCount INTEGER, "
		"type TEXT, "
		"savePath TEXT, "
		"startTime TEXT, "
		"endTime TEXT, "
		"status TEXT"
		")");


}

void SQLiteHistoryBackend::add(const DownloadHistoryItem &item)
{
	QSqlQuery q(db);
	q.prepare("INSERT INTO history (vid, title, `index`, playlistCount, type, savePath, startTime, endTime, status) "
		"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(item.vid);
	q.addBindValue(item.title);
	q.addBindValue(item.index);
	q.addBindValue(item.playlistCount);
	q.addBindValue(item.type == UrlType::Single ? "Single" :
		item.type == UrlType::Lists ? "PlayList" :
		"UnKnown");
	q.addBindValue(item.savePath);
	q.addBindValue(item.startTime.toString(Qt::ISODate));
	q.addBindValue(item.endTime.toString(Qt::ISODate));
	q.addBindValue(item.status == DownloadStatus::Success ? "success" :
		item.status == DownloadStatus::Failed ? "failed" :
		"canceled");  // 转换 DownloadStatus 枚举值为字符串

	if (!q.exec()) {
		qWarning() << "Insert failed:" << q.lastError();
	}
}

void SQLiteHistoryBackend::remove(const QString& vid)
{
	QSqlQuery q(db);
	q.prepare("DELETE FROM history WHERE vid = ?");
	q.addBindValue(vid);  // 绑定要删除的唯一标识符

	if (!q.exec()) {
		qWarning() << "Delete failed:" << q.lastError();
	}
}


QList<DownloadHistoryItem> SQLiteHistoryBackend::load()
{
    QList<DownloadHistoryItem> items;
    QSqlQuery q("SELECT vid, title, `index`, playlistCount, type, savePath, startTime, endTime, status FROM history", db);
    while (q.next()) {
		//qDebug() << "Fetching row " << q.at();  // 输出当前行的索引
        DownloadHistoryItem item;
        item.vid = q.value(0).toString();
        item.title = q.value(1).toString();
        item.index = q.value(2).toInt();
		item.playlistCount = q.value(3).toInt();
		QString typeStr = q.value(4).toString();
		if (typeStr == "Single") {
			item.type = UrlType::Single;
		}
		else if (typeStr == "PlayList") {
			item.type = UrlType::Lists;
		}
		else {
			item.type = UrlType::Unknown; // 如果不匹配，则设置为 Unknown
		}

        item.savePath = q.value(5).toString();
        item.startTime = QDateTime::fromString(q.value(6).toString(), Qt::ISODate);
        item.endTime = QDateTime::fromString(q.value(7).toString(), Qt::ISODate);

		QString statusStr = q.value(8).toString();
		if (statusStr == "success") {
			item.status = DownloadStatus::Success;
		}
		else if (statusStr == "failed") {
			item.status = DownloadStatus::Failed;
		}
		else if (statusStr == "canceled") {
			item.status = DownloadStatus::Canceled;
		}
		else {
			item.status = DownloadStatus::Failed; // 默认值
		}

        items.append(item);
    }
	qDebug() << items.size();
    return items;
}


void SQLiteHistoryBackend::save(const QVector<DownloadHistoryItem> &items) {
    // SQLite 不需要一次性写全体，通常只增量 add
    // 这里可以选择清空再写，或者直接忽略
    return;
}

