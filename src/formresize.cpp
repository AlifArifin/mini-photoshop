#include "formresize.h"
#include "ui_formresize.h"
#include <QTextStream>
#include <QDebug>

FormResize::FormResize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormResize)
{
    ui->setupUi(this);
}

FormResize::~FormResize()
{
    delete ui;
}

int FormResize::getHeight() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(2)->widget();
    QString text = lineEdit->text();
    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}

int FormResize::getWidth() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(3)->widget();
    QString text = lineEdit->text();
    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}
