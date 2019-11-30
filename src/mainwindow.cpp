#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tabimage.h"
#include "binary.h"
#include "grayscale.h"
#include "monochrome.h"
#include "image_format.h"
#include "image_type.h"
#include "truecolor.h"
#include "imagepreview.h"
#include "tabpage.h"
#include "operation.h"
#include "transformation.h"
#include "geometry.h"
#include "histogram.h"
#include "formcontrast.h"
#include "formtransformationlog.h"
#include "formgrayslicing.h"
#include "formresize.h"
#include "bitplane.h"
#include "dialoghistogram.h"
#include "convolution.h"
#include "padding.h"
#include "rgba.h"
#include "ppm_state.h"
#include "formtranslation.h"
#include "formsobel.h"
#include "formcanny.h"
#include <QFileDialog>
#include <QTabBar>
#include <QTabWidget>
#include <QWidget>
#include <iostream>
#include <fstream>
#include <QDebug>
#include <QTextStream>
#include <QImage>
#include <QInputDialog>
#include <QLineEdit>
#include <QTextStream>
#include <QMessageBox>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QChartView>
#include <QPainter>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>

using namespace std;
using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    window = new QMainWindow(this);
    qInfo("create");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,
                "Open file",
                "D://",
                "Images (*.pbm *.pgm *.ppm *.bmp *.raw_image)"
    );

    QRegExp rx("[/]");// match /
    QStringList splitName = fileName.split(rx, QString::SkipEmptyParts);

    qInfo("lala");

    QRegExp rx2("[\.]");// match .
    QStringList splitName2 = fileName.split(rx2, QString::SkipEmptyParts);

    string amount = to_string(splitName2.size());
    QString fileFormat = splitName2[splitName2.size() - 1];

    string sFileFormat = fileFormat.toLocal8Bit().constData();
    string sFileName = fileName.toLocal8Bit().constData();

    // check image format and type
    ImageType imageType = Image::checkFile(sFileFormat);
    ImageFormat imageFormat = Image::checkFormat(sFileFormat);

    int width = 0, height = 0;

    ifstream file;
    file.open(sFileName.c_str());

    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    TabPage * tabPage = new TabPage(tabWidget);
    QLabel * label = tabPage->findChild<QLabel*>("label");
    label->setAlignment(Qt::AlignCenter);

    if (imageType == ImageType::BINARY) {
        Binary binary(&file, imageFormat, imageType);
        height = binary.getResolution().height;
        width = binary.getResolution().width;

        QImage image(width, height, QImage::Format_Grayscale8);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (binary.getIndividualPixel(i, j) == 0) {
                    image.setPixel(j, i, qRgb(0, 0, 0));
                } else {
                    image.setPixel(j, i, qRgb(255, 255, 255));
                }
            }
        }

        label->setPixmap(QPixmap::fromImage(image));
        binaries.push_back(new Binary(binary));
        tabPage->setImageType(ImageType::BINARY);
    } else if (imageType == ImageType::GRAYSCALE) {
        qInfo("lala");
        Grayscale grayscale(&file, imageFormat, imageType);
        height = grayscale.getResolution().height;
        width = grayscale.getResolution().width;

        QImage image(width, height, QImage::Format_Grayscale8);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int value = grayscale.getIndividualPixel(i, j);
                if (grayscale.getLevel() != 255) {
                    value = (int) ((float) grayscale.getIndividualPixel(i, j) / grayscale.getLevel() * 255);
                }

                image.setPixel(j, i, qRgb(value, value, value));
            }
        }
        label->setPixmap(QPixmap::fromImage(image));
        grayscales.push_back(new Grayscale(grayscale));
        tabPage->setImageType(ImageType::GRAYSCALE);
    } else if (imageType == ImageType::TRUECOLOR) {
        Truecolor * truecolor;
        if (imageFormat == ImageFormat::BMP) {
            truecolor = new Truecolor(sFileName, imageFormat, imageType);
        } else {
            truecolor = new Truecolor(&file, imageFormat, imageType);
        }
        qInfo("after ctor");
        height = truecolor->getResolution().height;
        width = truecolor->getResolution().width;

        QImage image(width, height, QImage::Format_RGB32);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                RGBA value = truecolor->getIndividualPixel(i, j);
                if (truecolor->getLevel() != 255) {
                    value.r = (int) ((float) value.r / truecolor->getLevel() * 255);
                    value.g = (int) ((float) value.g / truecolor->getLevel() * 255);
                    value.b = (int) ((float) value.b / truecolor->getLevel() * 255);
                }

                image.setPixel(j, i, qRgb(value.r, value.g, value.b));
            }
        }
        qInfo("after ctor");
        label->setPixmap(QPixmap::fromImage(image));
        qInfo("after ctor");
        truecolors.push_back(new Truecolor(*truecolor));
        qInfo("after ctor");
        tabPage->setImageType(ImageType::TRUECOLOR);
        qInfo("after ctor");
    }

    label->show();
    tabWidget->addTab(tabPage, splitName[splitName.size() - 1]);
    qInfo("done");
}

void MainWindow::on_actionNegative_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Monochrome prev = b->negative();

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                binaries.at(imageIdx) = new Binary(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Monochrome prev = g->negative();
            QImage image = this->fromMonochrome(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.at(imageIdx) = new Grayscale(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        } case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Truecolor prev = tr->negative();
            QImage image = this->fromTruecolor(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                truecolors.at(imageIdx) = new Truecolor(prev);
                QImage newImage = fromTruecolor(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

int MainWindow::getVectorIdx(int current, ImageType imageType) {
    int index = 0;
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");


    for (int i = 0; i < current; i++) {
        TabPage * t = (TabPage *) tabWidget->widget(i);
        if (imageType == t->getImageType()) {
            index++;
        }
    }

    return index;
}

QImage MainWindow::fromBinary(Binary binary) {
    int height = binary.getResolution().height;
    int width = binary.getResolution().width;

    QImage image(width, height, QImage::Format_Grayscale8);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (binary.getIndividualPixel(i, j) == 0) {
                image.setPixel(j, i, qRgb(0, 0, 0));
            } else {
                image.setPixel(j, i, qRgb(255, 255, 255));
            }
        }
    }

    return image;
}
QImage MainWindow::fromGrayscale(Grayscale grayscale) {
    int height = grayscale.getResolution().height;
    int width = grayscale.getResolution().width;

    QImage image(width, height, QImage::Format_Grayscale8);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int value = grayscale.getIndividualPixel(i, j);
            if (grayscale.getLevel() != 255) {
                value = (int) ((float) grayscale.getIndividualPixel(i, j) / grayscale.getLevel() * 255);
            }

            image.setPixel(j, i, qRgb(value, value, value));
        }
    }

    return image;
}

QImage MainWindow::fromTruecolor(Truecolor truecolor) {
    int height = truecolor.getResolution().height;
    int width = truecolor.getResolution().width;

    QImage image(width, height, QImage::Format_RGB32);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            RGBA pixel = truecolor.getIndividualPixel(i, j);
            if (truecolor.getLevel() != 255) {
                image.setPixel(j, i, qRgba(
                                   (int) ((float) pixel.r / truecolor.getLevel() * 255),
                                   (int) ((float) pixel.g / truecolor.getLevel() * 255),
                                   (int) ((float) pixel.b / truecolor.getLevel() * 255),
                                   pixel.a)
                               );
            } else {
                image.setPixel(j, i, qRgba(pixel.r, pixel.g, pixel.b, pixel.a));
            }
        }
    }

    return image;
}

QImage MainWindow::fromMonochrome(Monochrome monochrome) {
    int height = monochrome.getResolution().height;
    int width = monochrome.getResolution().width;

    QImage image(width, height, QImage::Format_Grayscale8);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int value = monochrome.getIndividualPixel(i, j);
            if (monochrome.getLevel() != 255) {
                value = (int) ((float) monochrome.getIndividualPixel(i, j) / monochrome.getLevel() * 255);
            }

            image.setPixel(j, i, qRgb(value, value, value));
        }
    }

    return image;
}

