#include "tabimage.h"
#include "image_type.h"

TabImage::TabImage(QWidget *parent) : QWidget(parent)
{
}

void TabImage::setImageType(ImageType i) {
    this->imageType = i;
}

ImageType TabImage::getImageType() {
    return this->imageType;
}
