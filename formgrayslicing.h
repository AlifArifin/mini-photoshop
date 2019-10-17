#ifndef FORMGRAYSLICING_H
#define FORMGRAYSLICING_H

#include <QDialog>

namespace Ui {
class FormGraySlicing;
}

class FormGraySlicing : public QDialog
{
    Q_OBJECT

public:
    explicit FormGraySlicing(QWidget *parent = nullptr);
    ~FormGraySlicing();
    int getA();
    int getB();
    int getMax();

private:
    Ui::FormGraySlicing *ui;
};

#endif // FORMGRAYSLICING_H
