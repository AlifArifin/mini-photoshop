#include "dialoghistogram.h"
#include "ui_dialoghistogram.h"
#include <QChartView>
#include <QChart>

using namespace QtCharts;

DialogHistogram::DialogHistogram(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHistogram)
{
    ui->setupUi(this);
}

DialogHistogram::~DialogHistogram()
{
    delete ui;
}

void DialogHistogram::setHistogram(QGraphicsView * view) {
    ui->graphicsView = view;
}

QGraphicsView * DialogHistogram::getHistogram() {
    return ui->graphicsView;
}

void DialogHistogram::setChart(QChart * chart) {
    ui->graphicsView = new QChartView(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}
