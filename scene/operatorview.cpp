#include "operatorview.h"

#include <QResizeEvent>

OperatorView::OperatorView(QWidget *parent)
    : QWidget{parent}
{
    _mainView=new OpGraphicsView(this);
    _simpleView=new QGraphicsView(this);

    _mainView->move(0,0);

    _simpleView->resize((FIXED_SCENE_SIZE/10).toSize());
    _simpleView->scale(1.0*_simpleView->width()/FIXED_SCENE_WIDTH, 1.0*_simpleView->height()/FIXED_SCENE_HEIGHT);
    _simpleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _simpleView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void OperatorView::setScene(OperatorScene *scene)
{
    _mainView->setScene(scene);
    _simpleView->setScene(scene);
}


void OperatorView::resizeEvent(QResizeEvent *e)
{
    _mainView->resize(e->size());

    //移动到右下角
    int x=width()-_simpleView->width();
    int y=height()-_simpleView->height();
    _simpleView->move(x,y);
}

OpGraphicsView::OpGraphicsView(QWidget *parent)
    :QGraphicsView(parent)
{

}

void OpGraphicsView::setScene(OperatorScene *scene)
{
    _scene=scene;

    QGraphicsView::setScene(scene);
}

void OpGraphicsView::paintEvent(QPaintEvent *event)
{
    QRectF r=mapToScene(viewport()->rect()).boundingRect();
    _scene->viewRect(r);

    QGraphicsView::paintEvent(event);
}
