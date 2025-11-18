#include "operatorselector.h"

#include<QHeaderView>
#include <QPainter>
#include <QResizeEvent>
#include <QVBoxLayout>

OperatorSelector::OperatorSelector(QWidget *parent)
    : QToolBox{parent}
{

}

void OperatorSelector::listOperator(const QList<OperatorInfo>& oinfos)
{
    //移除原有的
    setCurrentIndex(-1);
    for(int i=0; i<count(); i++){
        QWidget* w=widget(i);
        removeItem(i);
        w->deleteLater();
    }

    //分类
    QMap<QString, QList<OperatorInfo>> map;
    for(int i=0; i<oinfos.size(); ++i){
        map[oinfos[i].title].append(oinfos[i]);
    }

    for(auto itr=map.begin(); itr!=map.end(); ++itr){
        QList<OperatorInfo>& infos=itr.value();

        QList<QTableWidgetItem *> items;
        for(int i=0; i<infos.size(); ++i){
            QTableWidgetItem* item=new QTableWidgetItem;
            item->setText(infos[i].opName);

            QIcon icon(infos[i].iconPath);
            if(icon.isNull())
                icon=QIcon(DEFAULT_OPERATOR_ICON);
            item->setIcon(icon);

            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);

            items.append(item);
        }
        addItem(new OperatorTable(items, this), itr.key());
    }
}



OperatorTable::OperatorTable(const QList<QTableWidgetItem *>& items, QWidget *parent)
    :QTableWidget(parent), _items(items)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setDragEnabled(true);

    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);

    setShowGrid(false);

    IconTextDelegate *delegate = new IconTextDelegate(this);
    setItemDelegate(delegate);

}

OperatorTable::~OperatorTable()
{
    for(int i=0; i<_items.size(); ++i)
        delete _items[i];
}

void OperatorTable::resizeEvent(QResizeEvent *event)
{
    QTableWidget::resizeEvent(event);  // 调用父类方法

    int newCols = qMax(1, event->size().width() / FIXED_SIZE.width());
    int oldCols = qMax(1, event->oldSize().width() / FIXED_SIZE.width());

    if(newCols != oldCols) {
        updateLayout();
    }
}

void OperatorTable::startDrag(Qt::DropActions supportedActions)
{
    QTableWidgetItem *citem=currentItem();
    if(citem)
        QTableWidget::startDrag(supportedActions);
}

void OperatorTable::updateLayout()
{
    int cols = qMax(1, width() / FIXED_SIZE.width());
    int rows = (int)ceil(1.0 * _items.size() / cols);

    // 清空所有item，但保留所有权（重要！）
    for(int r = 0; r < rowCount(); ++r) {
        for(int c = 0; c < columnCount(); ++c) {
            takeItem(r, c);
        }
    }

    // 重新设置行列数
    setColumnCount(cols);
    setRowCount(rows);

    // 设置行高和列宽
    for(int r = 0; r < rows; ++r) {
        setRowHeight(r, FIXED_SIZE.height());
    }
    for(int c = 0; c < cols; ++c) {
        setColumnWidth(c, FIXED_SIZE.width());
    }

    // 添加items
    for(int i = 0; i < _items.size(); ++i) {
        int row = i / cols;
        int col = i % cols;

        QTableWidgetItem *clonedItem = _items[i];
        setItem(row, col, clonedItem);
    }
}



IconTextDelegate::IconTextDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void IconTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    // 绘制背景
    QTableWidget* tableWidget=qobject_cast<QTableWidget*>(parent());
    //被选中的cell有内容时高亮
    if ((option.state & QStyle::State_Selected) && tableWidget->itemFromIndex(index)) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }

    QVariant iconData = index.data(Qt::DecorationRole);
    QVariant textData = index.data(Qt::DisplayRole);

    QIcon icon = qvariant_cast<QIcon>(iconData);
    QString text = textData.toString();

    if (!icon.isNull() || !text.isEmpty()) {
        int padding = 3;
        int iconSize = 40;  // 图标大小，可根据需要调整

        // 计算图标位置（居中）
        int iconX = option.rect.left() + (option.rect.width() - iconSize) / 2;
        int iconY = option.rect.top() + padding;

        QRect iconRect(iconX, iconY, iconSize, iconSize);

        // 绘制图标
        if (!icon.isNull()) {
            painter->drawPixmap(iconRect, icon.pixmap(iconSize, iconSize));
        }

        // 计算文字位置
        QRect textRect(option.rect.left() + padding,
                       iconY + iconSize + padding,
                       option.rect.width() - 2 * padding,
                       option.rect.height() - iconSize - 2 * padding);

        // 绘制文字
        painter->setPen(option.palette.text().color());
        painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, text);
    }

    painter->restore();
}

QSize IconTextDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    // 设置较大的单元格尺寸以容纳图标和文字
    return FIXED_SIZE;  // 根据需要调整
}

