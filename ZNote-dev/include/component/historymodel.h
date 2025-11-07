#pragma once

#include "core/download/task.h"

#include <QAbstractItemModel>

class HistoryModel  : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit HistoryModel(QObject *parent);
	~HistoryModel();


	// ����
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	// ����
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	// ��������
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	// ��������
	//bool setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/);

	// ��ȡ��ͷ����
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	// ��ȡָ���������
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

	// ��ȡ��������������Ҫ��
	QModelIndex parent(const QModelIndex& index) const override;

	// ������/�еȲ���
	void addhistory(const DownloadHistoryItem& history);

	// �����Ƴ�����ķ���
	void removehistorys(const QList<int>& rows);

	QList<DownloadHistoryItem> getHistortyItems() { return historyItems; }
	
	// 删除单条历史记录
	void removeHistory(int row);
	
	// 清空所有历史记录
	void clearHistory();
	
	// 刷新历史记录（从服务加载）
	void refresh();
	
	// 设置历史记录（从服务加载）
	void setHistory(const QList<DownloadHistoryItem> &items);


private:
	QList<DownloadHistoryItem> historyItems;
};

