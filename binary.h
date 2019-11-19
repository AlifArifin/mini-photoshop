#ifndef _BINARY_H_
#define _BINARY_H_

#include "monochrome.h"
#include "image_format.h"
#include "image_type.h"

class Binary : public Monochrome {
public:
    Binary();
    Binary(ImageFormat imageFormat, Resolution resolution);
    Binary(ifstream * file, ImageFormat imageFormat, ImageType imageType);
    Binary(const Binary & binary);
    Binary(const Monochrome & monochrome);

    // image processing
    Binary Not();
    Binary thinning();
};

#endif
