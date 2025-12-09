#include "dataview.h"

DataView::DataView(QWidget *parent)
    : QScrollArea{parent}
{
    _content=new QWidget(this);

}
