#ifndef UTILITY_H
#define UTILITY_H

#include "log.h"

#include <QImage>
#include <QPointF>
#include <QString>
#include <string>

#include <opencv2/core/mat.hpp>

static double distance(const QPointF& p1, const QPointF& p2){
    return std::sqrt(std::pow(p2.x() - p1.x(), 2) + std::pow(p2.y() - p1.y(), 2));
}

//QString转换为可以被c++读取文件使用的std::strings
inline std::string qtcvstring(const std::string& str){
    return QString(str.c_str()).toLocal8Bit().toStdString();
}
inline std::string qtcvstring(const QString& str){
    return str.toLocal8Bit().toStdString();
}
inline std::string qtcvstring(const char* str){
    return QString(str).toLocal8Bit().toStdString();
}


static QImage MatToQImage(const cv::Mat& mat) {
    // 检查 Mat 是否为空
    if (mat.empty()) {
        log_warn("MatToQImage：输入mat为空");
        return QImage();
    }
    // 从 cv::Mat 生成 QImage
    switch (mat.type()) {
    case CV_8UC1: { // 单通道灰度图像
        return QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_Grayscale8);
    }
    case CV_8UC3: { // 三通道 BGR 图像
        // OpenCV 使用 BGR 格式，Qt 使用 RGB 格式，因此需要转换
        return QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_RGB888).rgbSwapped();
    }
    case CV_8UC4: { // 四通道 BGRA 图像
        return QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_ARGB32);
    }
    default: // 不支持的格式
        log_error("MatToQImage：不支持的格式。仅支持CV_8U,C1、C3、C4. 当前格式：CV_{0}TC{1}",mat.depth()/mat.channels(),mat.channels());
        return QImage();
    }
}

#endif // UTILITY_H
