#ifndef _IMAGE_FORMAT_H_
#define _IMAGE_FORMAT_H_

#include <iostream>

using namespace std;

enum ImageFormat {
    PGM,
    PPM,
    PBM,
    BMP,
    RAW_IMAGE,
    NONE
};

ImageFormat convertFromString(string fileFormat);

namespace image_format {
    string toString(ImageFormat imageFormat);
}

#endif