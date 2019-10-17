#ifndef TABIMAGE_H
#define TABIMAGE_H

#include <QWidget>
#include <QLabel>
#include "image_type.h"

class TabImage : public QWidget
{
    Q_OBJECT

private:
    ImageType imageType;

public:
    explicit TabImage(QWidget *parent = nullptr);
    TabImage(QString filename, QWidget *parent = nullptr);

signals:

public slots:
    void setImageType(ImageType i);
    ImageType getImageType();
};

#endif // TABIMAGE_H
