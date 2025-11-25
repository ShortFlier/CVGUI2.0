#ifndef OPERATORVIEW_H
#define OPERATORVIEW_H

#include <QWidget>
#include <QGraphicsView>

#include "operatorscene.h"
#include "sceneproject.h"


/*
 * 算子工程视图，负责查看场景，接受拖拽事件
 * 一个主视图和一个副视图
 * 副视图是略缩图，在右下角，在副视图点击可以快速移动主视图
 */


class OpGraphicsView;


class OperatorView : public QWidget
{
public:
    explicit OperatorView(QWidget *parent = nullptr);

    void setScene(OperatorScene* scene);

    bool eventFilter(QObject *watched, QEvent *event) override;

    void newSceneProject();

protected:
    void resizeEvent(QResizeEvent* e)override;

private:
    OpGraphicsView* _mainView;
    QGraphicsView* _simpleView;

    SceneProject* _sceneProject=nullptr;

    QWidget* _mask;
};

class OpGraphicsView:public QGraphicsView{
    Q_OBJECT
public:
    explicit OpGraphicsView(QWidget* parent=nullptr);

    void setScene(OperatorScene* scene);

    //拖拽事件
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    OperatorScene* _scene;

    void parseMimeData(const QMimeData* mime, QString& className, QString& iconPath);
};

#endif // OPERATORVIEW_H
