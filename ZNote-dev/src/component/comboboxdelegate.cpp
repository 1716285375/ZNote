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

SubtitlesDelegate::SubtitlesDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

void SubtitlesDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStringList subs = index.data(Qt::UserRole).toStringList();
    QStyleOptionViewItem opt = option;
    QApplication::style()->drawItemText(painter, opt.rect, Qt::AlignLeft|Qt::AlignVCenter, opt.palette, true, subs.join(", "));
}

bool SubtitlesDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() != QEvent::MouseButtonRelease)
        return false;

    QStringList allSubs = index.data(Qt::UserRole).toStringList();

    // 转成 QSet 用于快速判断
    QSet<QString> selectedSubs = QSet<QString>(index.data(Qt::EditRole).toStringList().begin(),
                                               index.data(Qt::EditRole).toStringList().end());

    // 简单切换选择状态（点击就翻转）
    for (const QString &s : allSubs) {
        if (selectedSubs.contains(s))
            selectedSubs.remove(s);
        else
            selectedSubs.insert(s);
    }

    // QSet -> QStringList
    QStringList result = selectedSubs.values();
    model->setData(index, result, Qt::EditRole);

    return true;
}
