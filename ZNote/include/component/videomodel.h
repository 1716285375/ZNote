#pragma once
#include "core/download/task.h"

#include <QAbstractItemModel>
#include <QList>
#include <QVariant>


class VideoModel  : public QAbstractItemModel
{
	Q_OBJECT

public:
	VideoModel(QObject *parent = nullptr);
	~VideoModel();


	// ����
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	// ����
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	// ��������
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	// ��������
	bool setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/);

	// ��ȡ��ͷ����
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	// ��ȡָ���������
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

	// ��ȡ��������������Ҫ��
	QModelIndex parent(const QModelIndex& index) const override;

	// ������/�еȲ���
	void addTask(const DownloadTask& task);

	// �����Ƴ�����ķ���
	void removeTasks(const QList<int>& rows);

	QList<DownloadTask*> getTasks() { return taskItems; }
	
	// 清空所有任务
	void clear();


private:
	QList<DownloadTask*> taskItems;
};

