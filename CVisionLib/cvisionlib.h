#ifndef CVISIONLIB_H
#define CVISIONLIB_H

#include "CVisionLib_global.h"
#include "UI/imageview.h"

class CVISIONLIB_EXPORT CVisionLib
{
public:
    CVisionLib();

    static ImageView* createImageView(QWidget* parent = nullptr);
};

#endif // CVISIONLIB_H
