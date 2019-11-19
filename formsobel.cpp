#include "formsobel.h"
#include "ui_formsobel.h"
#include <QTextStream>
#include <QDebug>

formsobel::formsobel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formsobel)
{
    ui->setupUi(this);
}

formsobel::~formsobel()
{
    delete ui;
}

int formsobel::getThreshold() {
    qInfo("form sobel");
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(2)->widget();
    qInfo("form sobel");
    QString text = lineEdit->text();
    qInfo("form sobel");
    QTextStream intTextStream(&text);
    qInfo("form sobel");
    int c;
    qInfo("form sobel");
    intTextStream >> c;

    return c;
}

int formsobel::getC() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(3)->widget();
    QString text = lineEdit->text();
    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}
