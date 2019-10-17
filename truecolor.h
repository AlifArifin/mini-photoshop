#ifndef _TRUECOLOR_H_
#define _TRUECOLOR_H_

#include "image.h"
#include "rgba.h"

class Truecolor : public Image {
private:
    RGBA **pixel;
    bool alpha;
    int level;

public:
    Truecolor(ifstream * file, ImageFormat imageFormat, ImageType ImageType);
    Truecolor(Resolution resolution, ImageFormat imageFormat, ImageType ImageType, int level);
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
};

#endif
