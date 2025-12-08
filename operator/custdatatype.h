#ifndef CUSTDATATYPE_H
#define CUSTDATATYPE_H

#include <QString>
#include <vector>
#include <opencv2/opencv.hpp>

#define OPDATATYPE_OFFSET 1000

class OpDataType{
public:
    enum Type{ Int, Real, Mat, String, Point
    , IntArray=OPDATATYPE_OFFSET, RealArray, MatArray, StringArray, PointArray };
};

template<typename T>
using Array=std::vector<T>;

typedef int Int;
typedef Array<Int> IntArray;

typedef double Real;
typedef Array<Real> RealArray;

typedef cv::Mat Mat;
typedef Array<Mat> MatArray;

typedef QString String;
typedef Array<String> StringArray;

typedef cv::Point2i Point;
typedef Array<Point> PointArray;

#endif // CUSTDATATYPE_H
