#ifndef OPERATORSCENE_H
#define OPERATORSCENE_H

#include "opgraphicsblock.h"
#include <QGraphicsScene>
#include <QDragEnterEvent>

/*
 *自定义图像场景类
 *负责场景元素的添加，删除，连线
 */
#define FIXED_SCENE_WIDTH 2000
#define FIXED_SCENE_HEIGHT 4000
#define FIXED_SCENE_SIZE QSizeF(FIXED_SCENE_WIDTH,FIXED_SCENE_HEIGHT)
#define FIXED_SCENE_RECT QRectF(QPointF(-FIXED_SCENE_WIDTH/2,-FIXED_SCENE_HEIGHT/2),FIXED_SCENE_SIZE)

class OpGraphicsBlockCtrl;

class OperatorScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit OperatorScene(OperatorScene* parent=nullptr);
    ~OperatorScene();

    //绘制可视矩形，以便在副视图显示
    void viewRect(QRectF rect);

    void createItem(const QString& className, QPointF pos, const QString& iconPath);

    void setParentScene(OperatorScene* parent){
        _parent=parent;
        setParent(parent);
    }

private:
    QGraphicsRectItem* _viewRect;

    int _nums=0;

    OperatorScene* _parent=nullptr;
    QMap<QString, OperatorScene*> _childrens;

    OpGraphicsBlockCtrl* _blockCtrl=nullptr;
signals:
};

/*
 * 图像块生命周期管理器
 * 添加、删除、重命名
 */
class OpGraphicsBlockCtrl:public QMap<QString, OpGraphicsBlock*>{
public:
};

#endif // OPERATORSCENE_H
