#ifndef OPERATORSCENE_H
#define OPERATORSCENE_H

#include "opgraphicsblock.h"
#include "linepath.h"

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

    void setParentScene(OperatorScene* parent);

    //尝试获取点上的OpGraphicsBlock
    OpGraphicsBlock* getBlockByPos(const QPointF& scenePos);

    LinePathCtrl& lineCtrl();
    OpGraphicsBlockCtrl& blockCtrl();

    OperatorScene* parentScene();
    QMap<QString, OperatorScene*>& childrenScenes();

public slots:
    void changeOpName(const QString& oldName, const QString& newName);
    void deleteBlock(const QString& opName);
    void moveBlock(const QPointF& newScenePos);
    void clickBlock(const QString& opName);
    //如果点击的是嵌套算子，显示
    void doubleClickBlock(const QString& opName);

protected:
    //返回上一层
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent) override;

    //连线
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)override;

private:
    QGraphicsRectItem* _viewRect;

    int _nums=0;

    OperatorScene* _parent=nullptr;
    QMap<QString, OperatorScene*> _childrens;

    OpGraphicsBlockCtrl* _blockCtrl=nullptr;

    LinePathCtrl _lineCtrl;

    //临时连线
    LinePath* _tempLine=nullptr;
    OpGraphicsDot* _startDot=nullptr;
    //是否连线
    bool _isLine=false;

signals:
    void showScene(QGraphicsScene* scene);
};

/*
 * 图像块生命周期管理器
 * 添加、删除、重命名
 */
class OpGraphicsBlockCtrl:public QMap<QString, OpGraphicsBlock*>{
public:
    OpGraphicsBlock* addBlock(const QString& opName, const QString &className, const QString &iconPath);
    OpGraphicsBlock* deleteBlock(const QString& opName);
    bool renameBlock(const QString& oldName, const QString& newName);
};

#endif // OPERATORSCENE_H
