#include "component/checkboxdelegate.h"



CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
     : QStyledItemDelegate(parent)
{

}

void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool checked = index.model()->data(index, Qt::CheckStateRole).toInt() == Qt::Checked;

    QStyleOptionButton checkbox;

    // 计算合适的复选框大小和位置（居中显示）
    QSize size = QApplication::style()->sizeFromContents(QStyle::CT_CheckBox, &checkbox, QSize());
    checkbox.rect = QRect(option.rect.x() + (option.rect.width() - size.width()) / 2,
                          option.rect.y() + (option.rect.height() - size.height()) / 2,
                          size.width(), size.height());

    checkbox.state = QStyle::State_Enabled;
    if (checked)
    {
        checkbox.state |= QStyle::State_On;
    }
    else
    {
        checkbox.state |= QStyle::State_Off;
    }

    if (option.state & QStyle::State_Selected) {
        checkbox.state |= QStyle::State_HasFocus;
    }

    // 绘制复选框
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkbox, painter);
}

bool CheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // // 只处理鼠标点击和键盘事件
    // if (event->type() == QEvent::MouseButtonPress ||
    //     event->type() == QEvent::MouseButtonRelease ||
    //     event->type() == QEvent::KeyPress) {

    //     // 对于鼠标事件，检查点击是否在复选框区域内
    //     if (event->type() == QEvent::MouseButtonPress ||
    //         event->type() == QEvent::MouseButtonRelease) {

    //         QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

    //         // 计算复选框区域（与paint方法中相同）
    //         QSize size = QApplication::style()->sizeFromContents(QStyle::CT_CheckBox, &checkbox, QSize());
    //         QRect checkRect(option.rect.x() + (option.rect.width() - size.width()) / 2,
    //                         option.rect.y() + (option.rect.height() - size.height()) / 2,
    //                         size.width(), size.height());

    //         // 如果点击不在复选框区域内，不处理事件
    //         if (!checkRect.contains(mouseEvent->pos())) {
    //             return false;
    //         }

    //         // 只有在鼠标释放时才切换状态
    //         if (event->type() == QEvent::MouseButtonRelease) {
    //             bool checked = model->data(index, Qt::CheckStateRole).toInt() == Qt::Checked;
    //             model->setData(index, checked ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);
    //             return true;
    //         }
    //     }
    //     // 处理键盘事件（如空格键切换）
    //     else if (event->type() == QEvent::KeyPress) {
    //         QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    //         if (keyEvent->key() == Qt::Key_Space) {
    //             bool checked = model->data(index, Qt::CheckStateRole).toInt() == Qt::Checked;
    //             model->setData(index, checked ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);
    //             return true;
    //         }
    //     }

    //     return true; // 事件已处理
    // }

    return false; // 事件未处理
}
