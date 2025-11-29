#include "linepath.h"
#include "utility.h"

#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPen>
#include <QRandomGenerator>
#include <QStyle>
#include <QStyleOptionGraphicsItem>


LinePath::LinePath(QGraphicsItem *parent)
    :QGraphicsPathItem(parent){
    setFlag(ItemIsSelectable, true);
    setFlag(ItemIsFocusable, true);
}


LinePath::LinePath(const Dependency &depend, QGraphicsItem *parent)
    :LinePath(parent){
    _depend=depend;
}

void LinePath::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem styleOption = *option;
    styleOption.state &= ~QStyle::State_Selected;

    QGraphicsPathItem::paint(painter, &styleOption, widget);
}

void LinePath::reLine()
{
    LinePath* line=getLinePath(_depend);
    setPath(line->path());
    delete line;
}

LinePath *LinePath::getLinePath(OpGraphicsDot *dot, OpGraphicsBlock *block)
{
    OpGraphicsDot* xdot=block->getXDot(dot->center());
    OpGraphicsDot* ydot=block->getYDot(dot->center());

    OpGraphicsDot* near=distance(xdot->center(),dot->center())<distance(ydot->center(),dot->center())?xdot:ydot;
    QPointF end=block->getStartPos(near);
    //绘制到block的startPos
    LinePath* linePath=getLinePath(dot, end);
    //检查连线是否会在block内部
    if(block->contains(block->mapFromScene(linePath->_pathPoints.at(3)))){
        //重新获取block的绘制点
        delete linePath;
        QPointF npos(block->getStartPos(xdot).x(), block->getStartPos(ydot).y());
        linePath=getLinePath(dot, npos);
        QPainterPath path=linePath->path();
        linePath->lineTo(path, end);
        linePath->setPath(path);
    }

    //绘制end->near.center，带箭头
    QPainterPath path=linePath->path();
    drawArrow(path, end, near->center(), GRAPHICSDOT_RADIUS*2);

    linePath->setPath(path);
    return linePath;
}

LinePath *LinePath::getLinePath(const Dependency &depend)
{
    //选择dp.from上距离dp.to最近的dot
    QPointF target=depend.to->pos();
    OpGraphicsDot* xdot=depend.from->getXDot(target);
    OpGraphicsDot* ydot=depend.from->getYDot(target);

    OpGraphicsDot* near=distance(xdot->center(),target)<distance(ydot->center(),target)?xdot:ydot;

    LinePath* linePath=getLinePath(near, depend.to);
    linePath->_depend=depend;
    return linePath;
}

void LinePath::drawArrow(QPainterPath& path, const QPointF &from, const QPointF &to, double arrowSize, double arrowAngle) {
    // 绘制主线
    path.moveTo(from);
    path.lineTo(to);
    // 计算箭头方向
    double angle = std::atan2(to.y() - from.y(), to.x() - from.x());
    // 计算箭头两个翼点的位置
    QPointF arrowP1 = to - QPointF(
                          arrowSize * std::cos(angle - arrowAngle * M_PI / 180),
                          arrowSize * std::sin(angle - arrowAngle * M_PI / 180)
                          );
    QPointF arrowP2 = to - QPointF(
                          arrowSize * std::cos(angle + arrowAngle * M_PI / 180),
                          arrowSize * std::sin(angle + arrowAngle * M_PI / 180)
                          );
    // 绘制箭头翼
    path.moveTo(arrowP1);
    path.lineTo(to);
    path.moveTo(arrowP2);
    path.lineTo(to);
    path.moveTo(to);
}

void LinePath::focusInEvent(QFocusEvent *event)
{
    QPen p=pen();
    p.setColor(LINEPATH_ACTIVE_COLOR);
    setPen(p);

    QGraphicsPathItem::focusInEvent(event);
}

void LinePath::focusOutEvent(QFocusEvent *event)
{
    QPen p=pen();
    p.setColor(LINEPATH_VALID_COLOR);
    setPen(p);

    QGraphicsPathItem::focusOutEvent(event);
}

void LinePath::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent)
{
    QMenu menu;
    menu.addAction(QString("[%1]==>[%2]").arg(_depend.from->opName()).arg(_depend.to->opName()))->setDisabled(true);
    QAction* act=menu.addAction("删除");
    if(act==menu.exec(contextMenuEvent->screenPos())){
        emit deleted(_depend);
    }

    QGraphicsPathItem::contextMenuEvent(contextMenuEvent);
}

