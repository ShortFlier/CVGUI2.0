#ifndef OPERATORVIEW_H
#define OPERATORVIEW_H

#include <QWidget>
#include <QGraphicsView>

#include "operatorscene.h"


/*
 * 算子工程视图，否则查看场景
 * 一个主视图和一个副视图
 * 副视图是略缩图，在右下角，在副视图点击可以快速移动主视图
 */


class OpGraphicsView;


class OperatorView : public QWidget
{
public:
    explicit OperatorView(QWidget *parent = nullptr);

    void setScene(OperatorScene* scene);

protected:
    void resizeEvent(QResizeEvent* e)override;

private:
    OpGraphicsView* _mainView;
    QGraphicsView* _simpleView;

};

class OpGraphicsView:public QGraphicsView{
    Q_OBJECT
public:
    explicit OpGraphicsView(QWidget* parent=nullptr);

    void setScene(OperatorScene* scene);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    OperatorScene* _scene;
};

#endif // OPERATORVIEW_H
