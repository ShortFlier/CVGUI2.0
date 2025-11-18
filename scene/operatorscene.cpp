#include "operatorscene.h"

#include <QGraphicsRectItem>

OperatorScene::OperatorScene(QObject *parent)
    : QGraphicsScene{parent}
{
    setSceneRect(FIXED_SCENE_RECT);

    _viewRect=new QGraphicsRectItem;
    addItem(_viewRect);

    QPen pen;
    pen.setWidth(10);
    pen.setColor(Qt::green);
    _viewRect->setPen(pen);
}

void OperatorScene::viewRect(QRectF rect)
{
    QPoint p(10,10);
    QRectF r(rect.topLeft()-p, rect.bottomRight()+p);

    _viewRect->setRect(r);
}
