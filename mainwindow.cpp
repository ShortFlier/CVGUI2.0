#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0,2);
    ui->splitter->setStretchFactor(1,6);
    ui->splitter->setStretchFactor(2,3);

    ui->opView->setStretchFactor(0,1);
    ui->opView->setStretchFactor(1,1);

    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    ui->opBox->init();
    QList<OperatorInfo> oinfos;
    oinfos.append(OperatorInfo("t1","o1"));
    oinfos.append(OperatorInfo("t1","边2"));
    oinfos.append(OperatorInfo("t2","o3"));
    ui->opBox->listOperator(oinfos);
}

MainWindow& MainWindow::instance()
{
    static MainWindow mainWindow;
    return mainWindow;
}

MainWindow *MainWindow::instancePtr()
{
    return &instance();
}
