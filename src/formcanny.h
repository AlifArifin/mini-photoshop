#ifndef FORMCANNY_H
#define FORMCANNY_H

#include <QDialog>

namespace Ui {
class FormCanny;
}

class FormCanny : public QDialog
{
    Q_OBJECT

public:
    explicit FormCanny(QWidget *parent = nullptr);
    ~FormCanny();
    int getC();
    float getLowThreshold();
    float getHighThreshold();
    float getSigma();

private:
    Ui::FormCanny *ui;
};

#endif // FORMCANNY_H
