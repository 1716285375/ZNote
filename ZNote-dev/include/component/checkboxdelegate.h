#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QApplication>
#include <QStyleOptionButton>
#include <QMouseEvent>

class CheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CheckBoxDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) override;

};

#endif // CHECKBOXDELEGATE_H
