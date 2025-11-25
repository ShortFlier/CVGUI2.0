#include "operatorscene.h"
#include "log.h"
#include "commongraphicsblock.h"
#include <QGraphicsRectItem>

OperatorScene::OperatorScene(OperatorScene *parent)
{
    _blockCtrl=new OpGraphicsBlockCtrl;

    setParentScene(parent);

    setSceneRect(FIXED_SCENE_RECT);

    _viewRect=new QGraphicsRectItem;
    addItem(_viewRect);

    QPen pen;
    pen.setWidth(10);
    pen.setColor(Qt::green);
    _viewRect->setPen(pen);
}

OperatorScene::~OperatorScene()
{
    delete _blockCtrl;
}

void OperatorScene::viewRect(QRectF rect)
{
    QPoint p(10,10);
    QRectF r(rect.topLeft()-p, rect.bottomRight()+p);

    _viewRect->setRect(r);
}

void OperatorScene::createItem(const QString &className, QPointF pos, const QString &iconPath)
{
    QString opName=className+QString::number(_nums);

    if(_blockCtrl->contains(opName)){
        tip_warn("算子名称重复："+opName);
    }else{
        OpGraphicsBlock* block=new CommonGraphicsBlock(opName,className,iconPath);
        addItem(block);
        block->setPos(pos);
        log_info("添加算子：{0}", opName.toStdString());
    }
    ++_nums;
}

