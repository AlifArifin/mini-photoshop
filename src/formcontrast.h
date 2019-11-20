#ifndef FORMCONTRAST_H
#define FORMCONTRAST_H

#include <QDialog>

namespace Ui {
class FormContrast;
}

class FormContrast : public QDialog
{
    Q_OBJECT

public:
    explicit FormContrast(QWidget *parent = nullptr);
    ~FormContrast();
    int getA();
    int getB();
    int getYa();
    int getYb();
    float getAlpha();
    float getBeta();
    float getGamma();

private:
    Ui::FormContrast *ui;
};

#endif // FORMCONTRAST_H
