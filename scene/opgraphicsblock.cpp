#include "opgraphicsblock.h"

#include <QFont>
#include <QFontMetrics>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QStyleOptionGraphicsItem>

const QList<QColor> OpGraphicsBlock::statusColors={Qt::black, Qt::blue, Qt::green, Qt::red};

OpGraphicsBlock::OpGraphicsBlock(const QString &opName, const QString &className, const QString &iconPath)
:_opName(opName),_className(className){
    _status=No;


    setFlag(ItemIsMovable, true);
    setFlag(ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);

    _leftDot=new OpGraphicsDot(QRectF(-GRAPHICSDOT_RADIUS, -GRAPHICSDOT_RADIUS, 2*GRAPHICSDOT_RADIUS, 2*GRAPHICSDOT_RADIUS), this);
    _rightDot=new OpGraphicsDot(QRectF(-GRAPHICSDOT_RADIUS, -GRAPHICSDOT_RADIUS, 2*GRAPHICSDOT_RADIUS, 2*GRAPHICSDOT_RADIUS), this);
    _topDot=new OpGraphicsDot(QRectF(-GRAPHICSDOT_RADIUS, -GRAPHICSDOT_RADIUS, 2*GRAPHICSDOT_RADIUS, 2*GRAPHICSDOT_RADIUS), this);
    _bottomDot=new OpGraphicsDot(QRectF(-GRAPHICSDOT_RADIUS, -GRAPHICSDOT_RADIUS, 2*GRAPHICSDOT_RADIUS, 2*GRAPHICSDOT_RADIUS), this);

    setDotVisible(false);

    _text=new OpGraphicsText(this);
    _text->setText(opName);
    connect(_text, &OpGraphicsText::textChanged, this, &OpGraphicsBlock::tryChangeOpName);
}

void OpGraphicsBlock::setOpName(const QString &name)
{
    _opName=name;
    _text->setText(_opName);
}

QPointF OpGraphicsBlock::getStartPos(OpGraphicsDot *dot)
{
    QPointF pos=dot->center();
    if(dot==_leftDot){
        pos+=QPointF(-2*GRAPHICSDOT_RADIUS, 0);
    }else if(dot==_rightDot){
        pos+=QPointF(2*GRAPHICSDOT_RADIUS, 0);
    }else  if(dot==_topDot){
        pos+=QPointF(0, -2*GRAPHICSDOT_RADIUS);
    }else  if(dot==_bottomDot){
        pos+=QPointF(0, 2*GRAPHICSDOT_RADIUS);
    }else
        return pos;
    return pos;
}

void OpGraphicsBlock::setDotVisible(bool b)
{
    _leftDot->setVisible(b);
    _rightDot->setVisible(b);
    _topDot->setVisible(b);
    _bottomDot->setVisible(b);
}

bool OpGraphicsBlock::setStatus(int status)
{
    if(status==Success||status==Error){
        _status=status;
        return true;
    }else if(_status==No||_status==Active){
        _status=status;
        return true;
    }else
        return false;
}

void OpGraphicsBlock::setStatusFocus(int status)
{
    if(status>=0&&status<=3)
        _status=status;
}

void OpGraphicsBlock::move(qreal dx, qreal dy)
{
    setPos(scenePos()+QPointF(dx,dy));
}

QVariant OpGraphicsBlock::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change==GraphicsItemChange::ItemPositionHasChanged){
        QPointF newScenePos=value.toPointF();
        emit moved(newScenePos);
    }

    return QGraphicsItem::itemChange(change, value);
}

void OpGraphicsBlock::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction* rename=menu.addAction("重命名");
    QAction* dlt=menu.addAction("删除");
    QAction* act=menu.exec(event->screenPos());
    if(act==rename){
        _text->setTextInteractionFlags(Qt::TextEditorInteraction);
        _text->setFocus();
    }else if(act==dlt){
        emit deleted(_opName);
    }else{

    }
}

void OpGraphicsBlock::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit doubleClicked(_opName);

    QGraphicsItem::mouseDoubleClickEvent(event);
}


void OpGraphicsBlock::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
        setCursor(Qt::SizeAllCursor);

    emit clicked(_opName);

    QGraphicsItem::mousePressEvent(event);
}

void OpGraphicsBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);

    QGraphicsItem::mouseReleaseEvent(event);
}

void OpGraphicsBlock::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setStatus(Active);
    setDotVisible(true);

    QGraphicsItem::hoverEnterEvent(event);
}

void OpGraphicsBlock::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setStatus(No);
    setDotVisible(false);

    QGraphicsItem::hoverLeaveEvent(event);
}


OpGraphicsBlock::~OpGraphicsBlock()
{

}

OpGraphicsDot::OpGraphicsDot(const QRectF &rect, QGraphicsItem *parent)
    :QGraphicsEllipseItem(rect, parent){
    setCursor(Qt::PointingHandCursor);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    QBrush brush;
    brush.setColor(Qt::blue);
    brush.setStyle(Qt::SolidPattern);
    setBrush(brush);
}

void OpGraphicsDot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem styleOption = *option;
    styleOption.state &= ~QStyle::State_Selected;

    QGraphicsEllipseItem::paint(painter, &styleOption, widget);
}

QPointF OpGraphicsDot::center()
{
    return mapToScene(pos());
}

OpGraphicsBlock *OpGraphicsDot::block()
{
    return static_cast<OpGraphicsBlock*>(parentItem());
}


OpGraphicsText::OpGraphicsText(QGraphicsItem *parent, int w)
    :QGraphicsTextItem(parent){
    setTextWidth(w);
}

void OpGraphicsText::setText(const QString &text)
{
    _srcText=text;
    setToolTip(_srcText);
    QFontMetrics fm(font());
    QString showText=fm.elidedText(_srcText, Qt::ElideRight, textWidth()-fm.horizontalAdvance("..."));
    setPlainText(showText);
}

void OpGraphicsText::focusOutEvent(QFocusEvent *event)
{
    QString newText=toPlainText();
    if(newText!=_srcText){
        emit textChanged(_srcText, newText);
    }
    reRender();
    setTextInteractionFlags(Qt::NoTextInteraction);

    QGraphicsTextItem::focusOutEvent(event);
}

void OpGraphicsText::focusInEvent(QFocusEvent *event)
{
    setPlainText(_srcText);

    QGraphicsTextItem::focusInEvent(event);
}

