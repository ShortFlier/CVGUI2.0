#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QList<OperatorInfo> oinfos;
    oinfos.append(OperatorInfo("t1","o1"));
    oinfos.append(OperatorInfo("t1","o2"));
    oinfos.append(OperatorInfo("t2","o3"));

    ui->toolBox->listOperator(oinfos);
}

MainWindow::~MainWindow()
{
    delete ui;
}
