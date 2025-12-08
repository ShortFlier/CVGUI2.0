#ifndef RESULTVIEW_H
#define RESULTVIEW_H

#include <QTabWidget>


class ImageDisplayer;
class DataView;

/*
 * 算子结果显示
 * 显示图片和其他类型
 */
class ResultView : public QTabWidget
{
    Q_OBJECT
public:
    explicit ResultView(QWidget *parent = nullptr);

private:
    ImageDisplayer* _imgView=nullptr;
    DataView* _dataView=nullptr;

signals:
};

#endif // RESULTVIEW_H
