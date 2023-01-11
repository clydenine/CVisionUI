#include "imageview.h"
#include <QFileInfo>
#include <QDateTime>
#include <QFileDialog>
#include <QProcess>
#include <QScrollBar>
#include <QAction>
#include <QDebug>

#if QT_CONFIG(wheelevent)
void ImageView::wheelEvent(QWheelEvent* event)
{
    if (m_dispImage.isNull()) {
        return;
    }

    // 获取当前鼠标相对于view的位置;
    QPointF cursorPoint = event->position();
    // 获取当前鼠标相对于scene的位置;
    QPointF scenePos = this->mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));

    // 获取view的宽高;
    qreal viewWidth = this->viewport()->width();
    qreal viewHeight = this->viewport()->height();

    // 获取当前鼠标位置相当于view大小的横纵比例;
    qreal hScale = cursorPoint.x() / viewWidth;
    qreal vScale = cursorPoint.y() / viewHeight;

    // 当前放缩倍数;
    qreal scaleFactor = this->transform().m11();
    qDebug()<<scaleFactor;
    int wheelDeltaValue = event->angleDelta().y();
    // 向上滚动，放大;
    if (wheelDeltaValue > 0)
    {
        this->scale(1.2, 1.2);
    }
    // 向下滚动，缩小;
    else
    {
        this->scale(1.0 / 1.2, 1.0 / 1.2);
    }

    // 将scene坐标转换为放大缩小后的坐标;
    QPointF viewPoint = this->transform().map(scenePos);
    // 通过滚动条控制view放大缩小后的展示scene的位置;
    horizontalScrollBar()->setValue(int(viewPoint.x() - viewWidth * hScale));
    verticalScrollBar()->setValue(int(viewPoint.y() - viewHeight * vScale));

//    event->accept();
}
#endif

ImageView::ImageView(QWidget* parent) :QGraphicsView(parent)
, m_isPan(true)
, m_nWidth(0)
, m_nHeight(0)
, m_pScene(nullptr)
, m_pMenu(nullptr)
{
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

    m_pScene = new QGraphicsScene(this);
    this->setScene(m_pScene);

    setContextMenuPolicy(Qt::CustomContextMenu);
    m_pMenu = new QMenu(this);
    QAction* actionLoadImage = new QAction(tr("Load image"));
    QAction* actionSaveImage = new QAction(tr("Save Image"));
    m_pMenu->addAction(actionLoadImage);
    m_pMenu->addAction(actionSaveImage);
    connect(this, &ImageView::customContextMenuRequested, this, [=]() {
        m_pMenu->exec(QCursor::pos());
        });
    connect(actionLoadImage, &QAction::triggered, this, [=]() {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load image"), "c:\\", "Images (*.bmp *.png *.jpg *jpeg *.xpm )");
        if (!fileName.isEmpty())
        {
            QFileInfo fileInfo = QFileInfo(fileName);
            if (fileInfo.exists()) {
                loadImage(fileName);
                dispObj();
            }
        }
        });
    connect(actionSaveImage, &QAction::triggered, this, [=]() {
        saveImage();
        });
}

ImageView::~ImageView()
{

}

void ImageView::loadImage(QString fileName)
{
    QFileInfo info(fileName);
    if (info.exists()) {
        QString filter;
        filter = info.suffix();
        if ((filter != "jpg") && (filter != "jpeg") && (filter != "png") && (filter != "xpm") && (filter != "bmp")) {
            return;
        }
        m_dispMat = imread(fileName.toLocal8Bit().toStdString()/*,cv::IMREAD_GRAYSCALE*/);
        //imshow("debug", m_dispMat);
        if (!m_dispMat.empty()) {
            m_nWidth = m_dispMat.cols;
            m_nHeight = m_dispMat.rows;
        }
    }
}

void ImageView::loadImage(Mat image)
{
    if (!m_dispMat.empty()) {
        m_dispMat.release();
    }
    m_dispMat = image.clone();//deep copy
}

void ImageView::saveImage()
{
    if (!m_dispMat.empty()) {
        QString defalutSaveName = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss") + ".bmp";
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save image as ..."), defalutSaveName, "Images (*.bmp *.png *.jpg *jpeg *.xpm )");
        if (!fileName.isEmpty())
        {
            //            QFileInfo fileInfo=QFileInfo(fileName);
            //            QString fileSuffix = fileInfo.suffix();
            imwrite(fileName.toLocal8Bit().toStdString(), m_dispMat);
            QProcess process;
            fileName.replace("/", "\\"); // 只能识别 "\"
            QString cmd = QString("explorer.exe /select,\"%1\"").arg(fileName);
            process.startDetached(cmd);
        }
    }
}

