#include "mainwindow.h"
#include <QGuiApplication>
#include <QScreen>
#include "CVisionLib.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QScreen* screen=QGuiApplication::primaryScreen();
    QRect mainRect=screen->availableGeometry();
    resize(QSize(mainRect.width()*0.68,mainRect.height()*0.68));
    setWindowTitle("CVisionUI");

    ImageView* pImageView = CVisionLib::createImageView(this);
    setCentralWidget(pImageView);
}

MainWindow::~MainWindow()
{
}

