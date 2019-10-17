#include "bitplane.h"
#include "ui_bitplane.h"
#include <QLabel>
#include <QPixmap>

BitPlane::BitPlane(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BitPlane)
{
    ui->setupUi(this);
}

BitPlane::~BitPlane()
{
    delete ui;
}

void BitPlane::setBit7(QImage image) {
    QLabel * label = ui->bit7;
    label->setPixmap(QPixmap::fromImage(image));
    label->show();
}

void BitPlane::setBit6(QImage image) {
    QLabel * label = ui->bit6;
    label->setPixmap(QPixmap::fromImage(image));
    label->show();
}

void BitPlane::setBit5(QImage image) {
    QLabel * label = ui->bit5;
    label->setPixmap(QPixmap::fromImage(image));
    label->show();
}

void BitPlane::setBit4(QImage image) {
    QLabel * label = ui->bit4;
    label->setPixmap(QPixmap::fromImage(image));
    label->show();
}

void BitPlane::setBit3(QImage image) {
    QLabel * label = ui->bit3;
    label->setPixmap(QPixmap::fromImage(image));
    label->show();
}

void BitPlane::setBit2(QImage image) {
    QLabel * label = ui->bit2;
    label->setPixmap(QPixmap::fromImage(image));
    label->show();
}

void BitPlane::setBit1(QImage image) {
    qInfo("1");
    QLabel * label = ui->bit1;
    qInfo("2");
    label->setPixmap(QPixmap::fromImage(image));
    qInfo("3");
    label->show();
    qInfo("4");
}

void BitPlane::setBit0(QImage image) {
    qInfo("1");
    QLabel * label = ui->bit0;
    qInfo("2");
    label->setPixmap(QPixmap::fromImage(image));
    qInfo("3");
    label->show();
    qInfo("4");
}
