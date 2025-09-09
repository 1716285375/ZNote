#include "component/historymodel.h"

HistoryModel::HistoryModel(QObject *parent)
	: QAbstractItemModel(parent)
{}

HistoryModel::~HistoryModel()
{}

int HistoryModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	if (parent.isValid()) {
		qDebug() << "Invalid index!";
		return 0;
	}
	return historyItems.count();
}

int HistoryModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	return 8;	// 8列：ID, 视频标题, 集数, 类型, 保存路径, 开始下载时间, 结束下载时间, 下载状态
}

QVariant HistoryModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
	if (!index.isValid())
	{
		qDebug() << "Invalid index!";
		return QVariant();
	}

	auto history = historyItems.at(index.row());

	if (role == Qt::DisplayRole) {
		switch (index.column()) {
		case 0:
			return history.vid;
		case 1:
			return history.title;
		case 2:
			return QString::asprintf("%d/%d", history.index, history.playlistCount);
		case 3:
			return history.type == UrlType::Single ? tr("Single") : tr("Playlist");
		case 4:
			return history.savePath;
		case 5:
			return history.startTime.toString("yyyy-MM-dd HH:mm:ss");
		case 6:
			return history.endTime.toString("yyyy-MM-dd HH:mm:ss");
		case 7:
			return history.status == DownloadStatus::Success ? "success" : history.status == DownloadStatus::Failed ? "failed" : "canceled";
		default:
			return QVariant();
		}
	}

	return QVariant();
}

//bool HistoryModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
//{	
//	return true;
//}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		switch (section) {
		case 0:
			return tr("ID");
		case 1:
			return tr("Video Title");
		case 2:
			return tr("Current/Total Episodes");
		case 3:
			return tr("Type");
		case 4:
			return tr("Save Path");
		case 5:
			return tr("Start Time");
		case 6:
			return tr("End Time");
		case 7:
			return tr("Download Status");
		default:
			return QVariant();
		}
	}
	return QVariant();
}

QModelIndex HistoryModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
	if (row < 0 || column < 0 || row >= historyItems.count())
		return QModelIndex();
	return createIndex(row, column);
}

QModelIndex HistoryModel::parent(const QModelIndex& index) const
{
	return QModelIndex();  // 没有父项
}

void HistoryModel::addhistory(const DownloadHistoryItem& history)
{
	beginInsertRows(QModelIndex(), historyItems.count(), historyItems.count());
	historyItems.append(history);
	endInsertRows();
}

void HistoryModel::removehistorys(const QList<int>& rows)
{
	// 必须从最后一行开始删除，以避免删除时行索引变化
	if (rows.isEmpty()) return;

	QList<int> sortedRows = rows;
	std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>()); // 按降序排列

	beginRemoveRows(QModelIndex(), sortedRows.last(), sortedRows.first()); // 从首尾删除行

	for (int row : sortedRows) {
		if (row < 0 || row >= historyItems.count()) {
			continue;  // 跳过无效行
		}

		historyItems.removeAt(row);  // 从 QList 中移除该任务
	}

	endRemoveRows();  // 通知视图结束移除行
}

