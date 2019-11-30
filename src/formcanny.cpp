#include "formcanny.h"
#include "ui_formcanny.h"
#include <QTextStream>

FormCanny::FormCanny(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormCanny)
{
    ui->setupUi(this);
}

FormCanny::~FormCanny()
{
    delete ui;
}

int FormCanny::getC() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(4)->widget();
    QString text = lineEdit->text();
    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}

float FormCanny::getLowThreshold() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(5)->widget();
    QString text = lineEdit->text();

    if (text.isEmpty()) {
        return 1;
    }

    QTextStream floatTextStream(&text);
    float c;
    floatTextStream >> c;

    return c;
}

float FormCanny::getHighThreshold() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(6)->widget();
    QString text = lineEdit->text();

    if (text.isEmpty()) {
        return 1;
    }

    QTextStream floatTextStream(&text);
    float c;
    floatTextStream >> c;

    return c;
}

float FormCanny::getSigma() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(7)->widget();
    QString text = lineEdit->text();

    if (text.isEmpty()) {
        return 1;
    }

    QTextStream floatTextStream(&text);
    float c;
    floatTextStream >> c;

    return c;
}
