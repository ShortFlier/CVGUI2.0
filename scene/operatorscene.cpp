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

void OperatorScene::setParentScene(OperatorScene *parent){
    _parent=parent;
    setParent(parent);
}

OpGraphicsBlock *OperatorScene::getBlockByPos(const QPointF &scenePos)
{
    QGraphicsItem *item=itemAt(scenePos, QTransform());
    if(item==_viewRect)
        return nullptr;
    //不为LinePath
    LinePath* line=dynamic_cast<LinePath*>(item);
    if(!line && item){
        while(item->parentItem()){
            item=item->parentItem();
        }
        return static_cast<OpGraphicsBlock *>(item);
    }else
        return nullptr;
}

LinePathCtrl &OperatorScene::lineCtrl(){
    return _lineCtrl;
}

OpGraphicsBlockCtrl &OperatorScene::blockCtrl(){
    return *_blockCtrl;
}

OperatorScene *OperatorScene::parentScene(){
    return _parent;
}

QMap<QString, OperatorScene *> &OperatorScene::childrenScenes(){
    return _childrens;
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
    OpGraphicsBlock* block=static_cast<OpGraphicsBlock*>(sender());
    _lineCtrl.reLine(block);
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
            emit showScene(_parent);
        }
    }
}

void OperatorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(_isLine){
        if(_tempLine)
            removeItem(_tempLine);
        delete _tempLine;

        QPointF scenePos=mouseEvent->scenePos();

        //如果pos上有block，且不为dot的block，绘制到block
        OpGraphicsBlock* block=getBlockByPos(scenePos);
        if(block && block!=_startDot->block())
            _tempLine=LinePath::getLinePath(_startDot, block);
        else
            _tempLine=LinePath::getLinePath(_startDot, scenePos);

        //合法性判断
        QPen pen=_tempLine->pen();
        pen.setColor(LINEPATH_INVALID_COLOR);
        if(!_lineCtrl.isValid(Dependency(_startDot->block(),block)))
            _tempLine->setPen(pen);

        addItem(_tempLine);
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void OperatorScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF scenePos=mouseEvent->scenePos();
    QGraphicsItem *item=itemAt(scenePos, QTransform());
    OpGraphicsDot* dot=dynamic_cast<OpGraphicsDot*>(item);
    if(dot){
        _isLine=true;
        _startDot=dot;
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void OperatorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(_isLine){
        QPointF scenePos=mouseEvent->scenePos();

        OpGraphicsBlock* block=getBlockByPos(scenePos);
        if(block){
            Dependency depend(_startDot->block(), block);
            LinePath* linePath=_lineCtrl.add(depend);
            if(linePath)
                addItem(linePath);
        }

        _isLine=false;
        removeItem(_tempLine);
        delete _tempLine;
        _tempLine=nullptr;
    }

    QGraphicsScene::mouseReleaseEvent(mouseEvent);
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
