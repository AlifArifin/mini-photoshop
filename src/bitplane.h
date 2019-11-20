#ifndef BITPLANE_H
#define BITPLANE_H

#include <QDialog>

namespace Ui {
class BitPlane;
}

class BitPlane : public QDialog
{
    Q_OBJECT

public:
    explicit BitPlane(QWidget *parent = nullptr);
    ~BitPlane();
    void setBit7(QImage image);
    void setBit6(QImage image);
    void setBit5(QImage image);
    void setBit4(QImage image);
    void setBit3(QImage image);
    void setBit2(QImage image);
    void setBit1(QImage image);
    void setBit0(QImage image);

private:
    Ui::BitPlane *ui;
};

#endif // BITPLANE_H