void LinePath::lineTo(QPainterPath& path, const QPointF &p)
{
    path.lineTo(p);
    path.moveTo(p);
    _pathPoints.append(p);
}

void LinePath::lineTo(QPainterPath &path, const QList<QPointF> &ps)
{
    path.moveTo(ps[0]);
    _pathPoints.append(ps[0]);
    for(int i=1;i<ps.size();++i){
        lineTo(path, ps[i]);
    }
}

LinePath *LinePath::getLinePath(OpGraphicsDot *dot, const QPointF& scenePos)
{
    LinePath* line=new LinePath;
    QPen pen;
    pen.setWidth(LINEPATH_WIDTH);
    pen.setColor(LINEPATH_VALID_COLOR);
    line->setPen(pen);
    /*
     * 连线原理
     * 获取block中x,y方向上最合适的xdot,ydot
     * 点(xdot.start.x, ydot.start.y)即为最合适中间点
     */
    OpGraphicsDot* xdot;
    OpGraphicsDot* ydot;

    OpGraphicsBlock* block=dot->block();
    if(block->inX(dot)){
        xdot=dot;
        ydot=block->getYDot(scenePos);
    }else{
        ydot=dot;
        xdot=block->getXDot(scenePos);
    }


    QPointF dotStart=block->getStartPos(dot);

    QPointF p1;
    p1.setX(block->getStartPos(xdot).x());
    p1.setY(block->getStartPos(ydot).y());
    QPointF p2;
    const QPointF& p3=scenePos;
    //p2为(xdot.start.x, p3.y)或(p3.x, ydot.start.y),选择不在线(dotStart, p1)上的点
    QPointF p21(p1.x(), p3.y());
    QPointF p22(p3.x(), p1.y());
    QPointF p=dotStart-p1;
    if(qFuzzyIsNull(p.x()))
        p2=p22;
    else
        p2=p21;

    //绘制线条：dot.center->dot.start->p1->p2->p3
    QPainterPath path;
    QList<QPointF> ps={dot->center(), dotStart, p1, p2, p3};
    line->lineTo(path, ps);

    line->setPath(path);
    return line;
}


bool Dependency::operator==(const Dependency &depend)
{
    return (from == depend.from && to == depend.to) || (from == depend.to && to == depend.from);
}

bool Dependency::contain(const OpGraphicsBlock *block)const
{
    return (from==block) || (to==block);
}

bool Dependency::operator<(const Dependency &depend)const
{
    // 使用 pair 的默认字典序比较
    auto pair1 = std::minmax(from, to);
    auto pair2 = std::minmax(depend.from, depend.to);
    return pair1 < pair2;
}

LinePath *LinePathCtrl::add(const Dependency &depend)
{
    LinePath* lp=nullptr;

    if(isValid(depend))
        lp=LinePath::getLinePath(depend);

    insert(depend, lp);
    return lp;
}

LinePath *LinePathCtrl::deleted(const Dependency &depend)
{
    return take(depend);
}


bool LinePathCtrl::isValid(const Dependency& depend)
{
    // 新增依赖后，检查是否形成回路
    // 使用 DFS 从 depend.to 出发，看能否到达 depend.from

    // 特殊情况：自环
    if (depend.from == depend.to) {
        return false;
    }

    // 检查是否已存在相同的依赖
    if (this->contains(depend)) {
        return false;  // 重复的依赖
    }

    // 使用 DFS 检测是否存在从 depend.to 到 depend.from 的路径
    std::unordered_set<OpGraphicsBlock*> visited;

    // 如果存在从 depend.to 到 depend.from 的路径，加入 depend 会形成回路
    return !hasPath(depend.to, depend.from, visited);
}

void LinePathCtrl::reLine(OpGraphicsBlock *block)
{
    for(auto itr=begin(); itr!=end(); ++itr){
        if(itr.key().contain(block)){
            itr.value()->reLine();
        }
    }
}

bool LinePathCtrl::hasPath(OpGraphicsBlock* from, OpGraphicsBlock* to,
                           std::unordered_set<OpGraphicsBlock*>& visited)
{
    if (from == to) {
        return true;
    }

    if (visited.count(from)) {
        return false;  // 已访问过，避免无限循环
    }

    visited.insert(from);

    // 遍历所有依赖，找出以 from 为起点的边
    for (auto it = this->begin(); it != this->end(); ++it) {
        const Dependency& dep = it.key();

        // 找出以 from 为起点的有向边
        if (dep.from == from) {
            if (hasPath(dep.to, to, visited)) {
                return true;
            }
        }
    }

    return false;
}


