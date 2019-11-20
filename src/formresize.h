#ifndef FORMRESIZE_H
#define FORMRESIZE_H

#include <QDialog>

namespace Ui {
class FormResize;
}

class FormResize : public QDialog
{
    Q_OBJECT

public:
    explicit FormResize(QWidget *parent = nullptr);
    ~FormResize();
    int getHeight();
    int getWidth();

private:
    Ui::FormResize *ui;
};

#endif // FORMRESIZE_H
