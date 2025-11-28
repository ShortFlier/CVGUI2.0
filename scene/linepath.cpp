#include "linepath.h"
#include "utility.h"

#include <QPen>
#include <QRandomGenerator>




LinePath::LinePath(QGraphicsItem *parent)
:QGraphicsPathItem(parent){

}

LinePath::LinePath(const Dependency &depend, QGraphicsItem *parent)
    :QGraphicsPathItem(parent), _depend(depend) {

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
    LinePath* linePath=getLinePath(dot, end);
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
    path.moveTo(dot->center());

    path.lineTo(dotStart);
    path.moveTo(dotStart);

    path.lineTo(p1);
    path.moveTo(p1);

    path.lineTo(p2);
    path.moveTo(p2);

    path.lineTo(p3);
    path.moveTo(p3);

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


