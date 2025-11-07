#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include "component/historymodel.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QGroupBox>
#include <QHeaderView>

class HistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryWidget(QWidget *parent = nullptr);
    ~HistoryWidget() override;

    void setModel(HistoryModel *model);
    void refreshHistory();

private slots:
    void onRefreshClicked();
    void onClearClicked();
    void onDeleteClicked();
    void onSearchChanged();
    void onFilterChanged();
    void onHistoryDoubleClicked(const QModelIndex &index);

private:
    void setupUI();
    void setupConnections();
    void updateStatistics();

    // UI组件
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_toolbarLayout;
    QHBoxLayout *m_filterLayout;
    
    // 工具栏
    QPushButton *m_refreshButton;
    QPushButton *m_clearButton;
    QPushButton *m_deleteButton;
    
    // 搜索和过滤
    QLineEdit *m_searchEdit;
    QComboBox *m_statusFilter;
    QComboBox *m_typeFilter;
    QDateEdit *m_dateFromEdit;
    QDateEdit *m_dateToEdit;
    
    // 统计信息
    QGroupBox *m_statsGroup;
    QLabel *m_totalLabel;
    QLabel *m_successLabel;
    QLabel *m_failedLabel;
    
    // 历史记录表格
    QTableView *m_historyView;
    HistoryModel *m_model;
};

#endif // HISTORYWIDGET_H

