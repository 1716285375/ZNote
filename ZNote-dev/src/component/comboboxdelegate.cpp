#include "component/comboboxdelegate.h"



ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    QComboBox* editor = new QComboBox(parent);
    const QVector<QString> items = index.data(Qt::UserRole).toStringList().toVector();
    for (const QString &s : items) editor->addItem(s);
    return editor;
}

void ComboBoxDelegate::setItems(QStringList items)
{

}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox* combo = qobject_cast<QComboBox*>(editor);
    combo->setCurrentIndex(index.data(Qt::EditRole).toInt());
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* combo = qobject_cast<QComboBox*>(editor);
    model->setData(index, combo->currentIndex(), Qt::EditRole);
}
