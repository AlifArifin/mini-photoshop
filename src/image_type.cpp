#include "image_type.h"

unsigned int getPixelSizeFromImageType(ImageType imageType) {
    switch (imageType) {
        case ImageType::BINARY :
            return 1;
        case ImageType::GRAYSCALE :
            return 8;
        case ImageType::TRUECOLOR :
            return 24;
        default :
            return 0;
    }
}

string image_type::toString(ImageType imageType) {
    switch (imageType) {
        case ImageType::BINARY :
            return "BINARY";
        case ImageType::GRAYSCALE :
            return "GRAYSCALE";
        case ImageType::TRUECOLOR :
            return "TRUECOLOR";
        default :
            return "NONE";
    }
}
