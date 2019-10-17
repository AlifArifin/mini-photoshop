#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

#include <QDialog>
#include <QImage>

namespace Ui {
class ImagePreview;
}

class ImagePreview : public QDialog
{
    Q_OBJECT

public:
    explicit ImagePreview(QWidget *parent = nullptr);
    ~ImagePreview();
    void setImage(QImage image);

private:
    Ui::ImagePreview *ui;
};

#endif // IMAGEPREVIEW_H
