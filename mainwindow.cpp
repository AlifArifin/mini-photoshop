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
#include "bitplane.h"
#include "dialoghistogram.h"
#include "convolution.h"
#include "padding.h"
#include "rgba.h"
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
        Truecolor truecolor(&file, imageFormat, imageType);
        qInfo("after ctor");
        height = truecolor.getResolution().height;
        width = truecolor.getResolution().width;

        QImage image(width, height, QImage::Format_RGB32);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                RGBA value = truecolor.getIndividualPixel(i, j);
                if (truecolor.getLevel() != 255) {
                    value.r = (int) ((float) value.r / truecolor.getLevel() * 255);
                    value.g = (int) ((float) value.g / truecolor.getLevel() * 255);
                    value.b = (int) ((float) value.b / truecolor.getLevel() * 255);
                }

                image.setPixel(j, i, qRgb(value.r, value.g, value.b));
            }
        }
        qInfo("after ctor");
        label->setPixmap(QPixmap::fromImage(image));
        qInfo("after ctor");
        truecolors.push_back(new Truecolor(truecolor));
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

QImage fromTruecolor(Truecolor truecolor) {
    int height = truecolor.getResolution().height;
    int width = truecolor.getResolution().width;

    QImage image(width, height, QImage::Format_Grayscale8);
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
                    Monochrome prev = g2->operation(g, o, 1);

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
        this->brightening(c, Operation::ADD);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionSubtract_triggered()
{
    try {
        int c = inputInt("Operation Subtract", "Insert index");
        this->brightening(c, Operation::SUBTRACT);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionMultiply_triggered()
{
    try {
        int c = inputInt("Operation Multiply", "Insert index");
        this->brightening(c, Operation::MULTIPLY);
    } catch (const char* msg) {
        // do nothing
    }
}

void MainWindow::on_actionDivision_triggered()
{
    try {
        int c = inputInt("Operation Division", "Insert index");
        this->brightening(c, Operation::DIVISION);
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
        axisY->setRange(0, 1);
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
                qInfo("lala");
                Grayscale * g = grayscales.at(imageIdx);
                qInfo("lala");
                int a = form.getA();
                qInfo("getA");
                //qDebug() << form.getA() << form.getB() << form.getYa() << form.getYb();
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
            }
        }
    }
}

void MainWindow::on_actionBit_Slicing_triggered()
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

            qInfo("bitplane");

            vector<Binary*> prev = g->bitSlicing();

            qInfo("bitplane");

            QImage image0 = this->fromBinary(*prev[0]);
            QImage image1 = this->fromBinary(*prev[1]);
            QImage image2 = this->fromBinary(*prev[2]);
            QImage image3 = this->fromBinary(*prev[3]);
            QImage image4 = this->fromBinary(*prev[4]);
            QImage image5 = this->fromBinary(*prev[5]);
            QImage image6 = this->fromBinary(*prev[6]);
            QImage image7 = this->fromBinary(*prev[7]);

            qInfo("bitplane");

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
    }
}
