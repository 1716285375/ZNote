#include "ui/widgets/historywidget.h"
#include "ui/widgets/historywidget.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>

HistoryWidget::HistoryWidget(QWidget *parent)
    : QWidget(parent)
    , m_model(nullptr)
{
    setupUI();
    setupConnections();
}

HistoryWidget::~HistoryWidget()
{
}

void HistoryWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // 工具栏
    m_toolbarLayout = new QHBoxLayout();
    m_refreshButton = new QPushButton("刷新", this);
    m_clearButton = new QPushButton("清空", this);
    m_deleteButton = new QPushButton("删除选中", this);
    
    m_toolbarLayout->addWidget(m_refreshButton);
    m_toolbarLayout->addWidget(m_clearButton);
    m_toolbarLayout->addWidget(m_deleteButton);
    m_toolbarLayout->addStretch();
    
    // 搜索和过滤
    m_filterLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索...");
    m_statusFilter = new QComboBox(this);
    m_statusFilter->addItems({"全部", "成功", "失败", "进行中"});
    m_typeFilter = new QComboBox(this);
    m_typeFilter->addItems({"全部", "视频", "音频", "播放列表"});
    m_dateFromEdit = new QDateEdit(this);
    m_dateToEdit = new QDateEdit(this);
    
    m_filterLayout->addWidget(new QLabel("搜索:", this));
    m_filterLayout->addWidget(m_searchEdit);
    m_filterLayout->addWidget(new QLabel("状态:", this));
    m_filterLayout->addWidget(m_statusFilter);
    m_filterLayout->addWidget(new QLabel("类型:", this));
    m_filterLayout->addWidget(m_typeFilter);
    m_filterLayout->addWidget(new QLabel("从:", this));
    m_filterLayout->addWidget(m_dateFromEdit);
    m_filterLayout->addWidget(new QLabel("到:", this));
    m_filterLayout->addWidget(m_dateToEdit);
    
    // 统计信息
    m_statsGroup = new QGroupBox("统计信息", this);
    QHBoxLayout *statsLayout = new QHBoxLayout(m_statsGroup);
    m_totalLabel = new QLabel("总计: 0", this);
    m_successLabel = new QLabel("成功: 0", this);
    m_failedLabel = new QLabel("失败: 0", this);
    
    statsLayout->addWidget(m_totalLabel);
    statsLayout->addWidget(m_successLabel);
    statsLayout->addWidget(m_failedLabel);
    statsLayout->addStretch();
    
    // 历史记录表格
    m_historyView = new QTableView(this);
    m_historyView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_historyView->setAlternatingRowColors(true);
    m_historyView->setSortingEnabled(true);
    
    // 布局
    m_mainLayout->addLayout(m_toolbarLayout);
    m_mainLayout->addLayout(m_filterLayout);
    m_mainLayout->addWidget(m_statsGroup);
    m_mainLayout->addWidget(m_historyView);
}

void HistoryWidget::setupConnections()
{
    connect(m_refreshButton, &QPushButton::clicked, this, &HistoryWidget::onRefreshClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &HistoryWidget::onClearClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &HistoryWidget::onDeleteClicked);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &HistoryWidget::onSearchChanged);
    connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HistoryWidget::onFilterChanged);
    connect(m_typeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HistoryWidget::onFilterChanged);
    connect(m_historyView, &QTableView::doubleClicked, this, &HistoryWidget::onHistoryDoubleClicked);
}

void HistoryWidget::setModel(HistoryModel *model)
{
    m_model = model;
    m_historyView->setModel(model);
    
    if (model) {
        // 设置列宽
        m_historyView->horizontalHeader()->setStretchLastSection(true);
        m_historyView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_historyView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        m_historyView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        m_historyView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        
        updateStatistics();
    }
}

void HistoryWidget::refreshHistory()
{
    if (m_model) {
        m_model->refresh();
        updateStatistics();
    }
}

void HistoryWidget::updateStatistics()
{
    if (!m_model) return;
    
    int total = m_model->rowCount();
    int success = 0;
    int failed = 0;
    
    // 统计成功和失败的数量
    for (int i = 0; i < total; ++i) {
        QModelIndex statusIndex = m_model->index(i, 2); // 假设状态在第2列
        QString status = m_model->data(statusIndex).toString();
        if (status == "成功") {
            success++;
        } else if (status == "失败") {
            failed++;
        }
    }
    
    m_totalLabel->setText(QString("总计: %1").arg(total));
    m_successLabel->setText(QString("成功: %1").arg(success));
    m_failedLabel->setText(QString("失败: %1").arg(failed));
}

void HistoryWidget::onRefreshClicked()
{
    refreshHistory();
}

void HistoryWidget::onClearClicked()
{
    if (QMessageBox::question(this, "确认", "确定要清空所有历史记录吗？", 
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (m_model) {
            m_model->clearHistory();
            updateStatistics();
        }
    }
}

void HistoryWidget::onDeleteClicked()
{
    QModelIndexList selected = m_historyView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择要删除的记录");
        return;
    }
    
    if (QMessageBox::question(this, "确认", QString("确定要删除选中的 %1 条记录吗？").arg(selected.size()), 
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (m_model) {
            // 从后往前删除，避免索引变化
            std::sort(selected.begin(), selected.end(), [](const QModelIndex &a, const QModelIndex &b) {
                return a.row() > b.row();
            });
            
            for (const QModelIndex &index : selected) {
                m_model->removeHistory(index.row());
            }
            updateStatistics();
        }
    }
}

void HistoryWidget::onSearchChanged()
{
    // 搜索功能：根据输入文本过滤历史记录
    // 当前版本暂未实现，保留接口供后续扩展
    Q_UNUSED(m_searchEdit->text())
}

void HistoryWidget::onFilterChanged()
{
    // 过滤功能：根据状态、类型和日期范围过滤历史记录
    // 当前版本暂未实现，保留接口供后续扩展
}

void HistoryWidget::onHistoryDoubleClicked(const QModelIndex &index)
{
    if (!m_model) return;
    
    // 双击功能：打开对应的文件或文件夹
    // 当前版本暂未实现，保留接口供后续扩展
    Q_UNUSED(index)
}

