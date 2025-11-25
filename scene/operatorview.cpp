#include "operatorview.h"

#include <QMimeData>
#include <QResizeEvent>

OperatorView::OperatorView(QWidget *parent)
    : QWidget{parent}
{
    _mainView=new OpGraphicsView(this);
    _simpleView=new QGraphicsView(this);

    _mainView->move(0,0);

    _mask=new QWidget(this);
    _mask->installEventFilter(this);

    _simpleView->resize((FIXED_SCENE_SIZE/10).toSize());
    _simpleView->scale(1.0*_simpleView->width()/FIXED_SCENE_WIDTH, 1.0*_simpleView->height()/FIXED_SCENE_HEIGHT);

    _simpleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _simpleView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _simpleView->setInteractive(false);


    newSceneProject();
    setScene(_sceneProject->subProjects().begin().value());
}

void OperatorView::setScene(OperatorScene *scene)
{
    _mainView->setScene(scene);
    _simpleView->setScene(scene);
}

bool OperatorView::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type()==QEvent::MouseButtonPress){
        QMouseEvent* e=static_cast<QMouseEvent*>(event);
        if(e && e->button()==Qt::LeftButton){
            _mainView->centerOn(_simpleView->mapToScene(e->pos()));
        }
    }
    if(event->type()==QEvent::MouseMove){
        QMouseEvent* e=static_cast<QMouseEvent*>(event);
        if(e->buttons()&Qt::LeftButton)
            _mainView->centerOn(_simpleView->mapToScene(e->pos()));
    }

    return true;
}

void OperatorView::newSceneProject()
{
    if(_sceneProject){
        _sceneProject->deleteLater();
        _sceneProject=nullptr;
    }
    _sceneProject=new SceneProject;
}


void OperatorView::resizeEvent(QResizeEvent *e)
{
    _mainView->resize(e->size());

    //移动到右下角
    int x=width()-_simpleView->width();
    int y=height()-_simpleView->height();
    _simpleView->move(x,y);

    _mask->resize(e->size());
    _mask->move(x,y);
}

OpGraphicsView::OpGraphicsView(QWidget *parent)
    :QGraphicsView(parent)
{
    setAcceptDrops(true);
}

void OpGraphicsView::setScene(OperatorScene *scene)
{
    _scene=scene;

    QGraphicsView::setScene(scene);
}

void OpGraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* mime = event->mimeData();
    if(mime->hasFormat("application/x-qabstractitemmodeldatalist")){
        event->acceptProposedAction();
    }
}

void OpGraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    const QMimeData* mime = event->mimeData();
    if(mime->hasFormat("application/x-qabstractitemmodeldatalist")){
        event->acceptProposedAction();
    }
}

void OpGraphicsView::dropEvent(QDropEvent *event)
{
    const QMimeData* mime = event->mimeData();
    QString className;
    QString iconPath;
    parseMimeData(mime, className, iconPath);
    QPointF scenePos=mapToScene(event->position().toPoint());
    _scene->createItem(className, scenePos, iconPath);
}

void OpGraphicsView::paintEvent(QPaintEvent *event)
{
    QRectF r=mapToScene(viewport()->rect()).boundingRect();
    _scene->viewRect(r);

    QGraphicsView::paintEvent(event);
}

void OpGraphicsView::parseMimeData(const QMimeData *mimeData, QString &className, QString &iconPath)
{
    //解析 QTableWidget 的特殊格式
    if (mimeData->hasFormat("application/x-qabstractitemmodeldatalist")) {
        QByteArray itemData = mimeData->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&itemData, QIODevice::ReadOnly);

        // 解析流数据
        while (!stream.atEnd()) {
            int row, col;
            QMap<int, QVariant> roleDataMap;

            stream >> row >> col >> roleDataMap;

            // 获取显示文本
            if (roleDataMap.contains(Qt::DisplayRole)) {
                className = roleDataMap[Qt::DisplayRole].toString();
            }
            //获取icon路径
            if (roleDataMap.contains(Qt::UserRole+1)) {
                iconPath = roleDataMap[Qt::UserRole+1].toString();
            }
        }
    }
}
