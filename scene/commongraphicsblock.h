#ifndef COMMONGRAPHICSBLOCK_H
#define COMMONGRAPHICSBLOCK_H

#include "opgraphicsblock.h"

/*
 * 长方形的图形项块
 * 连线点在每条边的中间
 */
class CommonGraphicsBlock:public OpGraphicsBlock
{
public:
    explicit CommonGraphicsBlock(const QString& opName, const QString& className, const QString& iconPath);

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    bool setStatus(int status)override;
    void setStatusFocus(int status)override;

protected:

private:
    QString _iconPath;

    QList<OpGraphicsDot*> _dots;
};


#endif // COMMONGRAPHICSBLOCK_H