void ImageView::dispObj()
{
    if (m_dispMat.empty()) {
        return;
    }
    m_dispImage = QPixmap::fromImage(cvMatToQImage(m_dispMat));

    m_pScene->clear();
    m_pScene->addPixmap(m_dispImage);
    m_pScene->update();
    this->resetTransform();
    this->setSceneRect(m_dispImage.rect());
    this->fitInView(QRect(0, 0, m_dispImage.width(), m_dispImage.height()), Qt::KeepAspectRatio);
}

//void ImageView::wheelEvent(QWheelEvent* event)
//{
//    if (m_dispImage.isNull()) {
//        return;
//    }
//    QPoint numDegree = event->angleDelta() / 8;
//    if (!numDegree.isNull()) {
//        QPoint numSteps = numDegree / 15;
//        zoom(numSteps);
//    }
//    event->accept();
//}

void ImageView::keyPressEvent(QKeyEvent* event)
{
    if (m_dispImage.isNull()) {
        return;
    }
    if (Qt::Key_O == event->key()) {
        this->resetTransform();
        this->setSceneRect(m_dispImage.rect());
        this->fitInView(QRect(0, 0, m_dispImage.width(), m_dispImage.height()), Qt::KeepAspectRatio);
    }
}

void ImageView::mousePressEvent(QMouseEvent* event)
{
    if (Qt::LeftButton == event->button()) {
        togglePan(true, event->pos());
        event->accept();
        return;
    }
    else if (Qt::MiddleButton == event->button()) {
        this->resetTransform();
        this->setSceneRect(m_dispImage.rect());
        this->fitInView(QRect(0, 0, m_dispImage.width(), m_dispImage.height()), Qt::KeepAspectRatio);
        event->accept();
        return;
    }
    event->ignore();
}

void ImageView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isPan) {
        pan(event->pos());
        event->accept();
        return;
    }
    event->ignore();
}

void ImageView::mouseReleaseEvent(QMouseEvent* event)
{
    if (Qt::LeftButton == event->button()) {
        togglePan(false);
        event->accept();
        return;
    }
    event->ignore();
}

void ImageView::zoom(QPoint factor)
{
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left(), FOV.top(), FOV.width(), FOV.height());
    float scaleX = static_cast<float>(m_dispImage.width()) / FOVImage.width();
    float scaleY = static_cast<float>(m_dispImage.height()) / FOVImage.height();
    float minScale = scaleX > scaleY ? scaleY : scaleX;
    float maxScale = scaleX > scaleY ? scaleX : scaleY;
    //if ((factor.y() > 0 && minScale > 100) || (factor.y() < 0 && maxScale < 1)) {
    //	return;
    //}
    if (factor.y() > 0)
        scale(1.2, 1.2);
    else
        scale(0.8, 0.8);
}

void ImageView::togglePan(bool pan, const QPoint& startPos)
{
    if (pan) {
        if (m_isPan) {
            return;
        }
        m_isPan = true;
        m_prevPan = startPos;
        setCursor(Qt::ClosedHandCursor);
    }
    else {
        if (!m_isPan) {
            return;
        }
        m_isPan = false;
        m_prevPan = QPoint(0, 0);
        setCursor(Qt::ArrowCursor);
    }
}

void ImageView::pan(const QPoint& panTo)
{
    auto hBar = horizontalScrollBar();
    auto vBar = verticalScrollBar();
    auto delta = panTo - m_prevPan;
    m_prevPan = panTo;
    hBar->setValue(hBar->value() - delta.x());
    vBar->setValue(vBar->value() - delta.y());
}

QImage ImageView::cvMatToQImage(Mat& srcMat)
{
    if (srcMat.channels() == 1) {//grayscale image
        return QImage((uchar*)srcMat.data,
            srcMat.cols,
            srcMat.rows,
            static_cast<int>(srcMat.step),
            QImage::Format_Grayscale8).copy();
    }
    if (srcMat.channels() == 3) {//color image
        Mat dstMat;
        cvtColor(srcMat, dstMat, cv::COLOR_BGR2RGB);
        return QImage((uchar*)dstMat.data,
            dstMat.cols,
            dstMat.rows,
            static_cast<int>(dstMat.step),
            QImage::Format_RGB888).copy();
    }
    return QImage();
}

Mat ImageView::qImageToCvMat(QImage& srcImg)
{
    switch (srcImg.format())
    {
    case QImage::Format_RGB32:
    case QImage::Format_RGB888:
    {
        QImage swapped = srcImg;
        if (srcImg.format() == QImage::Format_RGB32)
        {
            swapped = swapped.convertToFormat(QImage::Format_RGB888);
        }
        Mat matImg = Mat(swapped.height(), swapped.width(),
            CV_8UC3,
            const_cast<uchar*>(swapped.bits()),
            static_cast<size_t>(swapped.bytesPerLine())
        );
        cvtColor(matImg, matImg, cv::COLOR_RGB2BGR);
        return matImg.clone();
    }
    case QImage::Format_Indexed8:
    {
        Mat  mat(srcImg.height(), srcImg.width(),
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
