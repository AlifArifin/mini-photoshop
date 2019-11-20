#ifndef FORMTRANSLATION_H
#define FORMTRANSLATION_H

#include <QDialog>

namespace Ui {
class FormTranslation;
}

class FormTranslation : public QDialog
{
    Q_OBJECT

public:
    explicit FormTranslation(QWidget *parent = nullptr);
    ~FormTranslation();
    int getX();
    int getY();

private:
    Ui::FormTranslation *ui;
};

#endif // FORMTRANSLATION_H
