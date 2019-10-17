#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdio.h>
#include <fstream>
#include "image_format.h"
#include "image_type.h"
#include "resolution.h"
#include "bmp_header.h"
#include "mapping.h"
#include "histogram.h"

using namespace std;

class Image {
protected:
    Resolution resolution;
    ImageFormat imageFormat;
    ImageType imageType;
    
public:
    Image();
    Image(ifstream file);
    Image(ImageFormat imageFormat, ImageType imageType);
    Image(Resolution resolution, ImageFormat imageFormat, ImageType imageType);
    Image(const Image & image);

    // getter setter
    Resolution getResolution();
    void setResolution(Resolution Resolution);
    ImageFormat getImageFormat();
    void setImageFormat(ImageFormat imageFormat);
    ImageType getImageType();
    void setImageType(ImageType imageType);

    // image checker
    static ImageType checkFile(string filename);
    static ImageFormat checkFormat(string filename);
    static BMPHeader readBMPHeader(string filename);
    static bool isComment(string line);
    static short clipping(short pixel, short top, short bot = 0);
    static bool sameResolution(Resolution r1, Resolution r2);
    static Mapping histogramStretching(Histogram h);

    virtual void save(string filename) = 0;

    string toString();
};

#endif
