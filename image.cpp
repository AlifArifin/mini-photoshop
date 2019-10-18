#include "image.h"
#include "image_type.h"

#include <stdio.h>
#include <iostream>
#include <regex>
#include <sstream>
#include <QDebug>

using namespace std;

Image::Image() {

}

Image::Image(ifstream file) {

}

Image::Image(ImageFormat imageFormat, ImageType imageType) {
    this->imageFormat = imageFormat;
    this->imageType = imageType;
}

Image::Image(Resolution resolution, ImageFormat imageFormat, ImageType imageType) {
    this->resolution = resolution;
    this->imageFormat = imageFormat;
    this->imageType = imageType;
}

Image::Image(const Image & image) {
    this->imageType = image.imageType;
    this->imageFormat = image.imageFormat;
    this->resolution = image.resolution;
}

// getter setter
Resolution Image::getResolution() {
    return this->resolution;
}

void Image::setResolution(Resolution Resolution) {
    this->resolution = resolution;
}

ImageFormat Image::getImageFormat() {
    return this->imageFormat;
}

void Image::setImageFormat(ImageFormat imageFormat) {
    this->imageFormat = imageFormat;
}

ImageType Image::getImageType() {
    return this->imageType;
}

void Image::setImageType(ImageType imageType) {
    this->imageType = imageType;
}

ImageType Image::checkFile(string filename) {
    ImageFormat imageFormat = Image::checkFormat(filename);

    switch (imageFormat) {
        case ImageFormat::BMP :
            return ImageType::TRUECOLOR;
        case ImageFormat::PBM :
            return ImageType::BINARY;
        case ImageFormat::PGM :
            return ImageType::GRAYSCALE;
        case ImageFormat::PPM :
            return ImageType::TRUECOLOR;
        case ImageFormat::RAW_IMAGE :
        default :
            throw "File extention not found";
    }
}

ImageFormat Image::checkFormat(string filename) {
    return convertFromString(filename);
}

bool Image::isComment(string line) {
    char first = line.at(0);
    return first == '#';
}

short Image::clipping(short pixel, short top, short bot) {
    if (pixel < bot) {
        return 0;
    } else if (pixel > top) {
        return top;
    }
    return pixel;
}

bool Image::sameResolution(Resolution r1, Resolution r2) {
    return r1.height == r2.height && r1.width == r2.width;
}

Mapping Image::histogramStretching(Histogram h) {
    Mapping m;
    m.size = h.size;
    m.value = new short[m.size];
    m.real = new float[m.size];

    float cumulative = 0;
    for (int i = 0; i < m.size; i++) {
        cumulative += h.norm[i];
        m.real[i] = cumulative * (m.size - 1);
        m.value[i] = (short) m.real[i];
    }

    return m;
}

string Image::toString() {
    stringstream ss;
    ss << "Image\nResolution = " << this->resolution.height << "x" << this->resolution.width << "\nFormat = " << image_format::toString(this->imageFormat) << "\nType = " << image_type::toString(this->imageType) << endl;
    return ss.str();
}
