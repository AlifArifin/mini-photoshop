#include "formgrayslicing.h"
#include "ui_formgrayslicing.h"
#include <QTextStream>

FormGraySlicing::FormGraySlicing(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormGraySlicing)
{
    ui->setupUi(this);
}

FormGraySlicing::~FormGraySlicing()
{
    delete ui;
}

int FormGraySlicing::getA() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(3)->widget();
    QString text = lineEdit->text();

    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}

int FormGraySlicing::getB() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(4)->widget();
    QString text = lineEdit->text();

    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}

int FormGraySlicing::getMax() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(5)->widget();
    QString text = lineEdit->text();

    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}