void MainWindow::brightening(float c, Operation o) {
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Monochrome prev = b->brightening(c, o);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                binaries.at(imageIdx) = new Binary(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
        break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Monochrome prev = g->brightening(c, o);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.at(imageIdx) = new Grayscale(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
           break;
        } case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Truecolor prev = tr->brightening(c, o);
            QImage image = this->fromTruecolor(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                truecolors.at(imageIdx) = new Truecolor(prev);
                QImage newImage = fromTruecolor(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

void MainWindow::on_actionAdd_2_triggered()
{
    try {
        float c = inputFloat("Brightening Add", "Insert c");
        this->brightening(c, Operation::ADD);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionSubtract_2_triggered()
{
    try {
        float c = inputFloat("Brightening Subtract", "Insert c");
        this->brightening(c, Operation::SUBTRACT);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionMultiply_2_triggered()
{
    try {
        float c = inputFloat("Brightening Multiply", "Insert c");
        this->brightening(c, Operation::MULTIPLY);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionDivision_2_triggered()
{
    try {
        float c = inputFloat("Brightening Division", "Insert c");
        this->brightening(c, Operation::DIVISION);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::operations(int idx, Operation o) {
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");

    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");
    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    int idx2 = tabWidget->currentIndex();
    TabPage * tabPage2 = (TabPage *) tabWidget->widget(idx2);
    QLabel * label2 = tabPage2->findChild<QLabel*>("label");
    int imageIdx2 = this->getVectorIdx(idx2, tabPage2->getImageType());

    if (tabPage->getImageType() == tabPage2->getImageType()) {
        switch (tabPage->getImageType()) {
            case (ImageType::BINARY) : {
                Binary * b = binaries.at(imageIdx);
                Binary * b2 = binaries.at(imageIdx2);

                if (Image::sameResolution(b->getResolution(), b2->getResolution())) {
                    qInfo("operations");
                    Monochrome prev = b2->operation(b, o, 1);

                    QImage image = this->fromMonochrome(prev);
                    ImagePreview imagePreview(this);
                    imagePreview.setImage(image);
                    int result = imagePreview.exec();
                    if (result == QDialog::Accepted) {
                        binaries.at(imageIdx) = new Binary(prev);
                        QImage newImage = fromMonochrome(prev);
                        label->setPixmap(QPixmap::fromImage(newImage));
                    }
                } else {
                    errorMessage("Images have different resolution");
                }
                break;
            }
            case (ImageType::GRAYSCALE) : {
                Grayscale * g = grayscales.at(imageIdx);
                Grayscale * g2 = grayscales.at(imageIdx2);

                if (Image::sameResolution(g->getResolution(), g2->getResolution())) {
                    Monochrome prev = g2->operation(g, o, g2->getLevel());

                    QImage image = this->fromMonochrome(prev);
                    ImagePreview imagePreview(this);
                    imagePreview.setImage(image);
                    int result = imagePreview.exec();
                    if (result == QDialog::Accepted) {
                        grayscales.at(imageIdx) = new Grayscale(prev);
                        QImage newImage = fromMonochrome(prev);
                        label->setPixmap(QPixmap::fromImage(newImage));
                    }
                } else {
                    errorMessage("Images have different resolution");
                }
                break;
            } case (ImageType::TRUECOLOR) : {
                Truecolor * tr = truecolors.at(imageIdx);
                Truecolor * tr2 = truecolors.at(imageIdx2);

                if (Image::sameResolution(tr->getResolution(), tr2->getResolution())) {
                    Truecolor prev = tr2->operation(tr, o, tr->getLevel());

                    QImage image = this->fromTruecolor(prev);
                    ImagePreview imagePreview(this);
                    imagePreview.setImage(image);
                    int result = imagePreview.exec();
                    if (result == QDialog::Accepted) {
                        truecolors.at(imageIdx) = new Truecolor(prev);
                        QImage newImage = fromTruecolor(prev);
                        label->setPixmap(QPixmap::fromImage(newImage));
                    }
                } else {
                    errorMessage("Images have different resolution");
                }
                break;
            }
        }
    } else {
        errorMessage("Images has different type");
    }
}

void MainWindow::errorMessage(QString s) {
    QMessageBox msgBox;
    msgBox.setText(s);
    msgBox.exec();
}


string MainWindow::inputString(QString title, QString question) {
    bool ok;
    QString text = QInputDialog::getText(
                this,
                title,
                question,
                QLineEdit::Normal,
                "",
                &ok
            );

    if (ok && !text.isEmpty()) {
        return text.toStdString();
    } else {
        throw "Error";
    }
}

int MainWindow::inputInt(QString title, QString question) {
    bool ok;
    QString text = QInputDialog::getText(
                this,
                title,
                question,
                QLineEdit::Normal,
                "",
                &ok
            );

    if (ok && !text.isEmpty()) {
        QTextStream intTextStream(&text);
        int c;
        intTextStream >> c;

        return c;
    } else {
        throw "Error";
    }
}

float MainWindow::inputFloat(QString title, QString question) {
    bool ok;
    QString text = QInputDialog::getText(
                this,
                title,
                question,
                QLineEdit::Normal,
                "",
                &ok
            );

    if (ok && !text.isEmpty()) {
        QTextStream floatTextStream(&text);
        float c;
        floatTextStream >> c;

        return c;
    } else {
        throw "Error";
    }
}

QString MainWindow::inputComboBox(QString title, QString question, QStringList list) {
    QString item = QInputDialog::getItem(
                this,
                title,
                question,
                list
            );

    if (!item.isEmpty()) {
        return item;
    } else {
        throw "Error";
    }
}

void MainWindow::on_actionAdd_triggered()
{
    try {
        int c = inputInt("Operation Add", "Insert index");
        this->operations(c, Operation::ADD);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionSubtract_triggered()
{
    try {
        int c = inputInt("Operation Subtract", "Insert index");
        this->operations(c, Operation::SUBTRACT);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionMultiply_triggered()
{
    try {
        int c = inputInt("Operation Multiply", "Insert index");
        this->operations(c, Operation::MULTIPLY);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionDivision_triggered()
{
    try {
        int c = inputInt("Operation Division", "Insert index");
        this->operations(c, Operation::DIVISION);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::geometry(Geometry geom) {
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Monochrome prev = b->geometry(geom);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                binaries.at(imageIdx) = new Binary(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
        break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Monochrome prev = g->geometry(geom);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.at(imageIdx) = new Grayscale(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
        break;
        } case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Truecolor prev = tr->geometry(geom);
            QImage image = this->fromTruecolor(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                truecolors.at(imageIdx) = new Truecolor(prev);
                QImage newImage = fromTruecolor(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

void MainWindow::on_actionVertical_triggered()
{
    geometry(Geometry::FLIPPING_VERTICAL);
}

void MainWindow::on_actionCartesian_triggered()
{
    geometry(Geometry::MIRROR_CARTESIAN);
}

void MainWindow::on_actionHorizontal_triggered()
{
    geometry(Geometry::FLIPPING_HORIZONTAL);
}

void MainWindow::on_actionx_y_triggered()
{
    geometry(Geometry::MIRROR_X_Y);
}

void MainWindow::histogram(Histogram h, bool normalized) {
    qInfo(to_string(h.size).c_str());

    QBarSet *set = new QBarSet("amount");
    QStringList grayscales;

    for (int i = 0; i < h.size; i++) {
        grayscales << QString::fromStdString(to_string(i));
        if (normalized) {
            *set << h.norm[i];
        } else {
            *set << h.value[i];
        }
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    if (normalized) {
        chart->setTitle("Histogram Normalisasi");
    } else {
        chart->setTitle("Histogram Normal");
    }

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(grayscales);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    axisX->setRange(to_string(0).c_str(), to_string(h.size - 1).c_str());

    QValueAxis *axisY = new QValueAxis();
    if (normalized) {
        axisY->setRange(0, h.max/h.total);
    } else {
        axisY->setRange(0, h.max);
    }
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(chartView);
    QFormLayout *formLayout = new QFormLayout;

    QLabel *mean = new QLabel(window);
    mean->setText("Mean");
    QLabel *mean_value = new QLabel(window);
    mean_value->setNum(h.mean);
    QLabel *var = new QLabel(window);
    var->setText("Variance");
    QLabel *var_value = new QLabel(window);
    var_value->setNum(h.var);
    QLabel *std = new QLabel(window);
    std->setText("Standard Deviasion");
    QLabel *std_value = new QLabel(window);
    std_value->setNum(h.std);

    formLayout->addRow(mean, mean_value);
    formLayout->addRow(var, var_value);
    formLayout->addRow(std, std_value);
    layout->addLayout(formLayout);
    window->setWindowModality(Qt::WindowModal);
    window->setCentralWidget(new QWidget);
    window->centralWidget()->setLayout(layout);

    window->resize(420, 300);
    window->setParent(this);
    window->show();
}

void MainWindow::on_actionNormal_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Histogram h = b->generateHistogram();
            histogram(h, false);
            break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Histogram h = g->generateHistogram();
            histogram(h, false);
            break;
        }
    }
}

void MainWindow::on_actionContrast_Stretching_triggered()
{
    FormContrast form(this);
    form.setWindowTitle("Contrast Stretching");
    int result = form.exec();

    qInfo("after form");

    if (result == QDialog::Accepted) {
        qInfo("lala");
        QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
        int idx = tabWidget->currentIndex();
        TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
        QLabel * label = tabPage->findChild<QLabel*>("label");
        qInfo("lala");
        int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

        switch (tabPage->getImageType()) {
            case (ImageType::BINARY) : {
                errorMessage("Cannot used for Binary");
                break;
            } case (ImageType::GRAYSCALE) : {
                Grayscale * g = grayscales.at(imageIdx);
                Monochrome prev = g->contrastStretching(
                            form.getA(),
                            form.getB(),
                            form.getYa(),
                            form.getYb(),
                            form.getAlpha(),
                            form.getBeta(),
                            form.getGamma()
                        );

                QImage image = this->fromMonochrome(prev);
                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    grayscales.at(imageIdx) = new Grayscale(prev);
                    QImage newImage = fromMonochrome(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
                break;
            } case (ImageType::TRUECOLOR) : {
                Truecolor * tr = truecolors.at(imageIdx);
                Truecolor prev = tr->contrastStretching(
                            form.getA(),
                            form.getB(),
                            form.getYa(),
                            form.getYb(),
                            form.getAlpha(),
                            form.getBeta(),
                            form.getGamma()
                        );
                QImage image = this->fromTruecolor(prev);

                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    truecolors.at(imageIdx) = new Truecolor(prev);
                    QImage newImage = fromTruecolor(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
                break;
            }
        }
    }
}

void MainWindow::transformation(Transformation t, float c, float gamma) {
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            errorMessage("Cannot be used for binary");
            break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            qInfo("getA");
            Monochrome prev = g->transformation(t, c, gamma);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.at(imageIdx) = new Grayscale(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
        case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Truecolor prev = tr->transformation(t, c, gamma);
            QImage image = this->fromTruecolor(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                truecolors.at(imageIdx) = new Truecolor(prev);
                QImage newImage = fromTruecolor(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

void MainWindow::on_actionLog_triggered()
{
    try {
        float c = inputFloat("Transformation Log", "Insert c");
        this->transformation(Transformation::LOG, c, 1);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionInverse_Log_triggered()
{
    try {
        float c = inputFloat("Transformation Inverse Log", "Insert c");
        this->transformation(Transformation::INVERSE_LOG, c, 1);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionPower_triggered()
{
    FormTransformationLog form(this);
    form.setWindowTitle("Contrast Stretching");
    int result = form.exec();

    if (result == QDialog::Accepted) {
        this->transformation(Transformation::POWER, form.getC(), form.getGamma());
    }
}

void MainWindow::on_actionGray_Level_Slicing_triggered()
{
    FormGraySlicing form(this);
    form.setWindowTitle("Gray Level Slicing");
    int result = form.exec();

    if (result == QDialog::Accepted) {
        QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
        int idx = tabWidget->currentIndex();
        TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
        QLabel * label = tabPage->findChild<QLabel*>("label");
        int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

        switch (tabPage->getImageType()) {
            case (ImageType::BINARY) : {
                errorMessage("Cannot used for Binary");
                break;
            } case (ImageType::GRAYSCALE) : {
                Grayscale * g = grayscales.at(imageIdx);
                Grayscale prev = g->slicing(
                            form.getA(),
                            form.getB(),
                            form.getMax()
                        );

                QImage image = this->fromMonochrome(prev);
                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    grayscales.at(imageIdx) = new Grayscale(prev);
                    QImage newImage = fromMonochrome(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
                break;
            } case (ImageType::TRUECOLOR) : {
                Truecolor * g = truecolors.at(imageIdx);
                Truecolor prev = g->slicing(
                            form.getA(),
                            form.getB(),
                            form.getMax()
                        );

                QImage image = this->fromTruecolor(prev);
                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    truecolors.at(imageIdx) = new Truecolor(prev);
                    QImage newImage = fromTruecolor(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
            }
        }
    }
}

void MainWindow::bitPlane(PPMColorState color)
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");
    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
            case (ImageType::BINARY) : {
            errorMessage("Cannot used for Binary");
            break;
        } case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);

            vector<Binary*> prev = g->bitSlicing();

            QImage image0 = this->fromBinary(*prev[0]);
            QImage image1 = this->fromBinary(*prev[1]);
            QImage image2 = this->fromBinary(*prev[2]);
            QImage image3 = this->fromBinary(*prev[3]);
            QImage image4 = this->fromBinary(*prev[4]);
            QImage image5 = this->fromBinary(*prev[5]);
            QImage image6 = this->fromBinary(*prev[6]);
            QImage image7 = this->fromBinary(*prev[7]);

            BitPlane bitPlane(this);
            bitPlane.setBit0(image0);
            bitPlane.setBit1(image1);
            bitPlane.setBit2(image2);
            bitPlane.setBit3(image3);
            bitPlane.setBit4(image4);
            bitPlane.setBit5(image5);
            bitPlane.setBit6(image6);
            bitPlane.setBit7(image7);

            bitPlane.exec();

            break;
        } case (ImageType::TRUECOLOR) : {
            Truecolor * g = truecolors.at(imageIdx);

            vector<Binary*> prev = g->bitSlicing(color);

            QImage image0 = this->fromBinary(*prev[0]);
            QImage image1 = this->fromBinary(*prev[1]);
            QImage image2 = this->fromBinary(*prev[2]);
            QImage image3 = this->fromBinary(*prev[3]);
            QImage image4 = this->fromBinary(*prev[4]);
            QImage image5 = this->fromBinary(*prev[5]);
            QImage image6 = this->fromBinary(*prev[6]);
            QImage image7 = this->fromBinary(*prev[7]);

            BitPlane bitPlane(this);
            bitPlane.setBit0(image0);
            bitPlane.setBit1(image1);
            bitPlane.setBit2(image2);
            bitPlane.setBit3(image3);
            bitPlane.setBit4(image4);
            bitPlane.setBit5(image5);
            bitPlane.setBit6(image6);
            bitPlane.setBit7(image7);

            bitPlane.exec();

            break;
        }
    }
}

void MainWindow::on_actionNormalisasi_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Histogram h = b->generateHistogram();
            histogram(h, true);
            break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Histogram h = g->generateHistogram();
            histogram(h, true);
            break;
        }
    }
}

void MainWindow::on_actionHistogram_Leveling_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Monochrome prev = b->histogramLeveling();

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                binaries.at(imageIdx) = new Binary(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Monochrome prev = g->histogramLeveling();

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.at(imageIdx) = new Grayscale(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
        case (ImageType::TRUECOLOR) : {
            Truecolor * g = truecolors.at(imageIdx);
            Truecolor prev = g->histogramLeveling();

            QImage image = this->fromTruecolor(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                truecolors.at(imageIdx) = new Truecolor(prev);
                QImage newImage = fromTruecolor(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

void MainWindow::convolution(Convolution c, Padding pad, int size, float **kernel) {
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Monochrome prev = b->convolution(c, pad, size, kernel);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                binaries.at(imageIdx) = new Binary(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Monochrome prev = g->convolution(c, pad, size, kernel);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.at(imageIdx) = new Grayscale(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
        case (ImageType::TRUECOLOR): {
            Truecolor * tr = truecolors.at(imageIdx);
            Truecolor prev = tr->convolution(c, pad, size, kernel);

            QImage image = this->fromTruecolor(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                truecolors.at(imageIdx) = new Truecolor(prev);
                QImage newImage = fromTruecolor(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

void MainWindow::on_actionMedian_Filter_triggered()
{
    this->convolution(Convolution::MEDIAN, Padding::SAME, 3, 0);
}

void MainWindow::on_action1_triggered()
{
    float **kernel = new float*[3];
    for (int i =0; i < 3; i++) {
        kernel[i] = new float[3];
    }
    kernel[0][0] = -1; kernel[0][1] = -1; kernel[0][2] = -1;
    kernel[1][0] = -1; kernel[1][1] = 8; kernel[1][2] = -1;
    kernel[2][0] = -1; kernel[2][1] = -1; kernel[2][2] = -1;

    this->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
}

void MainWindow::on_action2_triggered()
{
    float **kernel = new float*[3];
    for (int i =0; i < 3; i++) {
        kernel[i] = new float[3];
    }
    kernel[0][0] = -1; kernel[0][1] = -1; kernel[0][2] = -1;
    kernel[1][0] = -1; kernel[1][1] = 9; kernel[1][2] = -1;
    kernel[2][0] = -1; kernel[2][1] = -1; kernel[2][2] = -1;

    this->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
}

void MainWindow::on_action3_triggered()
{
    float **kernel = new float*[3];
    for (int i =0; i < 3; i++) {
        kernel[i] = new float[3];
    }
    kernel[0][0] = 0; kernel[0][1] = -1; kernel[0][2] = 0;
    kernel[1][0] = -1; kernel[1][1] = 5; kernel[1][2] = -1;
    kernel[2][0] = 0; kernel[2][1] = -1; kernel[2][2] = 0;

    this->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
}

void MainWindow::on_action4_triggered()
{
    float **kernel = new float*[3];
    for (int i =0; i < 3; i++) {
        kernel[i] = new float[3];
    }
    kernel[0][0] = 1; kernel[0][1] = -2; kernel[0][2] = 1;
    kernel[1][0] = -2; kernel[1][1] = 5; kernel[1][2] = -2;
    kernel[2][0] = 1; kernel[2][1] = -2; kernel[2][2] = 1;

    this->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
}

void MainWindow::on_action5_triggered()
{
    float **kernel = new float*[3];
    for (int i =0; i < 3; i++) {
        kernel[i] = new float[3];
    }
    kernel[0][0] = 1; kernel[0][1] = -2; kernel[0][2] = 1;
    kernel[1][0] = -2; kernel[1][1] = 4; kernel[1][2] = -2;
    kernel[2][0] = 1; kernel[2][1] = -2; kernel[2][2] = 1;

    this->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
}

void MainWindow::on_action6_triggered()
{
    float **kernel = new float*[3];
    for (int i =0; i < 3; i++) {
        kernel[i] = new float[3];
    }
    kernel[0][0] = 0; kernel[0][1] = 1; kernel[0][2] = 0;
    kernel[1][0] = 1; kernel[1][1] = -4; kernel[1][2] = 1;
    kernel[2][0] = 0; kernel[2][1] = 1; kernel[2][2] = 0;

    this->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
}

void MainWindow::on_actionUnsharp_Masking_triggered()
{
    float **kernel = new float*[3];
    for (int i =0; i < 3; i++) {
        kernel[i] = new float[3];
    }
    kernel[0][0] = 1.0/10; kernel[0][1] = 1.0/10; kernel[0][2] = 1.0/10;
    kernel[1][0] = 1.0/10; kernel[1][1] = 1.0/5; kernel[1][2] = 1.0/10;
    kernel[2][0] = 1.0/10; kernel[2][1] = 1.0/10; kernel[2][2] = 1.0/10;

    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Monochrome low = b->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
            Monochrome prev = b->sharpening(&low, 2);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                binaries.at(imageIdx) = new Binary(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
           break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Monochrome low = g->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
            Monochrome prev = g->sharpening(&low, 2);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.at(imageIdx) = new Grayscale(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
           break;
        }
        case (ImageType::TRUECOLOR): {
            Truecolor * tr = truecolors.at(imageIdx);
            Truecolor low = tr->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
            Truecolor prev = tr->sharpening(&low, 2);

            QImage image = this->fromTruecolor(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                truecolors.at(imageIdx) = new Truecolor(prev);
                QImage newImage = fromTruecolor(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

void MainWindow::on_actionIn_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");
    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Monochrome prev = b->zoom(true);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                binaries.at(imageIdx) = new Binary(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Monochrome prev = g->zoom(true);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.at(imageIdx) = new Grayscale(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        } case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Truecolor prev = tr->zoom(true);
            QImage image = this->fromTruecolor(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                truecolors.at(imageIdx) = new Truecolor(prev);
                QImage newImage = fromTruecolor(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

void MainWindow::on_actionOut_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");
    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Monochrome prev = b->zoom(false);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                binaries.at(imageIdx) = new Binary(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Monochrome prev = g->zoom(false);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.at(imageIdx) = new Grayscale(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        } case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Truecolor prev = tr->zoom(false);
            QImage image = this->fromTruecolor(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                truecolors.at(imageIdx) = new Truecolor(prev);
                QImage newImage = fromTruecolor(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

void MainWindow::on_actionTranslation_triggered()
{
    FormTranslation form(this);
    form.setWindowTitle("Translation");
    int result = form.exec();

    if (result == QDialog::Accepted) {
        QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
        int idx = tabWidget->currentIndex();
        TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
        QLabel * label = tabPage->findChild<QLabel*>("label");
        int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

        switch (tabPage->getImageType()) {
            case (ImageType::BINARY) : {
                Binary * b = binaries.at(imageIdx);
                Monochrome prev = b->translastion(
                            form.getX(),
                            form.getY()
                        );

                QImage image = this->fromMonochrome(prev);
                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    binaries.at(imageIdx) = new Binary(prev);
                    QImage newImage = fromMonochrome(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }

                break;
            } case (ImageType::GRAYSCALE) : {
                Grayscale * g = grayscales.at(imageIdx);
                Monochrome prev = g->translastion(
                            form.getX(),
                            form.getY()
                        );

                QImage image = this->fromMonochrome(prev);
                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    grayscales.at(imageIdx) = new Grayscale(prev);
                    QImage newImage = fromMonochrome(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
                break;
            } case (ImageType::TRUECOLOR) : {
                Truecolor * tr = truecolors.at(imageIdx);
                Truecolor prev = tr->translastion(
                            form.getX(),
                            form.getY()
                        );

                QImage image = this->fromTruecolor(prev);
                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    truecolors.at(imageIdx) = new Truecolor(prev);
                    QImage newImage = fromTruecolor(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
                break;
            }
        }
    }
}

void MainWindow::on_actionRed_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Histogram h = tr->generateHistogram(PPMColorState::RED);
            histogram(h, false);
            break;
        }
    }
}

void MainWindow::on_actionGreen_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Histogram h = tr->generateHistogram(PPMColorState::GREEN);
            histogram(h, false);
            break;
        }
    }
}

void MainWindow::on_actionBlue_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Histogram h = tr->generateHistogram(PPMColorState::BLUE);
            histogram(h, false);
            break;
        }
    }
}

void MainWindow::on_actionRed_2_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Histogram h = tr->generateHistogram(PPMColorState::RED);
            histogram(h, true);
            break;
        }
    }
}

void MainWindow::on_actionGreen_2_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Histogram h = tr->generateHistogram(PPMColorState::GREEN);
            histogram(h, true);
            break;
        }
    }
}

void MainWindow::on_actionBlue_2_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Histogram h = tr->generateHistogram(PPMColorState::BLUE);
            histogram(h, true);
            break;
        }
    }
}

void MainWindow::on_actionHighboost_Filter_triggered()
{
    try {
        float alpha = inputFloat("Highboost filter", "Insert alpha");
        float **kernel = new float*[3];
        for (int i =0; i < 3; i++) {
            kernel[i] = new float[3];
        }
        kernel[0][0] = 1.0/10; kernel[0][1] = 1.0/10; kernel[0][2] = 1.0/10;
        kernel[1][0] = 1.0/10; kernel[1][1] = 1.0/5; kernel[1][2] = 1.0/10;
        kernel[2][0] = 1.0/10; kernel[2][1] = 1.0/10; kernel[2][2] = 1.0/10;

        QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
        int idx = tabWidget->currentIndex();
        TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
        QLabel * label = tabPage->findChild<QLabel*>("label");

        int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

        switch (tabPage->getImageType()) {
            case (ImageType::BINARY) : {
                Binary * b = binaries.at(imageIdx);
                Monochrome low = b->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
                Monochrome prev = b->sharpening(&low, alpha);

                QImage image = this->fromMonochrome(prev);
                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    binaries.at(imageIdx) = new Binary(prev);
                    QImage newImage = fromMonochrome(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
               break;
            }
            case (ImageType::GRAYSCALE) : {
                Grayscale * g = grayscales.at(imageIdx);
                Monochrome low = g->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
                Monochrome prev = g->sharpening(&low, alpha);

                QImage image = this->fromMonochrome(prev);
                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    grayscales.at(imageIdx) = new Grayscale(prev);
                    QImage newImage = fromMonochrome(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
               break;
            }
            case (ImageType::TRUECOLOR): {
                Truecolor * tr = truecolors.at(imageIdx);
                Truecolor low = tr->convolution(Convolution::BASIC, Padding::SAME, 3, kernel);
                Truecolor prev = tr->sharpening(&low, alpha);

                QImage image = this->fromTruecolor(prev);
                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    truecolors.at(imageIdx) = new Truecolor(prev);
                    QImage newImage = fromTruecolor(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
                break;
            }
        }
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::rotation(Geometry t) {
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Monochrome prev = b->geometry(t);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                binaries.at(imageIdx) = new Binary(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
        break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Monochrome prev = g->geometry(t);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.at(imageIdx) = new Grayscale(prev);
                QImage newImage = fromMonochrome(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
           break;
        } case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Truecolor prev = tr->geometry(t);
            QImage image = this->fromTruecolor(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                truecolors.at(imageIdx) = new Truecolor(prev);
                QImage newImage = fromTruecolor(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

void MainWindow::on_action90_triggered()
{
    rotation(Geometry::ROTATION_90);
}

void MainWindow::on_action180_triggered()
{
    rotation(Geometry::ROTATION_180);
}

void MainWindow::on_action270_triggered()
{
    rotation(Geometry::ROTATION_270);
}

void MainWindow::on_actionHistogram_Specification_triggered()
{
    int idx = inputInt("Operation Add", "Insert index");

    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");

    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");
    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    int idx2 = tabWidget->currentIndex();
    TabPage * tabPage2 = (TabPage *) tabWidget->widget(idx2);
    QLabel * label2 = tabPage2->findChild<QLabel*>("label");
    int imageIdx2 = this->getVectorIdx(idx2, tabPage2->getImageType());

    if (tabPage->getImageType() == tabPage2->getImageType()) {
        switch (tabPage->getImageType()) {
            case (ImageType::BINARY) : {
                Binary * b = binaries.at(imageIdx);
                Binary * b2 = binaries.at(imageIdx2);

                Histogram h = b->generateHistogram();

                if (b->getLevel() == b2->getLevel()) {
                    Monochrome prev = b2->histogramSpecification(h);

                    QImage image = this->fromMonochrome(prev);
                    ImagePreview imagePreview(this);
                    imagePreview.setImage(image);
                    int result = imagePreview.exec();
                    if (result == QDialog::Accepted) {
                        binaries.at(imageIdx) = new Binary(prev);
                        QImage newImage = fromMonochrome(prev);
                        label->setPixmap(QPixmap::fromImage(newImage));
                    }
                } else {
                    errorMessage("Images have different level");
                }
                break;
            }
            case (ImageType::GRAYSCALE) : {
                Grayscale * g = grayscales.at(imageIdx);
                Grayscale * g2 = grayscales.at(imageIdx2);

                Histogram h = g->generateHistogram();

                if (g->getLevel() == g2->getLevel()) {
                    Monochrome prev = g2->histogramSpecification(h);

                    QImage image = this->fromMonochrome(prev);
                    ImagePreview imagePreview(this);
                    imagePreview.setImage(image);
                    int result = imagePreview.exec();
                    if (result == QDialog::Accepted) {
                        grayscales.at(imageIdx) = new Grayscale(prev);
                        QImage newImage = fromMonochrome(prev);
                        label->setPixmap(QPixmap::fromImage(newImage));
                    }
                } else {
                    errorMessage("Images have different level");
                }
                break;
            } case (ImageType::TRUECOLOR) : {
                Truecolor * tr = truecolors.at(imageIdx);
                Truecolor * tr2 = truecolors.at(imageIdx2);

                Histogram hr = tr->generateHistogram(PPMColorState::RED);
                Histogram hg = tr->generateHistogram(PPMColorState::GREEN);
                Histogram hb = tr->generateHistogram(PPMColorState::BLUE);

                if (Image::sameResolution(tr->getResolution(), tr2->getResolution())) {
                    Truecolor prev = tr2->histogramSpecification(hr, hg, hb);

                    QImage image = this->fromTruecolor(prev);
                    ImagePreview imagePreview(this);
                    imagePreview.setImage(image);
                    int result = imagePreview.exec();
                    if (result == QDialog::Accepted) {
                        truecolors.at(imageIdx) = new Truecolor(prev);
                        QImage newImage = fromTruecolor(prev);
                        label->setPixmap(QPixmap::fromImage(newImage));
                    }
                } else {
                    errorMessage("Images have different resolution");
                }
                break;
            }
        }
    } else {
        errorMessage("Images has different type");
    }
}

void MainWindow::on_actionAbout_Image_triggered()
{
    QMainWindow * window = new QMainWindow(this);

    QFormLayout *formLayout = new QFormLayout;

    QLabel *width = new QLabel(window);
    width->setText("Width");
    QLabel *width_value = new QLabel(window);
    QLabel *height = new QLabel(window);
    height->setText("Height");
    QLabel *height_value = new QLabel(window);
    QLabel *level = new QLabel(window);
    level->setText("Level");
    QLabel *level_value = new QLabel(window);
    QLabel *type = new QLabel(window);
    type->setText("Image Type");
    QLabel *type_value = new QLabel(window);
    QLabel *format= new QLabel(window);
    format->setText("Format Type");
    QLabel *format_value = new QLabel(window);

    qInfo("tes");

    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);

            width_value->setNum((int) b->getResolution().width);
            qInfo("tes");
            height_value->setNum((int) b->getResolution().height);
            qInfo("tes");
            level_value->setNum(b->getLevel());
            qInfo("tes");
            type_value->setText("Binary");
            qInfo("tes");
            format_value->setText(image_format::toString(b->getImageFormat()).c_str());
            qInfo("tes");
            break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * b = grayscales.at(imageIdx);

            width_value->setNum((int) b->getResolution().width);
            height_value->setNum((int) b->getResolution().height);
            level_value->setNum(b->getLevel());
            type_value->setText("Grayscale");
            format_value->setText(image_format::toString(b->getImageFormat()).c_str());
            break;
        }
        case (ImageType::TRUECOLOR) : {
            Truecolor * b = truecolors.at(imageIdx);

            width_value->setNum((int) b->getResolution().width);
            height_value->setNum((int) b->getResolution().height);
            level_value->setNum(b->getLevel());
            type_value->setText("Truecolor");
            format_value->setText(image_format::toString(b->getImageFormat()).c_str());
            break;
        }
    }

    formLayout->addRow(type, type_value);
    formLayout->addRow(format, format_value);
    formLayout->addRow(width, width_value);
    formLayout->addRow(height, height_value);
    formLayout->addRow(level, level_value);

    window->setWindowModality(Qt::WindowModal);
    window->setCentralWidget(new QWidget);
    window->centralWidget()->setLayout(formLayout);

    window->resize(200, 200);
    window->setParent(this);
    window->show();
}

void MainWindow::on_actionAnd_triggered()
{
    try {
        int c = inputInt("Operation And", "Insert index");
        this->operations(c, Operation::AND);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionOr_triggered()
{
    try {
        int c = inputInt("Operation OR", "Insert index");
        this->operations(c, Operation::OR);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionGrayscale_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Grayscale prev = tr->toGrayscale();
            QImage image = this->fromGrayscale(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();

            if (result == QDialog::Accepted) {
                truecolors.erase(truecolors.begin() + imageIdx);
                int newIdx = this->getVectorIdx(idx, ImageType::GRAYSCALE);
                grayscales.insert(grayscales.begin() + newIdx, new Grayscale(prev));
                QImage newImage = fromGrayscale(prev);
                tabPage->setImageType(ImageType::GRAYSCALE);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
        case (ImageType::BINARY): {
            Binary * b = binaries.at(imageIdx);
            Grayscale prev(*b);
            QImage image = this->fromGrayscale(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();

            if (result == QDialog::Accepted) {
                binaries.erase(binaries.begin() + imageIdx);
                int newIdx = this->getVectorIdx(idx, ImageType::GRAYSCALE);
                grayscales.insert(grayscales.begin() + newIdx, new Grayscale(prev));
                QImage newImage = fromGrayscale(prev);
                tabPage->setImageType(ImageType::GRAYSCALE);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}

void MainWindow::on_actionGrayscale_2_triggered()
{
    this->bitPlane();
}


void MainWindow::on_actionRed_3_triggered()
{
    this->bitPlane(PPMColorState::RED);
}

void MainWindow::on_actionGreen_3_triggered()
{
    this->bitPlane(PPMColorState::GREEN);
}

void MainWindow::on_actionBlue_3_triggered()
{
    this->bitPlane(PPMColorState::BLUE);
}

void MainWindow::on_actionSave_triggered()
{
    string filename = this->inputString("Save", "Insert file name");

    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            b->save(filename);
            break;
        }
        case (ImageType::GRAYSCALE) : {
            Grayscale * b = grayscales.at(imageIdx);
            b->save(filename);
            break;
        }
        case (ImageType::TRUECOLOR) : {
            Truecolor * b = truecolors.at(imageIdx);
            b->save(filename);
            break;
        }
    }
}

void MainWindow::edgeDetection(EdgeDetection e, int t, int c, float lowT, float highT, float sigma) {
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");
    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            errorMessage("Cannot used for Binary");
            break;
        } case (ImageType::GRAYSCALE) : {
            Grayscale * g = grayscales.at(imageIdx);
            Binary b = g->edgeDetection(e, t, c, lowT, highT, sigma);

            QImage image = this->fromBinary(b);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                grayscales.erase(grayscales.begin() + imageIdx);
                int newIdx = this->getVectorIdx(idx, ImageType::BINARY);
                binaries.insert(binaries.begin() + newIdx, new Binary(b));
                QImage newImage = fromBinary(b);
                tabPage->setImageType(ImageType::BINARY);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        } case (ImageType::TRUECOLOR) : {
            errorMessage("Cannot used for Truecolor");
            break;
        }
    }
}

void MainWindow::on_actionGradient_triggered()
{
    try {
        int t = inputInt("Edge detection gradient", "Insert threshold");
        this->edgeDetection(EdgeDetection::GRADIENT, t);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionLaplace_triggered()
{
    try {
        int t = inputInt("Edge detection laplace", "Insert threshold");
        this->edgeDetection(EdgeDetection::LAPLACE, t);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionLaplace_of_Gaussian_triggered()
{
    this->edgeDetection(EdgeDetection::L_O_G);
}

void MainWindow::on_actionSobel_triggered()
{
    formsobel form(this);
    form.setWindowTitle("Edge detection Sobel");
    int result = form.exec();

    qInfo("after form");

    if (result == QDialog::Accepted) {
        qInfo(to_string(form.getThreshold()).c_str());
        qInfo(to_string(form.getC()).c_str());

        this->edgeDetection(EdgeDetection::SOBEL, form.getThreshold(), form.getC());
    }
}

void MainWindow::on_actionPrewitt_triggered()
{
    try {
        int t = inputInt("Edge detection Prewitt", "Insert threshold");
        this->edgeDetection(EdgeDetection::PREWITT, t);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionRoberts_triggered()
{
    try {
        int t = inputInt("Edge detection Prewitt", "Insert threshold");
        this->edgeDetection(EdgeDetection::ROBERTS, t);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionCanny_triggered()
{
    FormCanny form(this);
    form.setWindowTitle("Edge detection Canny");
    int result = form.exec();

    qInfo("after form");

    if (result == QDialog::Accepted) {
        this->edgeDetection(EdgeDetection::CANNY, 1, form.getC(), form.getLowThreshold(), form.getHighThreshold(), form.getSigma());
    }
}

void MainWindow::on_actionBinary_Segmentation_triggered()
{
    try {
        int t = inputInt("Binary Segmentation", "Insert threshold");
        QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
        int idx = tabWidget->currentIndex();
        TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
        QLabel * label = tabPage->findChild<QLabel*>("label");
        int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

        switch (tabPage->getImageType()) {
            case (ImageType::GRAYSCALE) : {
                Grayscale * g = grayscales.at(imageIdx);
                Binary prev = g->binarySegmentation(t);

                QImage image = this->fromBinary(prev);

                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    grayscales.erase(grayscales.begin() + imageIdx);
                    int newIdx = this->getVectorIdx(idx, ImageType::BINARY);
                    binaries.insert(binaries.begin() + newIdx, new Binary(prev));
                    QImage newImage = fromBinary(prev);
                    tabPage->setImageType(ImageType::BINARY);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
                break;
            }
            case (ImageType::TRUECOLOR) : {
                Truecolor * tr = truecolors.at(imageIdx);
                Grayscale g = tr->toGrayscale();
                Binary prev = g.binarySegmentation(t);

                QImage image = this->fromBinary(prev);

                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    truecolors.erase(truecolors.begin() + imageIdx);
                    int newIdx = this->getVectorIdx(idx, ImageType::BINARY);
                    binaries.insert(binaries.begin() + newIdx, new Binary(prev));
                    QImage newImage = fromBinary(prev);
                    tabPage->setImageType(ImageType::BINARY);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
                break;
            }
        }
    } catch (const char* msg) {

    }
}

void MainWindow::on_actionThinning_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");
    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Binary prev = b->thinning();

            QImage image = this->fromBinary(prev);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                binaries.at(imageIdx) = new Binary(prev);
                QImage newImage = fromBinary(prev);
                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        } default : {
            errorMessage("Cannot used for Binary");
            break;
        }
    }
}

void MainWindow::on_actionNumber_Plate_Detector_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");
    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b4 = binaries.at(imageIdx);
            Grayscale box = b4->cclTwoPass();

//            Binary b5 = b4->boundaryBoxPlate(&box, 0.7, 0.2);

            QImage image = this->fromBinary(*b4);
//            QImage image = this->fromGrayscale(box);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
                QImage newImage = fromBinary(*b4);
                tabPage->setImageType(ImageType::BINARY);
                label->setPixmap(QPixmap::fromImage(newImage));
            }

            break;
        }
        case (ImageType::TRUECOLOR) : {
            Truecolor * tr = truecolors.at(imageIdx);
            Grayscale g1 = tr->toGrayscale();

            Binary b1 = g1.binarySegmentation(80);
            Grayscale g2(b1);

            Grayscale g_3 = g1.negative();
            Binary b_13 = g_3.binarySegmentation(80);
            Grayscale g2_3(b_13);

            Binary b22 = g1.edgeDetection(EdgeDetection::PREWITT, 250, 1);
//            Binary b22 = g1.edgeDetection(EdgeDetection::ROBERTS, 150, 1);
//            Binary b2 = g1.edgeDetection(EdgeDetection::CANNY, 100, 1);
            Binary b2 = g2.edgeDetection(EdgeDetection::GRADIENT, 1, 1);
//            Binary b2 = b1;
            Binary b2_3 = g2_3.edgeDetection(EdgeDetection::GRADIENT, 1, 1);

            float ** kernel2 = Monochrome::initPixel(3, 3);
            kernel2[0][0] = 1; kernel2[0][1] = 1;  kernel2[0][2] = 1;
            kernel2[1][0] = 1; kernel2[1][1] = 1; kernel2[1][2] = 1;
            kernel2[2][0] = 1; kernel2[2][1] = 1;  kernel2[2][2] = 1;


//            Binary b4 = b2.thinning();

//
            Binary b3a = b2.convolution(Convolution::BASIC, Padding::ZERO, 3, kernel2);
            Binary b3a_3 = b2_3.convolution(Convolution::BASIC, Padding::ZERO, 3, kernel2);

//            Binary b23 = b22.convolution(Convolution::BASIC, Padding::ZERO, 3, kernel2);
            Binary b3a2 = b22.convolution(Convolution::BASIC, Padding::ZERO, 3, kernel2);

            QImage image = this->fromBinary(b2_3);

            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            imagePreview.exec();

            Grayscale box = b3a.cclTwoPass();
            Grayscale box2 = b3a2.cclTwoPass();
            Grayscale box_3 = b3a_3.cclTwoPass();

            int max_count = 0;
            int max_index = 0;

            int max_count_2 = 0;
            int max_index_2 = 0;

            int max_count_3 = 0;
            int max_index_3 = 0;

            vector<Monochrome*> vecMon = b3a.boundaryBox(&box, &g1, 4, 2.5);
            vector<Monochrome*> vecMon_2 = b3a2.boundaryBox(&box2, &g1, 4, 2.5);
            vector<Monochrome*> vecMon_3 = b3a_3.boundaryBox(&box_3, &g_3, 4, 2.5);

            for (int i = 0; i < vecMon.size(); i++) {
                Grayscale g_temp(*vecMon[i]);
                Binary b_temp = g_temp.binarySegmentation(80);
                Grayscale box_temp = b_temp.cclTwoPass();

                int count = b_temp.boundaryBoxCount(&box_temp, 0.7, 0.2);

                if (max_count < count) {
                    max_count = count;
                    max_index = i;
                }
            }

            for (int i = 0; i < vecMon_2.size(); i++) {
                Grayscale g_temp(*vecMon_2[i]);
                Binary b_temp = g_temp.binarySegmentation(200);
                Grayscale box_temp = b_temp.cclTwoPass();

                int count = b_temp.boundaryBoxCount(&box_temp, 0.7, 0.2);

                if (max_count_2 < count) {
                    max_count_2 = count;
                    max_index_2 = i;
                }
            }

            for (int i = 0; i < vecMon_3.size(); i++) {
                Grayscale g_temp(*vecMon_3[i]);
                Binary b_temp = g_temp.binarySegmentation(200);
                Grayscale box_temp = b_temp.cclTwoPass();

                int count = b_temp.boundaryBoxCount(&box_temp, 0.7, 0.2);

                if (max_count_3 < count) {
                    max_count_3 = count;
                    max_index_3 = i;
                }
            }

//            QImage image = this->fromBinary(b5);

            Grayscale g_a1(*vecMon[max_index]);
            Grayscale g_a1_2(*vecMon_2[max_index_2]);
            Grayscale g_a1_3(*vecMon_3[max_index_3]);

            int counter = 0;
            for (int i = 0; i < g_a1.getResolution().height; i++) {
                for (int j = 0; j < g_a1.getResolution().width; j++) {
                    if (g_a1.getIndividualPixel(i, j) < 130) {
                        counter++;
                    } else {
                        counter--;
                    }
                }
            }

            image = this->fromGrayscale(g_a1_2);

            imagePreview.setImage(image);
            imagePreview.exec();

            image = this->fromGrayscale(g_a1_3);
            imagePreview.setImage(image);
            imagePreview.exec();

            Binary b_a1 = g_a1.binarySegmentation(80);
            Grayscale box_2 = b_a1.cclTwoPass();

            Binary b_a1_2 = g_a1_2.binarySegmentation(200);
            Grayscale box_2_2 = b_a1_2.cclTwoPass();

            Binary b_a1_3 = g_a1_3.binarySegmentation(80);

            Binary b_a1_3s(b_a1_3.reverseFormat());

            Grayscale box_2_3 = b_a1_3s.cclTwoPass();

            image = this->fromBinary(b_a1_2);

            imagePreview.setImage(image);
            imagePreview.exec();

            image = this->fromBinary(b_a1_3s);

            imagePreview.setImage(image);
            imagePreview.exec();

            vector<Monochrome*> vecMon2 = b_a1.boundaryBoxPlate(&box_2, 0.7, 0.15);
            vector<Monochrome*> vecMon2_2 = b_a1_2.boundaryBoxPlate(&box_2_2, 0.7, 0.15);
            vector<Monochrome*> vecMon2_3 = b_a1_3s.boundaryBoxPlate(&box_2_3, 0.7, 0.15);

            Binary show_temp_2 = b_a1_2.showBoundaryBox(&box_2_2, 0.7, 0.15);
            Binary show_temp_3 = b_a1_3s.showBoundaryBox(&box_2_3, 0.7, 0.15);

            image = this->fromBinary(show_temp_2);

            imagePreview.setImage(image);
            imagePreview.exec();

            image = this->fromBinary(show_temp_3);

            imagePreview.setImage(image);
            imagePreview.exec();

            qInfo(("size vec mon 2 : " + to_string(vecMon2_2.size())).c_str());

            string list_char[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

            vector<Binary*> vecChar;

            Resolution resfinal;
            resfinal.width = 50;
            resfinal.height = 100;

            for (int i = 0; i < 36; i++) {
                string fname = "D:/ImageSample/char_bmp/" + list_char[i] + ".bmp";
                qInfo(fname.c_str());
                Truecolor chartr(fname, ImageFormat::BMP, ImageType::TRUECOLOR);
                Grayscale chargr = chartr.toGrayscale();

                Binary charbi = chargr.binarySegmentation(80);

                Binary charre = charbi.resizePixels(resfinal);

                QImage image2 = this->fromBinary(charre);

                vecChar.push_back(new Binary(charre));
            }

            string plat = "";
            string plat_2 = "";
            string plat_3 = "";

            for (int i = 0; i < vecMon2.size(); i++) {
                qInfo(("index : " + to_string(i)).c_str());
                Binary b_conv = vecMon2[i]->resizePixels(resfinal);
                int min_error = -1;
                int min_idx = -1;
                for (int j = 0; j < 36; j++) {
                    int curr_error = 0;
                    qInfo(("index j : " + to_string(j)).c_str());
                    for (int k = 0; k < resfinal.height; k++) {
                        for (int l = 0; l < resfinal.width; l++) {
//                            qInfo(("index k l : " + to_string(k) + " " + to_string(l)).c_str());
//                            qInfo(to_string(b_conv.getIndividualPixel(k, l)).c_str());
//                            qInfo(to_string(vecChar[j]->getIndividualPixel(k, l)).c_str());
                            if (b_conv.getIndividualPixel(k, l) != vecChar[j]->getIndividualPixel(k, l)) {
                                curr_error++;
                            }
                        }
                    }
                    if (min_error == -1) {
                        min_error = curr_error;
                        min_idx = j;
                    }

                    qInfo(("index j : " + to_string(curr_error)).c_str());
                    if (curr_error < min_error) {
                        min_error = curr_error;
                        min_idx = j;
                    }
                }
                if (min_error < 2500) {
                    plat += list_char[min_idx];
                }
            }

            for (int i = 0; i < vecMon2_2.size(); i++) {
                qInfo(("index : " + to_string(i)).c_str());
                Binary b_conv = vecMon2_2[i]->resizePixels(resfinal);
                int min_error = -1;
                int min_idx = -1;
                for (int j = 0; j < 36; j++) {
                    int curr_error = 0;
                    for (int k = 0; k < resfinal.height; k++) {
                        for (int l = 0; l < resfinal.width; l++) {
//                            qInfo(("index k l : " + to_string(k) + " " + to_string(l)).c_str());
//                            qInfo(to_string(b_conv.getIndividualPixel(k, l)).c_str());
//                            qInfo(to_string(vecChar[j]->getIndividualPixel(k, l)).c_str());
                            if (b_conv.getIndividualPixel(k, l) != vecChar[j]->getIndividualPixel(k, l)) {
                                curr_error++;
                            }
                        }
                    }
                    if (min_error == -1) {
                        min_error = curr_error;
                        min_idx = j;
                    }

                    if (curr_error < min_error) {
                        min_error = curr_error;
                        min_idx = j;
                    }
                }
                if (min_error < 2500) {
                    plat_2 += list_char[min_idx];
                }
            }

            for (int i = 0; i < vecMon2_3.size(); i++) {
                qInfo(("index : " + to_string(i)).c_str());
                Binary b_conv = vecMon2_3[i]->resizePixels(resfinal);
                int min_error = -1;
                int min_idx = -1;
                for (int j = 0; j < 36; j++) {
                    int curr_error = 0;
                    for (int k = 0; k < resfinal.height; k++) {
                        for (int l = 0; l < resfinal.width; l++) {
//                            qInfo(("index k l : " + to_string(k) + " " + to_string(l)).c_str());
//                            qInfo(to_string(b_conv.getIndividualPixel(k, l)).c_str());
//                            qInfo(to_string(vecChar[j]->getIndividualPixel(k, l)).c_str());
                            if (b_conv.getIndividualPixel(k, l) != vecChar[j]->getIndividualPixel(k, l)) {
                                curr_error++;
                            }
                        }
                    }
                    if (min_error == -1) {
                        min_error = curr_error;
                        min_idx = j;
                    }

                    if (curr_error < min_error) {
                        min_error = curr_error;
                        min_idx = j;
                    }
                }
                if (min_error < 2500) {
                    plat_3 += list_char[min_idx];
                }
            }

            qInfo(("first plate : " + plat).c_str());
            qInfo(("second plate : " + plat_2).c_str());
            qInfo(("second plate : " + plat_3).c_str());

            string final_plat;
            if (plat_2.length() > plat.length()) {
                if (plat_3.length() > plat_2.length()) {
                    final_plat = plat_3;
                } else {
                    final_plat = plat_2;
                }
            } else {
                if (plat_3.length() > plat.length()) {
                    final_plat = plat_3;
                } else {
                    final_plat = plat;
                }
            }

            QMessageBox msgBox;
            string messagePlat = "Plat nomor : " + final_plat;
            msgBox.setText(messagePlat.c_str());
            msgBox.exec();

//            if (result == QDialog::Accepted) {
//                truecolors.erase(truecolors.begin() + imageIdx);
//                int newIdx = this->getVectorIdx(idx, ImageType::BINARY);
//                binaries.insert(binaries.begin() + newIdx, new Binary(b5));
//                QImage newImage = fromBinary(b5);
//                tabPage->setImageType(ImageType::BINARY);
//                label->setPixmap(QPixmap::fromImage(newImage));
//            x}
            break;
        }
    }
}

void MainWindow::on_actionResize_triggered()
{
    FormResize form(this);
    form.setWindowTitle("Resize");
    int result = form.exec();

    qInfo("after form");

    if (result == QDialog::Accepted) {
        QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
        int idx = tabWidget->currentIndex();
        TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
        QLabel * label = tabPage->findChild<QLabel*>("label");
        int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

        switch (tabPage->getImageType()) {
            case (ImageType::BINARY) : {
                Binary * b = binaries.at(imageIdx);

                Resolution res;
                res.height = form.getHeight();
                res.width = form.getWidth();

                Binary b2 = b->resizePixels(res);

                QImage image = this->fromBinary(b2);

                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();

                if (result == QDialog::Accepted) {
                    binaries.at(imageIdx) = new Binary(b2);
                    QImage newImage = fromBinary(b2);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
                break;
            }
        }
    }
}

void MainWindow::on_actionGaussian_Smoothing_triggered()
{
    try {
        float s = inputFloat("Gaussian Smoothing", "Insert sigma");

        QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
        int idx = tabWidget->currentIndex();
        TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
        QLabel * label = tabPage->findChild<QLabel*>("label");
        int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

        switch (tabPage->getImageType()) {
            case (ImageType::GRAYSCALE) : {
                Grayscale * g = grayscales.at(imageIdx);
                Grayscale prev = g->gaussianSmoothing(s);

                QImage image = this->fromGrayscale(prev);

                ImagePreview imagePreview(this);
                imagePreview.setImage(image);
                int result = imagePreview.exec();
                if (result == QDialog::Accepted) {
                    grayscales.at(imageIdx) = new Grayscale(prev);
                    QImage newImage = fromMonochrome(prev);
                    label->setPixmap(QPixmap::fromImage(newImage));
                }
                break;
            }
        }
        //
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionLine_triggered()
{
    QTabWidget* tabWidget = ui->centralwidget->findChild<QTabWidget*>("tabWidget");
    int idx = tabWidget->currentIndex();
    TabPage * tabPage = (TabPage *) tabWidget->widget(idx);
    QLabel * label = tabPage->findChild<QLabel*>("label");

    int imageIdx = this->getVectorIdx(idx, tabPage->getImageType());

    switch (tabPage->getImageType()) {
        case (ImageType::BINARY) : {
            Binary * b = binaries.at(imageIdx);
            Monochrome prev = b->hough(500);

            QImage image = this->fromMonochrome(prev);
            ImagePreview imagePreview(this);
            imagePreview.setImage(image);
            int result = imagePreview.exec();
            if (result == QDialog::Accepted) {
//                grayscales.at(imageIdx) = new Grayscale(prev);
//                QImage newImage = fromMonochrome(prev);
//                label->setPixmap(QPixmap::fromImage(newImage));
            }
            break;
        }
    }
}
