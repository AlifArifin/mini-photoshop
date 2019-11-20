#ifndef _IMAGE_TYPE_H_
#define _IMAGE_TYPE_H_

#include <string>
#include <iostream>

using namespace std;

enum ImageType {
    BINARY,
    GRAYSCALE,
    TRUECOLOR
};

unsigned int getPixelSizeFromImageType(ImageType imageType);

namespace image_type {
    string toString(ImageType imageType);
}

#endif