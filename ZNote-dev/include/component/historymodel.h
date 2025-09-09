#pragma once

#include "task.h"

#include <QAbstractItemModel>

class HistoryModel  : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit HistoryModel(QObject *parent);
	~HistoryModel();


	// 行数
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	// 列数
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	// 返回数据
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	// 设置数据
	//bool setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/);

	// 获取表头数据
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	// 获取指定项的索引
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

	// 获取父项索引（若需要）
	QModelIndex parent(const QModelIndex& index) const override;

	// 插入行/列等操作
	void addhistory(const DownloadHistoryItem& history);

	// 添加移除任务的方法
	void removehistorys(const QList<int>& rows);

	QList<DownloadHistoryItem> getHistortyItems() { return historyItems; }


private:
	QList<DownloadHistoryItem> historyItems;
};

