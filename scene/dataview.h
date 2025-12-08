#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <QScrollArea>

/*
 * 树形结构显示数据结果
 * 自动展开数组
 */

class DataView : public QScrollArea
{
    Q_OBJECT
public:
    explicit DataView(QWidget *parent = nullptr);

signals:
};

#endif // DATAVIEW_H
