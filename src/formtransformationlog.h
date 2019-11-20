#ifndef FORMTRANSFORMATIONLOG_H
#define FORMTRANSFORMATIONLOG_H

#include <QDialog>

namespace Ui {
class FormTransformationLog;
}

class FormTransformationLog : public QDialog
{
    Q_OBJECT

public:
    explicit FormTransformationLog(QWidget *parent = nullptr);
    ~FormTransformationLog();
    float getC();
    float getGamma();

private:
    Ui::FormTransformationLog *ui;
};

#endif // FORMTRANSFORMATIONLOG_H
