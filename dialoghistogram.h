#ifndef DIALOGHISTOGRAM_H
#define DIALOGHISTOGRAM_H

#include <QDialog>
#include <QGraphicsView>
#include <QChart>
#include <QChartView>

namespace Ui {
class DialogHistogram;
}

class DialogHistogram : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHistogram(QWidget *parent = nullptr);
    ~DialogHistogram();
    void setHistogram(QGraphicsView * view);
    QGraphicsView * getHistogram();
    void setChart(QtCharts::QChart * chart);

private:
    Ui::DialogHistogram *ui;
};

#endif // DIALOGHISTOGRAM_H
