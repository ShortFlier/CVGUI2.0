#ifndef OPERATORSCENE_H
#define OPERATORSCENE_H

#include <QGraphicsScene>


/*
 *自定义图像场景类
 *复制场景元素的添加，删除，连线
 */
#define FIXED_SCENE_WIDTH 2000
#define FIXED_SCENE_HEIGHT 4000
#define FIXED_SCENE_SIZE QSizeF(FIXED_SCENE_WIDTH,FIXED_SCENE_HEIGHT)
#define FIXED_SCENE_RECT QRectF(QPointF(-FIXED_SCENE_WIDTH/2,-FIXED_SCENE_HEIGHT/2),FIXED_SCENE_SIZE)

class OperatorScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit OperatorScene(QObject *parent = nullptr);

    //绘制可视矩形，以便在副视图显示
    void viewRect(QRectF rect);

private:
    QGraphicsRectItem* _viewRect;

signals:
};

#endif // OPERATORSCENE_H
