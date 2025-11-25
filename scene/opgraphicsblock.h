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
    void setOpName(const QString& name){_opName=name;}

    QString className(){return _className;}

    OpGraphicsDot* leftDot();
    OpGraphicsDot* rightDot();
    OpGraphicsDot* topDot();
    OpGraphicsBlock* bottomDot();

    //获取连线起始点
    virtual QPointF getStartPos(OpGraphicsDot* dot);

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
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

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
