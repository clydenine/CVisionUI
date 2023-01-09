#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <QObject>

class ImageView : public QGraphicsView
{
    Q_OBJECT
public:
    ImageView();
};

#endif // IMAGEVIEW_H