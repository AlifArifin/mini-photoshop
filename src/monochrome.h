#ifndef _MONOCHROME_H_
#define _MONOCHROME_H_

#include "image.h"
#include "image_type.h"
#include "image_format.h"
#include "operation.h"
#include "geometry.h"
#include "histogram.h"
#include "convolution.h"
#include "edge_detection.h"
#include "padding.h"
#include <vector>

class Monochrome : public Image {
protected:
    short **pixel;
    short level;

public:
    Monochrome();
    Monochrome(ImageFormat imageFormat, ImageType imageType);
    Monochrome(ImageFormat imageFormat, ImageType imageType, Resolution resolution, short level);
    Monochrome(const Monochrome & monochrome);
    Monochrome(const Monochrome & monochrome, Resolution resTop, Resolution resBot);
    ~Monochrome();

    // getter setter
    short** getPixel();
    void setPixel(short** rgb);
    short getLevel();
    void setLevel(short level);
    string toString();
    void setIndividualPixel(int i, int j, short pixel);
    short getIndividualPixel(int i, int j);

    void save(string filename);

    static float ** initPixel(int h, int w);

    // image processing
    Monochrome negative();
    Monochrome brightening(float b, Operation o = Operation::ADD);
    Monochrome operation(Monochrome * m, Operation o, short level = 255);
    Monochrome translastion(int m, int n);
    Monochrome geometry(Geometry degree);
    Histogram generateHistogram();
    Monochrome convolution(Convolution c, Padding pad, int size, float** kernel);
    Monochrome convolutionTopLeft(Convolution c, int sizeX, int sizeY, float** kernel);
    Monochrome sharpening(Monochrome * lowPass, float alpha = 1);
    Monochrome histogramLeveling();
    Monochrome histogramSpecification(Histogram h);
    Monochrome zoom(bool in);
    vector<Monochrome*> boundaryBox(Monochrome * m, Monochrome * real, float rat_top, float rat_bot);
    Monochrome showBoundaryBox(Monochrome * m, float rat_top, float rat_bot);
    vector<Monochrome*> boundaryBoxPlate(Monochrome * m, float rat_top, float rat_bot);
    int boundaryBoxCount(Monochrome * m, float rat_top, float rat_bot);
    Monochrome resizePixels(Resolution res);
    Monochrome hough(int threshold);
};

#endif
