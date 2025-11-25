#include "commongraphicsblock.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QCursor>

#define COMMONBLOCK_ICON_SIZE 30
#define COMMONBLOCK_PADDING 5

CommonGraphicsBlock::CommonGraphicsBlock(const QString &opName, const QString &className, const QString &iconPath)
    :OpGraphicsBlock(opName, className, iconPath),_iconPath(iconPath){
    constexpr int w=COMMONBLOCK_PADDING+COMMONBLOCK_ICON_SIZE+COMMONBLOCK_PADDING+GRAPHICSTEXT_WIDTH+COMMONBLOCK_PADDING;
    constexpr int h=COMMONBLOCK_PADDING+COMMONBLOCK_ICON_SIZE+COMMONBLOCK_PADDING;

    QPointF tpf(-w/2.0+COMMONBLOCK_PADDING+COMMONBLOCK_ICON_SIZE+COMMONBLOCK_PADDING, -h/2.0+COMMONBLOCK_PADDING);
    _text->setPos(tpf);
    QFont font;
    font.setPointSize(12);
    _text->setFont(font);
    _text->reRender();

    _leftDot->setPos(-w/2.0+GRAPHICSDOT_RADIUS/2.0, 0);
    _rightDot->setPos(w/2.0-GRAPHICSDOT_RADIUS/2.0, 0);
    _topDot->setPos(0, -h/2.0+GRAPHICSDOT_RADIUS/2.0);
    _bottomDot->setPos(0, h/2.0-GRAPHICSDOT_RADIUS/2.0);

    _dots={_leftDot, _rightDot, _topDot, _bottomDot};

    setFlag(ItemIsMovable, true);
    setFlag(ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);
}

QRectF CommonGraphicsBlock::boundingRect() const
{
    constexpr int w=COMMONBLOCK_PADDING+COMMONBLOCK_ICON_SIZE+COMMONBLOCK_PADDING+GRAPHICSTEXT_WIDTH+COMMONBLOCK_PADDING;
    constexpr int h=COMMONBLOCK_PADDING+COMMONBLOCK_ICON_SIZE+COMMONBLOCK_PADDING;
    return QRectF(-w/2.0, -h/2.0, w, h);
}

void CommonGraphicsBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    QPixmap pixmap(_iconPath);
    pixmap = pixmap.scaled(COMMONBLOCK_ICON_SIZE, COMMONBLOCK_ICON_SIZE,
                           Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    constexpr int w=COMMONBLOCK_PADDING+COMMONBLOCK_ICON_SIZE+COMMONBLOCK_PADDING+GRAPHICSTEXT_WIDTH+COMMONBLOCK_PADDING;
    constexpr int h=COMMONBLOCK_PADDING+COMMONBLOCK_ICON_SIZE+COMMONBLOCK_PADDING;
    QPointF pf(-w/2.0+COMMONBLOCK_PADDING, -h/2.0+COMMONBLOCK_PADDING);
    painter->drawPixmap(pf, pixmap);
    //绘制矩形框
    QColor color=statusColors[_status];
    QPen pen;
    pen.setWidth(3);
    pen.setColor(color);
    painter->setPen(pen);
    painter->drawRect(-w/2.0, -h/2.0, w, h);

    painter->restore();
}


bool CommonGraphicsBlock::setStatus(int status)
{
    if(OpGraphicsBlock::setStatus(status)){
        update();
        return true;
    }
    return false;
}

void CommonGraphicsBlock::setStatusFocus(int status)
{
    OpGraphicsBlock::setStatusFocus(status);
    update();
}


QVariant CommonGraphicsBlock::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change==GraphicsItemChange::ItemPositionHasChanged){
        QPointF newScenePos=value.toPointF();
        emit moved(newScenePos);
    }

    return QGraphicsItem::itemChange(change, value);
}

void CommonGraphicsBlock::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
        setCursor(Qt::SizeAllCursor);
}

void CommonGraphicsBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    QGraphicsItem::mouseReleaseEvent(event);
}

void CommonGraphicsBlock::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setStatus(Active);
    setDotVisible(true);
}

void CommonGraphicsBlock::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setStatus(No);
    setDotVisible(false);
}

