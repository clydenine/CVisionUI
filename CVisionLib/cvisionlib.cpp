#include "cvisionlib.h"

CVisionLib::CVisionLib()
{
}

ImageView* CVisionLib::createImageView(QWidget* parent)
{
    return new ImageView(parent);
}
