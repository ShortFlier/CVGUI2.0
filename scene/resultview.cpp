#include "resultview.h"

#include "imagedisplayer.h"

#include "dataview.h"

ResultView::ResultView(QWidget *parent)
    : QTabWidget{parent}
{
    _imgView=new ImageDisplayer(this);
    _dataView=new DataView(this);

    addTab(_imgView, "图像");
    addTab(_dataView, "结果");
}

ImageDisplayer *ResultView::imgView()
{
    return _imgView;
}

DataView *ResultView::dataView()
{
    return _dataView;
}

