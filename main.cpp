#include "mainwindow.h"

#include <QApplication>

#include "log.h"

int main(int argc, char *argv[])
{
    //初始化日志
    initLogger();


    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int res=a.exec();

    log_info("程序正常关闭！");

    return res;
}
