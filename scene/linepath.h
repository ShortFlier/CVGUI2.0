#ifndef LINEPATH_H
#define LINEPATH_H

#include <QGraphicsPathItem>

#include <opgraphicsblock.h>
#include <unordered_set>


//表示算子连线从from到to
struct Dependency{

    Dependency(){}
    Dependency(OpGraphicsBlock* from, OpGraphicsBlock* to):from(from), to(to){}

    bool operator==(const Dependency& dp);
    bool contain(const OpGraphicsBlock* block);
    bool operator<(const Dependency& dp)const;

    OpGraphicsBlock* from;
    OpGraphicsBlock* to;
};


#define VALID_COLOR Qt::blue
#define INVALID_COLOR Qt::red

class LinePathCtrl;
//在Scene中显示的连线
class LinePath:public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    LinePath(QGraphicsItem *parent = nullptr);
    LinePath(const Dependency& dp, QGraphicsItem *parent = nullptr);
    Dependency _dp;

    //从block绘制到指定点
    static LinePath* getLinePath(OpGraphicsBlock* block, QPointF scenePos, QGraphicsScene* scene);
    //创建一条给定dp的线
    static LinePath* getLinePath(const Dependency& dp, const QColor color=VALID_COLOR);

signals:
    void deleted(const Dependency& dp);
};


/*
 * 连线管理器，负责连线生命周期管理
 */
class LinePathCtrl:public QMap<Dependency, LinePath*>{
public:
    //如果dp非法，两个相同，与原有的形成回路，返回null
    LinePath* add(const Dependency& dp);
    LinePath* deleted(const Dependency& dp);

    //不能相同，形成回路
    bool isValid(const Dependency& dp);

private:
    // 辅助函数：DFS 检查是否存在从 from 到 to 的路径
    bool hasPath(OpGraphicsBlock* from, OpGraphicsBlock* to,
                 std::unordered_set<OpGraphicsBlock*>& visited);
};

#endif // LINEPATH_H
