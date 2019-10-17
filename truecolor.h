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
};

#endif
