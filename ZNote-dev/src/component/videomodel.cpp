#include "component/videomodel.h"

VideoModel::VideoModel(QObject *parent)
	: QAbstractItemModel(parent)
{

}

VideoModel::~VideoModel()
{

}

int VideoModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	if (parent.isValid()) {
		qDebug() << "Invalid index!";
		return 0;
	}
	return taskItems.count();
}

int VideoModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	return 7;	// 7列：ID, 视频标题, 集数, 类型, 解析时间, 保存路径, 下载状态
}

QVariant VideoModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
	if (!index.isValid())
	{
		qDebug() << "Invalid index!";
		return QVariant();
	}

	auto task = taskItems.at(index.row());

	if (role == Qt::DisplayRole) {
		switch (index.column()) {
		case 0:
			return task->id;
		case 1:
			return task->video.title;
		case 2:
			return QString::asprintf("%d/%d", task->index, task->playlistCount);
		case 3:
			return task->type == UrlType::Single ? tr("Single") : tr("Playlist");
		case 4:
			return task->resolveTime.toString("yyyy-MM-dd HH:mm:ss");
		case 5:
			return task->savePath;
		//case 6:
		//	return task->isSelected ? Qt::Checked : Qt::Unchecked;  // 返回 Qt::Checked 或 Qt::Unchecked
		default:
			return QVariant();
		}
	}

	// 处理复选框状态
	if (role == Qt::CheckStateRole && index.column() == 6) {  // 处理复选框状态
		return task->isSelected ? Qt::Checked : Qt::Unchecked;
	}

	return QVariant();
}

bool VideoModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid()) {
		qDebug() << "Invalid index!";
		return false;
	}

	qDebug() << "Checkbox state updated to: " << value;

	if (role == Qt::CheckStateRole && index.column() == 6) {  // 只更新复选框列
		bool checked = value.toBool();
		taskItems[index.row()]->isSelected = checked;  // 更新任务的选中状态


		emit dataChanged(index, index, { role });  // 通知视图更新
		return true;
	}

	return false;
}

QVariant VideoModel::headerData(int section, Qt::Orientation orientation, int role) const
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
			return tr("Resolved Time");
		case 5:
			return tr("Save Path");
		case 6:
			return tr("Download");
		default:
			return QVariant();
		}
	}
	return QVariant();
}


QModelIndex VideoModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
	if (row < 0 || column < 0 || row >= taskItems.count())
		return QModelIndex();
	return createIndex(row, column);
}

QModelIndex VideoModel::parent(const QModelIndex& index) const
{
	return QModelIndex();  // 没有父项
}

void VideoModel::addTask(const DownloadTask& task)
{
	DownloadTask *taskPtr = new DownloadTask(task);
	beginInsertRows(QModelIndex(), taskItems.count(), taskItems.count());
	taskItems.append(taskPtr);
	endInsertRows();
}

void VideoModel::removeTasks(const QList<int>& rows)
{
	// 必须从最后一行开始删除，以避免删除时行索引变化
	if (rows.isEmpty()) return;

	QList<int> sortedRows = rows;
	std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>()); // 按降序排列

	beginRemoveRows(QModelIndex(), sortedRows.last(), sortedRows.first()); // 从首尾删除行

	for (int row : sortedRows) {
		if (row < 0 || row >= taskItems.count()) {
			continue;  // 跳过无效行
		}
		// 释放任务指针
		DownloadTask* task = taskItems.at(row);
		delete task;
		taskItems.removeAt(row);  // 从 QList 中移除该任务
	}

	endRemoveRows();  // 通知视图结束移除行
}

