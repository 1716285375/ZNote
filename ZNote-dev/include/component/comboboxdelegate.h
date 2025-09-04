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
    ComboBoxDelegate(QObject *parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex &index) const override;

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const override;
};
// 字幕多选 CheckBox Delegate
class SubtitlesDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    SubtitlesDelegate(QObject *parent=nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) override;
};


#endif // COMBOBOXDELEGATE_H
