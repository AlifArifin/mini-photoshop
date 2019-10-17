#ifndef TABPAGE_H
#define TABPAGE_H

#include <QWidget>
#include "image_type.h"

namespace Ui {
class TabPage;
}

class TabPage : public QWidget
{
    Q_OBJECT

public:
    explicit TabPage(QWidget *parent = nullptr);
    ~TabPage();
    ImageType getImageType();
    void setImageType(ImageType i);

private:
    Ui::TabPage *ui;
    ImageType imageType;
};

#endif // TABPAGE_H
