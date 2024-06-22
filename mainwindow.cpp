#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
//    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
//    setParent(0); // Create TopLevel-Widget
//    setAttribute(Qt::WA_NoSystemBackground, true);
//    setAttribute(Qt::WA_TranslucentBackground, true);
//    setAttribute(Qt::WA_PaintOnScreen); // not needed in Qt 5.2 and up
}

MainWindow::~MainWindow()
{
}

void MainWindow::handleDataReceived(const QByteArray &data)
{

    }

