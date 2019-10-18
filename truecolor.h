#ifndef _TRUECOLOR_H_
#define _TRUECOLOR_H_

#include "image.h"
#include "rgba.h"
#include "convolution.h"
#include "transformation.h"
#include "operation.h"
#include "ppm_state.h"
#include "geometry.h"
#include "padding.h"

class Truecolor : public Image {
private:
    RGBA **pixel;
    bool alpha;
    int level;

public:
    Truecolor(ifstream * file, ImageFormat imageFormat, ImageType ImageType);
    Truecolor(ImageFormat imageFormat, ImageType ImageType, Resolution resolution, int level);
    Truecolor(ImageFormat imageFormat, Resolution resolution, int level);
    Truecolor(const Truecolor & truecolor);

    // getter setter
    RGBA** getPixel();
    void setPixel(RGBA** pixel);
    bool getAlpha();
    void setAlpha(bool alpha);
    RGBA getIndividualPixel(int i, int j);
    int getLevel();
    void setLevel(int level);
    void save (string filename);

    // image processing
    Truecolor negative();
    Truecolor brightening(float b, Operation o = Operation::ADD);
    Truecolor operation(Truecolor * m, Operation o, short level = 255);
    Truecolor translastion(int m, int n);
    Truecolor geometry(Geometry degree);
    Histogram generateHistogram(PPMColorState color);
    Truecolor convolution(Convolution c, Padding pad, int size, float** kernel);
    Truecolor sharpening(Truecolor * lowPass, float alpha = 1);
    Truecolor histogramLeveling();
    Truecolor histogramSpecification(Histogram hr,Histogram hg, Histogram hb);
    Truecolor zoom(bool in);
    Truecolor contrastStretching(int a, int b, int ya, int yb, float alpha = 1, float beta = 1, float gamma = 1);
};

#endif
