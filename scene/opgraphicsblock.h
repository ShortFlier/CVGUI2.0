#ifndef OPGRAPHICSBLOCK_H
#define OPGRAPHICSBLOCK_H

#include <QGraphicsObject>
#include <QGraphicsTextItem>


/*
 * 算子图像块基类
 * 具有移动，重命名，连线功能，当前状态显示
 * 鼠标左键按住可以拖动，移动时连线同步移动
 * 鼠标右键弹出菜单，可选择重命名、删除
 * 边框颜色显示状态
 * 上下左右四个连线点，鼠标左击拖动连线
 */
class OpGraphicsDot;
class OpGraphicsText;

class OpGraphicsBlock : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit OpGraphicsBlock(const QString& opName, const QString& className, const QString& iconPath);
    virtual ~OpGraphicsBlock()=0;

    QString opName(){return _opName;}
    void setOpName(const QString& name);

    QString className(){return _className;}

    OpGraphicsDot* leftDot(){return _leftDot;}
    OpGraphicsDot* rightDot(){return _rightDot;}
    OpGraphicsDot* topDot(){return _topDot;}
    OpGraphicsDot* bottomDot(){return _bottomDot;}

    //获取连线起始点
    virtual QPointF getStartPos(OpGraphicsDot* dot);

    bool inX(OpGraphicsDot* dot);
    bool inY(OpGraphicsDot* dot);
    //给定一个点，获取x,y方向最靠近点的dot
    OpGraphicsDot* getXDot(const QPointF& scenePos);
    OpGraphicsDot* getYDot(const QPointF& scenePos);

    virtual void setDotVisible(bool b);

    enum Status{No,Active,Success,Error};

    //设置优先级Success,Error>Active,No;如果已经是Success,Error设置为Active,No不会生效
    virtual bool setStatus(int status)=0;
    //强制设置
    virtual void setStatusFocus(int status)=0;

    static const QList<QColor> statusColors;

public slots:
    void move(qreal dx, qreal dy);

protected:
    //图形项移动发射moved信号
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)override;
    //右键菜单
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    //鼠标双击发射双击事件
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)override;

    //左键按下设置指针为移动样式，发射点击事件
    void mousePressEvent(QGraphicsSceneMouseEvent *event)override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)override;
    //鼠标和离开改变边框颜色
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event)override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event)override;

    int _status;

    QString _opName;
    QString _className;

    OpGraphicsDot* _leftDot;
    OpGraphicsDot* _rightDot;
    OpGraphicsDot* _topDot;
    OpGraphicsDot* _bottomDot;

    OpGraphicsText* _text;

signals:
    void tryChangeOpName(const QString& oldName, const QString& newName);
    void deleted(const QString& opName);
    void moved(const QPointF& newScenePos);
    void clicked(const QString& opName);
    void doubleClicked(const QString& opName);
};




/*
 * 可连线的点
 */

#define GRAPHICSDOT_RADIUS 5

class OpGraphicsDot:public QGraphicsEllipseItem{
public:
    explicit OpGraphicsDot(const QRectF &rect, QGraphicsItem *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    QPointF center();

    OpGraphicsBlock* block();
};

/*
 * 文字显示与编辑
 */
#define GRAPHICSTEXT_WIDTH 100
class OpGraphicsText:public QGraphicsTextItem{
    Q_OBJECT
public:
    explicit OpGraphicsText(QGraphicsItem *parent = nullptr, int w=GRAPHICSTEXT_WIDTH);

    void setText(const QString& text);
    void reRender(){
        setText(_srcText);
    }
protected:
    void focusOutEvent(QFocusEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private:
    int _w;
    QString _srcText;

signals:
    void textChanged(const QString& oldText, const QString& newText);
};





#endif // OPGRAPHICSBLOCK_H
