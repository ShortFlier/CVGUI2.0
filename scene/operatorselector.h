#ifndef OPERATORSELECTOR_H
#define OPERATORSELECTOR_H

#include <QToolBox>
  #include <QTableWidget>
#include <QStyledItemDelegate>

/*
 * 界面左侧的算子选择器
 * 将算子分为若干栏位
 * 拖拽到界面中间的视图中以添加算子
 *
 * 随着宽度变化自动排列算子列表
 */

#define DEFAULT_OPERATOR_ICON ":/icon/operator/default.ico"
struct OperatorInfo{
    OperatorInfo(){}
    OperatorInfo(QString title, QString opName, QString iconPath=DEFAULT_OPERATOR_ICON):title(title), opName(opName), iconPath(iconPath){}
    QString title;
    QString opName;
    QString iconPath;
};

class OperatorSelector : public QToolBox
{
public:
    explicit OperatorSelector(QWidget *parent = nullptr);

    void listOperator(const QList<OperatorInfo>& oinfos);
};


/*
 * 表格形式显示算子
 * 宽度发生变化时，调整列数显示
 */
#define FIXED_SIZE QSize(60,60)
class OperatorTable:public QTableWidget{
public:
    explicit OperatorTable(const QList<QTableWidgetItem *>& _items, QWidget *parent = nullptr);
    ~OperatorTable();

protected:
    void resizeEvent(QResizeEvent *event) override;

    void startDrag(Qt::DropActions supportedActions) override;

private:
    void updateLayout();
    QList<QTableWidgetItem *> _items;
};


class IconTextDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit IconTextDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

};

#endif // OPERATORSELECTOR_H
