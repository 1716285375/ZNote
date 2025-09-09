#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QCheckBox>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

// 分辨率 / 音频 ComboBox Delegate
class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ComboBoxDelegate(QObject *parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override;

    void setItems(QStringList items);

    void setEditorData(QWidget* editor, const QModelIndex &index) const override;

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const override;

private:
    QStringList items;
};


#endif // COMBOBOXDELEGATE_H
