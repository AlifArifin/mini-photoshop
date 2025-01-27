#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "binary.h"
#include "grayscale.h"
#include "truecolor.h"
#include "monochrome.h"
#include "operation.h"
#include "transformation.h"
#include <vector>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionNegative_triggered();

    void on_actionAdd_2_triggered();

    void on_actionSubtract_2_triggered();

    void on_actionMultiply_2_triggered();

    void on_actionDivision_2_triggered();

    void on_actionAdd_triggered();

    void on_actionSubtract_triggered();

    void on_actionMultiply_triggered();

    void on_actionDivision_triggered();

    void on_actionVertical_triggered();

    void on_actionCartesian_triggered();

    void on_actionHorizontal_triggered();

    void on_actionx_y_triggered();

    void on_actionNormal_triggered();

    void on_actionContrast_Stretching_triggered();

    void on_actionLog_triggered();

    void on_actionInverse_Log_triggered();

    void on_actionPower_triggered();

    void on_actionGray_Level_Slicing_triggered();

    void on_actionNormalisasi_triggered();

    void on_actionHistogram_Leveling_triggered();

    void on_actionMedian_Filter_triggered();

    void on_action1_triggered();

    void on_action2_triggered();

    void on_action3_triggered();

    void on_action4_triggered();

    void on_action5_triggered();

    void on_action6_triggered();

    void on_actionUnsharp_Masking_triggered();

    void on_actionIn_triggered();

    void on_actionOut_triggered();

    void on_actionTranslation_triggered();

    void on_actionRed_triggered();

    void on_actionGreen_triggered();

    void on_actionBlue_triggered();

    void on_actionRed_2_triggered();

    void on_actionGreen_2_triggered();

    void on_actionBlue_2_triggered();

    void on_actionHighboost_Filter_triggered();

    void on_action90_triggered();

    void on_action180_triggered();

    void on_action270_triggered();

    void on_actionHistogram_Specification_triggered();

    void on_actionAbout_Image_triggered();

    void on_actionAnd_triggered();

    void on_actionOr_triggered();

    void on_actionGrayscale_triggered();

    void on_actionGrayscale_2_triggered();

    void on_actionRed_3_triggered();

    void on_actionGreen_3_triggered();

    void on_actionBlue_3_triggered();

    void on_actionSave_triggered();

    void on_actionGradient_triggered();

    void on_actionLaplace_triggered();

    void on_actionLaplace_of_Gaussian_triggered();

    void on_actionSobel_triggered();

    void on_actionPrewitt_triggered();

    void on_actionRoberts_triggered();

    void on_actionCanny_triggered();

    void on_actionBinary_Segmentation_triggered();

    void on_actionThinning_triggered();

    void on_actionNumber_Plate_Detector_triggered();

    void on_actionResize_triggered();

    void on_actionGaussian_Smoothing_triggered();

    void on_actionLine_triggered();

private:
    Ui::MainWindow *ui;
    QMainWindow * window;
    vector<Binary *> binaries;
    vector<Grayscale *> grayscales;
    vector<Truecolor *> truecolors;

    int getVectorIdx(int current, ImageType imageType);
    QImage fromBinary(Binary binary);
    QImage fromGrayscale(Grayscale grayscale);
    QImage fromTruecolor(Truecolor truecolor);
    QImage fromMonochrome(Monochrome monochrome);
    void brightening(float c, Operation o);
    void operations(int idx, Operation o);
    void geometry(Geometry t);
    void histogram(Histogram h, bool normalized);
    void transformation(Transformation t, float c, float gamma);
    void errorMessage(QString s);
    void convolution(Convolution c, Padding pad, int size, float ** kernel);
    void rotation(Geometry t);
    float inputFloat(QString title, QString question);
    int inputInt(QString title, QString question);
    QString inputComboBox(QString title, QString question, QStringList list);
    string inputString(QString title, QString question);
    void bitPlane(PPMColorState color = PPMColorState::RED);
    void edgeDetection(EdgeDetection e, int t = 100, int c = 1, float lowT = 0.05, float highT = 0.09, float sigma = 1);
};

#endif // MAINWINDOW_H
