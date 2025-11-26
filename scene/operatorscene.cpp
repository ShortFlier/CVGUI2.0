#include "operatorscene.h"
#include "log.h"
#include "commongraphicsblock.h"

#include <QMenu>
#include <QGraphicsRectItem>
#include <QGraphicsSceneContextMenuEvent>


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

    OpGraphicsBlock* block=_blockCtrl->addBlock(opName, className, iconPath);
    if(!block){
        tip_warn("算子名称重复："+opName);
    }else{
        addItem(block);
        block->setPos(pos);

        connect(block, &OpGraphicsBlock::tryChangeOpName, this, &OperatorScene::changeOpName);
        connect(block, &OpGraphicsBlock::deleted, this, &OperatorScene::deleteBlock);
        connect(block, &OpGraphicsBlock::moved, this, &OperatorScene::moveBlock);
        connect(block, &OpGraphicsBlock::clicked, this, &OperatorScene::clickBlock);
        connect(block, &OpGraphicsBlock::doubleClicked, this, &OperatorScene::doubleClickBlock);

        log_info("添加算子：{0}", opName.toStdString());
    }
    ++_nums;
}

void OperatorScene::changeOpName(const QString &oldName, const QString &newName)
{
    if(_blockCtrl->renameBlock(oldName, newName)){
        OpGraphicsBlock* block=static_cast<OpGraphicsBlock*>(sender());
        block->setOpName(newName);

        log_info("{0}重命名：{1}", oldName.toStdString(), newName.toStdString());
    }else{
        tip_warn("重命名失败，算子名称重复："+newName);
    }
}

void OperatorScene::deleteBlock(const QString &opName)
{
    OpGraphicsBlock* block=_blockCtrl->deleteBlock(opName);
    removeItem(block);

    log_info("删除算子：{0}", opName.toStdString());
    block->deleteLater();
}

void OperatorScene::moveBlock(const QPointF &newScenePos)
{

}

void OperatorScene::clickBlock(const QString &opName)
{

}

void OperatorScene::doubleClickBlock(const QString &opName)
{

}

void OperatorScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent)
{
    if(_parent){
        QMenu menu;
        QAction* act=menu.addAction("上一层");
        if(act==menu.exec(contextMenuEvent->screenPos())){
            emit upper();
        }
    }
}


OpGraphicsBlock *OpGraphicsBlockCtrl::addBlock(const QString &opName, const QString &className, const QString &iconPath)
{
    if(contains(opName)){
        return nullptr;
    }else{
        //根据className选择创建OpGraphicsBlock
        OpGraphicsBlock *block=new CommonGraphicsBlock(opName, className, iconPath);
        insert(opName, block);
        return block;
    }
}

OpGraphicsBlock* OpGraphicsBlockCtrl::deleteBlock(const QString &opName)
{
    return take(opName);
}

bool OpGraphicsBlockCtrl::renameBlock(const QString &oldName, const QString &newName)
{
    if(contains(newName)){
        return false;
    }else{
        OpGraphicsBlock* block=take(oldName);
        insert(newName, block);
        return true;
    }
}
