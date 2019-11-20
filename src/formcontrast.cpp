#include "formcontrast.h"
#include "ui_formcontrast.h"
#include <QTextStream>
#include <QDebug>

using namespace std;

FormContrast::FormContrast(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormContrast)
{
    ui->setupUi(this);
}

FormContrast::~FormContrast()
{
    delete ui;
}

int FormContrast::getA() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(7)->widget();
    QString text = lineEdit->text();
    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}

int FormContrast::getB() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(8)->widget();
    QString text = lineEdit->text();
    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}

int FormContrast::getYa() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(9)->widget();
    QString text = lineEdit->text();
    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}

int FormContrast::getYb() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(10)->widget();
    QString text = lineEdit->text();
    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}

float FormContrast::getAlpha() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(11)->widget();
    QString text = lineEdit->text();

    if (text.isEmpty()) {
        return 1;
    }

    QTextStream floatTextStream(&text);
    float c;
    floatTextStream >> c;

    return c;
}

float FormContrast::getBeta() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(12)->widget();
    QString text = lineEdit->text();

    if (text.isEmpty()) {
        return 1;
    }

    QTextStream floatTextStream(&text);
    float c;
    floatTextStream >> c;

    return c;
}

float FormContrast::getGamma() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(13)->widget();
    QString text = lineEdit->text();

    if (text.isEmpty()) {
        return 1;
    }

    QTextStream floatTextStream(&text);
    float c;
    floatTextStream >> c;

    return c;
}
