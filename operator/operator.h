#ifndef OPERATOR_H
#define OPERATOR_H

#include "custdata.h"
#include "log.h"
#include "utility.h"

#include <QWidget>

#include <memory>


#define FLAG_INPUTDATASET 0
#define FLAG_OUTPUTDATASET 1

template<int _flag>
class InputOutputDataSet{
public:

    void insert(const QString& key, CustDataBase* data){
        _data.insert(key, std::shared_ptr<CustDataBase>(data));
    }

    template<typename Type>
    void setData(const QString& key, const Type& data){
        if(_data.contains(key)){
            std::string str;
            if(_flag==FLAG_INPUTDATASET){
                str="InputDataSet";
            }else if(_flag==FLAG_OUTPUTDATASET){
                str="OutputDataSet";
            }else
                str="InputOutputDataSet";

            log_error("{0}::setData：不存在的键值{1}", str, qtcvstring(key));
        }else{
            _data.value(key)->setData(static_cast<void*>(&data));
        }
    }

    CustDataBase* getData(const QString& key){
        if(_data.contains(key)){
            std::string str;
            if(_flag==FLAG_INPUTDATASET){
                str="InputDataSet";
            }else if(_flag==FLAG_OUTPUTDATASET){
                str="OutputDataSet";
            }else
                str="InputOutputDataSet";

            log_error("{0}::getData：不存在的键值{1}", str, qtcvstring(key));
        }

        return _data.value(key).get();
    }

private:
    QMap<QString, std::shared_ptr<CustDataBase>> _data;
};

typedef InputOutputDataSet<FLAG_INPUTDATASET> InputDataSet;
typedef InputOutputDataSet<FLAG_OUTPUTDATASET> OutputDataSet;

/*
 * 算子基类
 * run，运行算子
 * output，返回算子输出
 */
class OperatorBase
{
public:
    OperatorBase();
    virtual ~OperatorBase();

    virtual void input(InputDataSet& inputData)=0;

    virtual void run()=0;

    virtual OutputDataSet* output(){
        return &_outputData;
    }

private:
    OutputDataSet _outputData;
};


#endif // OPERATOR_H


