#ifndef LINEPATH_H
#define LINEPATH_H

#include <QGraphicsPathItem>

#include <opgraphicsblock.h>
#include <unordered_set>


//表示算子连线从from到to
struct Dependency{

    Dependency(){}
    Dependency(OpGraphicsBlock* from, OpGraphicsBlock* to):from(from), to(to){}

    bool operator==(const Dependency& depend);
    bool contain(const OpGraphicsBlock* block)const;
    bool operator<(const Dependency& depend)const;

    OpGraphicsBlock* from;
    OpGraphicsBlock* to;
};


#define LINEPATH_VALID_COLOR Qt::blue
#define LINEPATH_INVALID_COLOR Qt::red

#define LINEPATH_WIDTH 3

class LinePathCtrl;
//在Scene中显示的连线
class LinePath:public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    LinePath(QGraphicsItem *parent = nullptr);
    LinePath(const Dependency& depend, QGraphicsItem *parent = nullptr);
    Dependency _depend;

    //根据当前Dependency重新绘制连线
    void reLine();

    //从dot绘制到指定点
    static LinePath* getLinePath(OpGraphicsDot* dot, const QPointF& scenePos);
    //从dot绘制到block
    static LinePath* getLinePath(OpGraphicsDot* dot, OpGraphicsBlock* black);
    //创建一条给定dp的线
    static LinePath* getLinePath(const Dependency& depend);

    //绘制由from->to的箭头
    static void drawArrow(QPainterPath& path, const QPointF& from, const QPointF& to,
                          double arrowSize = 10.0, double arrowAngle = 30.0);

signals:
    void deleted(const Dependency& depend);
};


/*
 * 连线管理器，负责连线生命周期管理
 */
class LinePathCtrl:public QMap<Dependency, LinePath*>{
public:
    //如果dp非法:两个block相同,已存在,与原有的形成回路，返回null
    LinePath* add(const Dependency& depend);
    LinePath* deleted(const Dependency& depend);

    //不能相同，形成回路
    bool isValid(const Dependency& depend);

    //重绘所有block有关的path
    void reLine(OpGraphicsBlock* block);

private:
    // 辅助函数：DFS 检查是否存在从 from 到 to 的路径
    bool hasPath(OpGraphicsBlock* from, OpGraphicsBlock* to,
                 std::unordered_set<OpGraphicsBlock*>& visited);
};

#endif // LINEPATH_H
