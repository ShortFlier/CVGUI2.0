#ifndef IMAGEDISPLAYER_H
#define IMAGEDISPLAYER_H

#include <QLabel>
#include <QResizeEvent>
#include <QWidget>
#include <QTableWidget>

#include <opencv2/opencv.hpp>


#define IMGSHOW_MIN_W 200
#define IMGSHOW_MIN_H 200

#define OLDPOINT QPoint(-1,-1)

#define SCALE_MIN 0.1
#define SCALE_DELTA 0.1

class ImgShow:public QWidget{
    Q_OBJECT
public:

    union PixelInfo{
        int xyrgba[6];
        struct{
            int x,y,r,g,b,a;
        };
        PixelInfo(int x=-1, int y=-1, int r=-1, int g=-1, int b=-1, int a=-1):x(x), y(y), r(r), g(g), b(b), a(a){}
    };

    explicit ImgShow(const QSize& size=QSize(IMGSHOW_MIN_W,IMGSHOW_MIN_H),const cv::Mat& mat=cv::Mat(),QWidget *parent = nullptr);

    cv::Mat imgMat;
    cv::Rect roi;
    float scale;

    QLabel canve;

    //设置图片后初始化
    void init();

    //渲染显示图片
    void render();

    //大小调整
    void resizeEvent(QResizeEvent *event) override;

    //鼠标事件监听
    bool eventFilter(QObject *watched, QEvent *event) override;

    void setImgMat(const cv::Mat& mat=cv::Mat());

    void setScale(float s);

    //鼠标指向像素信息
    PixelInfo mousePixel(QPoint pos);

private:
    QPoint oldPoint;

    //鼠标拖拽
    bool drag(const QPoint& newPoint);


signals:
    //鼠标指向像素信息抛出
    void mousePixelInfo(ImgShow::PixelInfo pixInfo);
    //scale改变
    void scaleChanged(float scale);
};


#define D_IMG_SIZE " 图片大小：%1 X %2 "
#define D_PIX_POINT " 像素位置：(%1,%2) "
#define D_SCALE_VALUE " 缩放：%1% "

#define NAV_HEIGHT 40


class ImageDisplayer : public QWidget
{
    Q_OBJECT
public:
    explicit ImageDisplayer(const QSize& showSize=QSize(IMGSHOW_MIN_W,IMGSHOW_MIN_H),const cv::Mat& mat=cv::Mat(),QWidget *parent = nullptr);

    ImageDisplayer(QWidget *parent);

    ImgShow* imgShow;

    //大小调整
    void resizeEvent(QResizeEvent *event) override;

    void setMat(const cv::Mat& mat);

public slots:
    //像素信息更新
    void pixInfoUpdate();

    //获取像素信息
    void setPixInfo(ImgShow::PixelInfo info);

private:
    QWidget nav;

    QLabel navPixInfo;

    ImgShow::PixelInfo pixInfo;

    void navInit();
};

#endif // IMAGEDISPLAYER_H
