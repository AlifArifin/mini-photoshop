#include "imagepreview.h"
#include "ui_imagepreview.h"
#include <QImage>
#include <QLabel>

ImagePreview::ImagePreview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImagePreview)
{
    ui->setupUi(this);
}

ImagePreview::~ImagePreview()
{
    delete ui;
}

void ImagePreview::setImage(QImage image) {
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->label->show();
}
