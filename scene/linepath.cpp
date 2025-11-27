#include "operatorscene.h"

#define LINE_WIDTH 3

LinePath::LinePath(QGraphicsItem *parent)
:QGraphicsPathItem(parent){

}

LinePath::LinePath(const Dependency &dp, QGraphicsItem *parent)
    :QGraphicsPathItem(parent), _dp(dp) {

}

LinePath *LinePath::getLinePath(OpGraphicsBlock *block, QPointF scenePos, QGraphicsScene *scene)
{
    LinePath* line=nullptr;
    OperatorScene* opScene=static_cast<OperatorScene*>(scene);
    OpGraphicsBlock* target=opScene->getBlockByPos(scenePos);
    if(target){
        Dependency dp(block, target);
        if(opScene->lineCtrl().isValid(dp))
            line=getLinePath(dp);
        else
            line=getLinePath(dp, INVALID_COLOR);
    }else{

    }
    return line;
}

LinePath *LinePath::getLinePath(const Dependency &dp, const QColor color)
{
    LinePath* line=new LinePath(dp);
    QPen pen;
    pen.setWidth(LINE_WIDTH);
    pen.setColor(color);
    line->setPen(pen);

    return line;
}

bool Dependency::operator==(const Dependency &dp)
{
    return (from == dp.from && to == dp.to) || (from == dp.to && to == dp.from);
}

bool Dependency::contain(const OpGraphicsBlock *block)
{
    return (from==block) || (to==block);
}

bool Dependency::operator<(const Dependency &dp)const
{
    // 使用 pair 的默认字典序比较
    auto pair1 = std::minmax(from, to);
    auto pair2 = std::minmax(dp.from, dp.to);
    return pair1 < pair2;
}

LinePath *LinePathCtrl::add(const Dependency &dp)
{
    LinePath* lp=nullptr;

    if(isValid(dp))
        lp=LinePath::getLinePath(dp);

    insert(dp, lp);
    return lp;
}

LinePath *LinePathCtrl::deleted(const Dependency &dp)
{
    return take(dp);
}


bool LinePathCtrl::isValid(const Dependency& dp)
{
    // 新增依赖后，检查是否形成回路
    // 使用 DFS 从 dp.to 出发，看能否到达 dp.from

    // 特殊情况：自环
    if (dp.from == dp.to) {
        return false;
    }

    // 检查是否已存在相同的依赖
    if (this->contains(dp)) {
        return false;  // 重复的依赖
    }

    // 使用 DFS 检测是否存在从 dp.to 到 dp.from 的路径
    std::unordered_set<OpGraphicsBlock*> visited;

    // 如果存在从 dp.to 到 dp.from 的路径，加入 dp 会形成回路
    return !hasPath(dp.to, dp.from, visited);
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


