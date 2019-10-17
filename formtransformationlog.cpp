#include "formtransformationlog.h"
#include "ui_formtransformationlog.h"
#include <QTextStream>

FormTransformationLog::FormTransformationLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormTransformationLog)
{
    ui->setupUi(this);
}

FormTransformationLog::~FormTransformationLog()
{
    delete ui;
}

float FormTransformationLog::getC() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(2)->widget();
    QString text = lineEdit->text();

    QTextStream floatTextStream(&text);
    float c;
    floatTextStream >> c;

    return c;
}

float FormTransformationLog::getGamma() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(3)->widget();
    QString text = lineEdit->text();

    if (text.isEmpty()) {
        return 1;
    }

    QTextStream floatTextStream(&text);
    float c;
    floatTextStream >> c;

    return c;
}
