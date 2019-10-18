#include "formtranslation.h"
#include "ui_formtranslation.h"

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
