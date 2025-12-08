#ifndef CUSTDATA_H
#define CUSTDATA_H


#include "custdatatype.h"



class CustDataBase{
public:
    virtual ~CustDataBase(){}
    virtual int type()=0;
};



template<int _type, typename Type>
class CustData{
public:
    int type(){
        return _type;
    }

    Type data(){
        return _data;
    }

private:
    Type _data;
};



typedef CustData<OpDataType::Int, Int> CustData_Int;
typedef CustData<OpDataType::IntArray, IntArray> CustData_IntArray;

typedef CustData<OpDataType::Real, Real> CustData_Real;
typedef CustData<OpDataType::RealArray, RealArray> CustData_RealArray;

typedef CustData<OpDataType::Mat, Mat> CustData_Mat;
typedef CustData<OpDataType::MatArray, MatArray> CustData_MatArray;

typedef CustData<OpDataType::String, String> CustData_String;
typedef CustData<OpDataType::StringArray, StringArray> CustData_StringArray;

typedef CustData<OpDataType::Point, Point> CustData_Point;
typedef CustData<OpDataType::PointArray, PointArray> CustData_PointArray;

#endif // CUSTDATA_H
