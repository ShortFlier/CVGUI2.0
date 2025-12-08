#include "imagedisplayer.h"

#include <QPushButton>
#include <QMimeData>
#include <QHeaderView>

#include "../utility.h"


static int findMedian(int a, int b, int c) {
    int arr[3]={a,b,c};
    std::sort(arr,arr+3);
    return arr[1];
}

ImgShow::ImgShow(const QSize &size, const cv::Mat &mat, QWidget *parent)
:QWidget(parent){
    //设置只身尺寸
    setMinimumSize(QSize(IMGSHOW_MIN_W,IMGSHOW_MIN_H));
    int w=MAX(IMGSHOW_MIN_W,size.width());
    int h=MAX(IMGSHOW_MIN_H,size.height());
    resize(w,h);
    setStyleSheet("background-color: #ffffff;");

    canve.setParent(this);
    canve.setMouseTracking(true);
    canve.installEventFilter(this);

    oldPoint=OLDPOINT;

    setImgMat(mat);
}

void ImgShow::init()
{
    scale=1.0f;

    roi.x=roi.y=0;
    roi.width=imgMat.cols;
    roi.height=imgMat.rows;
    setScale(scale);

    render();
}

void ImgShow::render()
{
    if(imgMat.empty())
        return;

    QImage qimg=MatToQImage(imgMat(roi));
    if(qimg.isNull())
        return;
    // 将 QImage 大小缩放到 QLabel 的大小
    // 使用线性插值缩小图像，使用最近邻插值放大图像
    if (qimg.width()>canve.width()||qimg.height()>canve.height()) {
        qimg = qimg.scaled(canve.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // 缩小
    } else {
        qimg = qimg.scaled(canve.size(), Qt::IgnoreAspectRatio, Qt::FastTransformation); // 放大
    }

    // 将处理后的图像设置到 QLabel
    canve.setPixmap(QPixmap::fromImage(qimg));
}

void ImgShow::resizeEvent(QResizeEvent *event)
{
    setScale(scale);
    render();
}

bool ImgShow::eventFilter(QObject *watched, QEvent *event)
{
    bool isRender=false;
    //获取鼠标事件
    QMouseEvent* mouseEvent=dynamic_cast<QMouseEvent*>(event);
    if(mouseEvent){
        //鼠标左键按住拖拽
        if((mouseEvent->buttons())&Qt::LeftButton){
            isRender=(isRender|drag(mouseEvent->pos()));
        }else{
            oldPoint=OLDPOINT;
        }
        //鼠标移动显示像素信息
        PixelInfo pixel=mousePixel(mouseEvent->pos());
        emit mousePixelInfo(pixel);
    }
    //获取鼠标滚轮事件
    QWheelEvent* wheelEvent=dynamic_cast<QWheelEvent*>(event);
    if(wheelEvent){
        //放大缩小
        // 获取滚轮的角度delta
        int delta = wheelEvent->angleDelta().y();
        if(delta>0){    //放大
            scale+=SCALE_DELTA+2*scale/10*SCALE_DELTA;
            isRender=true;
            setScale(scale);
        }else if(delta<0){  //缩小
            scale-=SCALE_DELTA+2*scale/10*SCALE_DELTA;
            isRender=true;
            setScale(scale);
        }
    }

    if(isRender)
        render();
    return false;
}

void ImgShow::setImgMat(const cv::Mat &mat)
{
    imgMat=mat;
    init();
}

void ImgShow::setScale(float s)
{
    scale=MAX(SCALE_MIN,s);
    emit scaleChanged(scale);
    //根据原始尺寸调整当前尺寸
    // int srcW=MIN(imgMat.cols,width());
    // int srcH=MIN(imgMat.rows,height());
    int srcW=width();
    int srcH=height();
    if(imgMat.cols>imgMat.rows){
        srcH=1.0*imgMat.rows/imgMat.cols*srcW;
    }else if(imgMat.rows>imgMat.cols){
        srcW=1.0*imgMat.cols/imgMat.rows*srcH;
    }else{
        srcW=srcH=MIN(width(),height());
    }
    //canve调整
    int cw=findMedian(1,srcW*scale,width());
    int ch=findMedian(1,srcH*scale,height());
    canve.resize(cw,ch);
    canve.move((width()-cw)/2,(height()-ch)/2);
    //roi调整
    int rw=findMedian(1,imgMat.cols/scale,imgMat.cols);
    int rh=findMedian(1,imgMat.rows/scale,imgMat.rows);
    //尽量调整roi宽高比等于canve宽高比
    if(rw>rh){
        rh=findMedian(1,1.0*rw*ch/cw,imgMat.rows);
    }else{
        rw=findMedian(1,1.0*rh*cw/ch,imgMat.cols);
    }
    int rdx=(rw-roi.width)/2;
    int rdy=(rh-roi.height)/2;
    roi.x=findMedian(0,roi.x-rdx,imgMat.cols-rw);
    roi.y=findMedian(0,roi.y-rdy,imgMat.rows-rh);
    roi.width=rw;
    roi.height=rh;
}

ImgShow::PixelInfo ImgShow::mousePixel(QPoint pos)
{
    PixelInfo pixel;
    if(imgMat.empty()||pos.x()<0||pos.x()>canve.width()||pos.y()<0||pos.y()>canve.height())
    {
        return pixel;
    }

    int x=(roi.width-1)*pos.x()*1.0/canve.width();
    int y=(roi.height-1)*pos.y()*1.0/canve.height();

    cv::Mat roiMat=imgMat(roi);

    uchar* ptr=roiMat.ptr<uchar>(y);
    ptr+=(imgMat.channels()*x);
    for(int i=0;i<imgMat.channels();++i)
        pixel.xyrgba[2+i]=ptr[i];
    if(imgMat.channels()==3){
        int a=pixel.r;
        pixel.r=pixel.b;
        pixel.b=a;
    }
    pixel.x=roi.x+x;
    pixel.y=roi.y+y;

    return pixel;
}

static int customCeil(float num) {
    if (num > 0) {
        return static_cast<int>(std::ceil(num)); // 对正数调用 std::ceil
    } else {
        return static_cast<int>(std::floor(num)); // 对负数调用 std::floor
    }
}

bool ImgShow::drag(const QPoint &newPoint)
{
    if(oldPoint==OLDPOINT){ //鼠标左键第一次按下
        oldPoint=newPoint;
        return false;
    }else{
        int dx=newPoint.x()-oldPoint.x();
        int dy=newPoint.y()-oldPoint.y();
        //移动roi区域
        int newx=roi.x+customCeil((dx/scale*2));
        int newy=roi.y+customCeil((dy/scale*2));
        roi.x=findMedian(0,newx,imgMat.cols-roi.width);
        roi.y=findMedian(0,newy,imgMat.rows-roi.height);

        oldPoint=newPoint;
        return true;
    }
}


ImageDisplayer::ImageDisplayer(const QSize &showSize, const cv::Mat &mat, QWidget *parent)
:QWidget(parent){
    imgShow=new ImgShow(showSize,mat,this);
    //设置只身大小
    setMinimumSize(QSize(IMGSHOW_MIN_W,IMGSHOW_MIN_H+NAV_HEIGHT));
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    int w=MAX(IMGSHOW_MIN_W,showSize.width());
    int h=MAX(IMGSHOW_MIN_H+NAV_HEIGHT,showSize.height()+NAV_HEIGHT);
    resize(w,h);

    setAcceptDrops(true);

    imgShow->move(0,NAV_HEIGHT);
    //初始化nav
    navInit();
}

ImageDisplayer::ImageDisplayer(QWidget *parent):ImageDisplayer(QSize(IMGSHOW_MIN_W,IMGSHOW_MIN_H),cv::Mat(),parent)
{

}

void ImageDisplayer::resizeEvent(QResizeEvent *event)
{
    QSize newSize=event->size();
    nav.resize(newSize.width(),NAV_HEIGHT);
    navPixInfo.resize(newSize.width()-NAV_HEIGHT,NAV_HEIGHT);
    imgShow->resize(QSize(newSize.width(),newSize.height()-NAV_HEIGHT));
}

void ImageDisplayer::setMat(const cv::Mat &mat)
{
    imgShow->setImgMat(mat.clone());
}

void ImageDisplayer::pixInfoUpdate()
{
    QString imgSizeStr(D_IMG_SIZE);
    imgSizeStr=imgSizeStr.arg(imgShow->imgMat.cols).arg(imgShow->imgMat.rows);

    QString pixelPointStr(D_PIX_POINT);
    pixelPointStr=pixelPointStr.arg(pixInfo.x).arg(pixInfo.y);

    QString pixelValueStr(" 像素值：(");
    for(int i=0;i<4;++i){
        int* v=&pixInfo.xyrgba[i+2];
        if(*v!=-1){
            pixelValueStr+=QString::number(*v);
        }
        if((i+2)<5&&v[1]!=-1){
            pixelValueStr+=",";
        }
    }
    pixelValueStr+=") ";

    QString scaleStr(D_SCALE_VALUE);
    scaleStr=scaleStr.arg(int(imgShow->scale*100));

    QString str=scaleStr+imgSizeStr+pixelPointStr+pixelValueStr;
    navPixInfo.setText(str);
}

void ImageDisplayer::navInit()
{
    nav.setParent(this);
    nav.resize(width(),NAV_HEIGHT);
    nav.setFixedHeight(NAV_HEIGHT);
    nav.move(0,0);
    //设置缩放图标
    QPushButton* sbtn=new QPushButton(&nav);
    sbtn->resize(NAV_HEIGHT,NAV_HEIGHT);
    sbtn->move(0,0);
    //图标设置
    QIcon icon(":/icons/icons/scale.png");
    sbtn->setIcon(icon);
    sbtn->setIconSize(sbtn->size());
    //设置点击事件
    sbtn->setCursor(Qt::PointingHandCursor);
    connect(sbtn,&QPushButton::clicked,this,[this](){
        imgShow->setScale(1.0);
        imgShow->render();
    });
    //设置信息显示
    navPixInfo.setParent(&nav);
    navPixInfo.move(NAV_HEIGHT,0);
    navPixInfo.resize(width()-NAV_HEIGHT,NAV_HEIGHT);

    pixInfoUpdate();

    //槽函数连接
    connect(imgShow,&ImgShow::mousePixelInfo,this,&ImageDisplayer::setPixInfo);
    connect(imgShow,&ImgShow::scaleChanged,this,&ImageDisplayer::pixInfoUpdate);
}

void ImageDisplayer::setPixInfo(ImgShow::PixelInfo info){
    pixInfo=info;
    pixInfoUpdate();
}
