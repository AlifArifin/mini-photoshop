#ifndef FORMSOBEL_H
#define FORMSOBEL_H

#include <QDialog>

namespace Ui {
class formsobel;
}

class formsobel : public QDialog
{
    Q_OBJECT

public:
    explicit formsobel(QWidget *parent = nullptr);
    ~formsobel();
    int getThreshold();
    int getC();

private:
    Ui::formsobel *ui;
};

#endif // FORMSOBEL_H
