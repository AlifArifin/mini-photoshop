#ifndef _GRAYSCALE_H_
#define _GRAYSCALE_H_

#include "monochrome.h"
#include "image_format.h"
#include "image_type.h"
#include "transformation.h"
#include "binary.h"
#include <vector>

class Grayscale : public Monochrome {
public:
    Grayscale(ifstream * file, ImageFormat imageFormat, ImageType imageType);
    Grayscale(ImageFormat imageFormat, Resolution resolution, int level);
    Grayscale(const Grayscale & grayscale);
    Grayscale(const Monochrome & monochrome);
    Grayscale contrastStretching(int a, int b, int ya, int yb, float alpha = 1, float beta = 1, float gamma = 1);
    Grayscale transformation(Transformation t, float c, float gamma = 1);
    Grayscale slicing(int a, int b, int max = 255);
    vector<Binary*> bitSlicing();
};

#endif
