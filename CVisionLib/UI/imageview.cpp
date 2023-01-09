#include "imageview.h"
#include <QFileInfo>
#include <QDateTime>
#include <QFileDialog>
#include <QProcess>

ImageView::ImageView(QWidget *parent):QGraphicsView(parent)
  ,m_scene(nullptr)

{
    m_scene=new QGraphicsScene(this);
    this->setScene(m_scene);

    setFrameStyle(Sunken | StyledPanel);
    setRenderHint(QPainter::Antialiasing, true);
    setDragMode(QGraphicsView::RubberBandDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setMouseTracking(true);//跟踪鼠标位置
    setFocusPolicy(Qt::NoFocus);
    viewport()->setStyleSheet("border:none;background-color:rgb(255,255,255);");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

ImageView::~ImageView()
{

}

void ImageView::loadImage(QString fileName)
{
    QFileInfo info(fileName);
    if(info.exists()){
        QString filter;
        filter=info.suffix();
        if( (filter != "jpg") && (filter != "jpeg") &&(filter != "png") &&(filter != "xpm")&&(filter != "bmp")){
            return;
        }
        m_dispMat = imread(fileName.toLocal8Bit().toStdString(),0);
        if(!m_dispMat.empty()){
            m_nWidth= m_dispMat.cols;
            m_nHeight=m_dispMat.rows;
        }
    }
}

void ImageView::loadImage(Mat image)
{
    if(!m_dispMat.empty()){
        m_dispMat.release();
    }
    m_dispMat=image.clone();//deep copy
}

void ImageView::saveImage()
{
    if(!m_dispMat.empty()){
        QString defalutSaveName=QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".bmp";
        QString fileName = QFileDialog::getSaveFileName (this,tr("Save image as ..."),defalutSaveName,"Images (*.bmp *.png *.jpg *jpeg *.xpm )");
        if(!fileName.isEmpty ())
        {
//            QFileInfo fileInfo=QFileInfo(fileName);
//            QString fileSuffix = fileInfo.suffix();
            imwrite(fileName.toLocal8Bit().toStdString(),m_dispMat);
            QProcess process;
            fileName.replace("/", "\\"); // 只能识别 "\"
            QString cmd = QString("explorer.exe /select,\"%1\"").arg(fileName);
            process.startDetached(cmd);
        }
    }
}

void ImageView::dispObj()
{
    if(m_dispMat.empty()){
        return;
    }
    Mat imgTemp;
    cvtColor(m_dispMat,imgTemp,COLOR_BGR2RGB);

}

QImage ImageView::cvMatToQImage(Mat &srcMat)
{
    if(srcMat.channels()==1){//grayscale image
        return QImage((uchar*)srcMat.data,
                      srcMat.cols,
                      srcMat.rows,
                      static_cast<int>(srcMat.step),
                      QImage::Format_Grayscale8).copy();
    }
    if(srcMat.channels()==3){//color image
        Mat dstMat;
        cvtColor(srcMat,dstMat,cv::COLOR_BGR2RGB);
        return QImage((uchar*)dstMat.data,
                      dstMat.cols,
                      dstMat.rows,
                      static_cast<int>(dstMat.step),
                      QImage::Format_Grayscale8).copy();
    }
    return QImage();
}

Mat ImageView::qImageToCvMat(QImage &srcImg)
{
    switch (srcImg.format())
    {
    case QImage::Format_RGB32:
    case QImage::Format_RGB888:
    {
        QImage swapped = srcImg;
        if ( srcImg.format() == QImage::Format_RGB32 )
        {
            swapped = swapped.convertToFormat( QImage::Format_RGB888 );
        }
        Mat matImg = Mat(swapped.height(), swapped.width(),
                         CV_8UC3,
                         const_cast<uchar*>(swapped.bits()),
                         static_cast<size_t>(swapped.bytesPerLine())
                         );
        cvtColor(matImg,matImg,cv::COLOR_RGB2BGR);
        return matImg.clone();
    }
    case QImage::Format_Indexed8:
    {
        Mat  mat( srcImg.height(), srcImg.width(),
                  CV_8UC1,
                  const_cast<uchar*>(srcImg.bits()),
                  static_cast<size_t>(srcImg.bytesPerLine())
                  );
        return mat.clone();
    }
    default:
        break;
    }
    return Mat();
}
