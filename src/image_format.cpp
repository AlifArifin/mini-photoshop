#include "image_format.h"

#include <string>

using namespace std;

ImageFormat convertFromString(string fileFormat) {
    if (fileFormat == "pgm") {
        return ImageFormat::PGM;
    } else if (fileFormat == "ppm") {
        return ImageFormat::PPM;
    } else if (fileFormat == "pbm") {
        return ImageFormat::PBM;
    } else if (fileFormat == "bmp") {
        return ImageFormat::BMP;
    } else if (fileFormat == "raw_image") {
        return ImageFormat::RAW_IMAGE;
    } else {
        throw "File extension not supported";
    }
}

string image_format::toString(ImageFormat imageFormat) {
    switch (imageFormat) {
        case (ImageFormat::PBM) :
            return "PBM";
        case (ImageFormat::PGM) :
            return "PGM";
        case (ImageFormat::PPM) :
            return "PPM";
        case (ImageFormat::RAW_IMAGE) :
            return "RAW IMAGE";
        case (ImageFormat::BMP) :
            return "BMP";
        default :
            return " ";
    }
}
