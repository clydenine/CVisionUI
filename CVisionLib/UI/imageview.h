#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QImage>
#include <QWheelEvent>
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

using namespace  cv;

class ImageView : public QGraphicsView
{
    Q_OBJECT
public:
    ImageView(QWidget *parent=nullptr);
    ~ImageView();

    void loadImage(QString fileName);
    void loadImage(Mat image);
    void saveImage();
    void dispObj();

protected:
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    void zoom(QPoint factor);
    void togglePan(bool pan,const QPoint &startPos=QPoint());
    void pan(const QPoint &panTo);

private:
    int m_nWidth;
    int m_nHeight;
    QGraphicsScene *m_scene;
    Mat m_dispMat;
    QPixmap m_dispImage;

    QImage cvMatToQImage(Mat &srcMat);
    Mat qImageToCvMat(QImage &srcImg);

};

#endif // IMAGEVIEW_H
