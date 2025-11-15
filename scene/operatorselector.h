#ifndef OPERATORSELECTOR_H
#define OPERATORSELECTOR_H

#include <QToolBox>

/*
 * 界面左侧的算子选择器
 * 将算子分为若干栏位
 * 拖拽到界面中间的视图中以添加算子
 *
 * 随着宽度变化自动排列算子列表
 */

class OperatorSelector : public QToolBox
{
public:
    explicit OperatorSelector(QWidget *parent = nullptr);

};

#endif // OPERATORSELECTOR_H
