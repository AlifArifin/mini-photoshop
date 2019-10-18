#include "formtranslation.h"
#include "ui_formtranslation.h"
#include <QLineEdit>
#include <QTextStream>
#include <QFormLayout>

using namespace std;

FormTranslation::FormTranslation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormTranslation)
{
    ui->setupUi(this);
}

FormTranslation::~FormTranslation()
{
    delete ui;
}

int FormTranslation::getX() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(2)->widget();
    QString text = lineEdit->text();
    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}

int FormTranslation::getY() {
    QLineEdit *lineEdit = (QLineEdit*) ui->formLayout->itemAt(3)->widget();
    QString text = lineEdit->text();
    QTextStream intTextStream(&text);
    int c;
    intTextStream >> c;

    return c;
}
