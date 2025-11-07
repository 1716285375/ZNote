#include "component/checkboxdelegate.h"
#include <QCheckBox>


CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
     : QStyledItemDelegate(parent)
{

}

void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() == 6) { // 第 6 列是复选框列
		bool checked = index.data(Qt::CheckStateRole).toBool(); // 获取复选框的状态
		QStyleOptionButton checkboxStyle;
		checkboxStyle.state = checked ? QStyle::State_On : QStyle::State_Off; // 设置复选框的状态
		checkboxStyle.rect = option.rect; // 设置复选框的大小和位置
		QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxStyle, painter); // 绘制复选框
	}
	else {
		QStyledItemDelegate::paint(painter, option, index); // 绘制其他列内容
	}
}

bool CheckBoxDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (index.column() == 6 && event->type() == QEvent::MouseButtonRelease) {  // 只处理第 6 列的点击事件
		bool checked = index.data(Qt::CheckStateRole).toBool();  // 获取当前状态
		model->setData(index, checked ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);  // 切换复选框状态
		return true;
	}
	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
