#ifndef UTILITY_H
#define UTILITY_H

#include <QPointF>

static double distance(const QPointF& p1, const QPointF& p2){
    return std::sqrt(std::pow(p2.x() - p1.x(), 2) + std::pow(p2.y() - p1.y(), 2));
}


#endif // UTILITY_H
