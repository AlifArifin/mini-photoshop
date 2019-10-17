#include "truecolor.h"
#include "image_format.h"
#include "image_type.h"
#include "ppm_state.h"
#include "utils.h"
#include "operation.h"
#include "transformation.h"
#include "geometry.h"
#include <vector>
#include <stdio.h>
#include <QDebug>

using namespace std;

Truecolor::Truecolor(ifstream * file, ImageFormat imageFormat, ImageType imageType) {
    qInfo("create truecolor");
    Resolution res;
    string fileline;
    int p, q;
    vector<string> pixels;

    if (!file->is_open()) {
        throw "Error, cannot open file";
    }

    int debug = 0;

    switch (imageFormat) {
        case (ImageFormat::PPM) : {
            qInfo("create truecolor");
            PPMState ppmState = PPMState::PPM_CODE;
            PPMColorState ppmColorState = PPMColorState::RED;
            while (getline(*file, fileline)) {
                debug++;
                //qInfo(to_string(debug).c_str());
                switch (ppmState) {
                    case (PPMState::PPM_CODE) : {
                        ppmState = PPMState::PPM_SIZE;
                        break;
                    }
                    case (PPMState::PPM_SIZE) : {
                        if (!isComment(fileline)) {
                            vector<string> words = Utils::split(fileline, ' ');

                            if (words.size() == 2) {
                                res.width = stoi(words[0]);
                                res.height = stoi(words[1]);
                            }
                            this->resolution = res;

                            this->pixel = new RGBA*[res.height];
                            for (int i = 0; i < res.height; i++) {
                                this->pixel[i] = new RGBA[res.width];
                            }

                            // init
                            p = 0;
                            q = 0;

                            ppmState = PPMState::PPM_LEVEL;
                        }
                        break;
                    }
                    case (PPMState::PPM_LEVEL) : {
                        qInfo("create truecolor");
                        this->level = stoi(fileline);
                        ppmState = PPMState::PPM_PIXEL;
                        break;
                    }
                    case (PPMState::PPM_PIXEL) : {
                        pixels = Utils::split(fileline, ' ');
                        for (size_t i = 0; i < pixels.size(); i++) {
                            if (ppmColorState == PPMColorState::RED) {
                                this->pixel[p][q].r = stoi(pixels[i]);
                                ppmColorState = PPMColorState::BLUE;
                            } else if (ppmColorState == PPMColorState::BLUE) {
                                this->pixel[p][q].b = stoi(pixels[i]);
                                ppmColorState = PPMColorState::GREEN;
                            } else {
                                this->pixel[p][q].g = stoi(pixels[i]);
                                ppmColorState = PPMColorState::RED;
                                if (q >= res.width - 1) {
                                    qInfo("lala");
                                    p++;
                                    q = 0;
                                } else {
                                    q++;
                                }
                            }
                        }
                        break;
                    }
                    default :
                        break;
                }
            }
            break;
        }
    }
}

Truecolor(Resolution resolution, ImageFormat imageFormat, ImageType ImageType, int level) : Image(imageFormat, imageType) {
    this->level = level;
    this->resolution = resolution;
    this->pixel = new RGBA*[resolution.height];
    for (int i = 0; i < resolution.height; i++) {
        this->pixel[i] = new RGBA[resolution.width];
        for (int j = 0; j < resolution.width; j++) {
            this->pixel[i][j].r = 0;
            this->pixel[i][j].g = 0;
            this->pixel[i][j].b = 0;
            this->pixel[i][j].a = 0;
        }
    }
}

Truecolor::Truecolor(const Truecolor & truecolor) {
    this->level = truecolor.level;
    this->pixel = new RGBA*[this->resolution.height];
    for (int i = 0; i < this->resolution.height; i++) {
        this->pixel[i] = new RGBA[this->resolution.width];
        for (int j = 0; j < this->resolution.width; j++) {
            this->pixel[i][j] = truecolor.pixel[i][j];
        }
    }
}

RGBA** Truecolor::getPixel() {
    return this->pixel;
}

void Truecolor::setPixel(RGBA** pixel) {
    this->pixel = pixel;
}

bool Truecolor::getAlpha() {
    return this->alpha;
}

void Truecolor::setAlpha(bool alpha) {
    this->alpha = alpha;
}

RGBA Truecolor::getIndividualPixel(int i, int j) {
    return this->pixel[i][j];
}

int Truecolor::getLevel() {
    return this->level;
}

void Truecolor::setLevel(int level) {
    this->level = level;
}

void Truecolor::save(string filename) {

}

Truecolor Truecolor::negative() {
    Truecolor mNew(
        this->imageFormat,
        this->imageType,
        this->resolution,
        this->level
    );

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            mNew.pixel[i][j].r = this->level - this->pixel[i][j].r;
            mNew.pixel[i][j].g = this->level - this->pixel[i][j].g;
            mNew.pixel[i][j].b = this->level - this->pixel[i][j].b;
        }
    }

    return mNew;
}

Truecolor Truecolor::brightening(float b, Operation o) {
    Truecolor mNew(
        this->imageFormat,
        this->imageType,
        this->resolution, 
        this->level
    );

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            RGBA temp;

            switch (o) {
                case (Operation::ADD) : {
                    temp.r = (short) (this->pixel[i][j].r + b);
                    temp.g = (short) (this->pixel[i][j].g + b);
                    temp.b = (short) (this->pixel[i][j].b + b);
                    break;
                }
                case (Operation::SUBTRACT) : {
                    temp.r = (short) (this->pixel[i][j].r - b);
                    temp.g = (short) (this->pixel[i][j].g - b);
                    temp.b = (short) (this->pixel[i][j].b - b);
                    break;
                }
                case (Operation::MULTIPLY) : {
                    temp.r = (short) (this->pixel[i][j].r * b);
                    temp.g = (short) (this->pixel[i][j].g * b);
                    temp.b = (short) (this->pixel[i][j].b * b);
                    break;
                }
                case (Operation::DIVISION) : {
                    temp.r = (short) ((float) this->pixel[i][j].r / b);
                    temp.g = (short) ((float) this->pixel[i][j].g / b);
                    temp.b = (short) ((float) this->pixel[i][j].b / b);
                    break;
                }
            }

            mNew.pixel[i][j].r = Image::clipping(temp.r, level);
            mNew.pixel[i][j].g = Image::clipping(temp.g, level);
            mNew.pixel[i][j].b = Image::clipping(temp.b, level);
        }
    }

    return mNew;
}

Truecolor Truecolor::operation(Truecolor *m, Operation o, short level) {
    // check if same
    if (!Image::sameResolution(this->resolution, m->resolution)) {
        throw "Images have different resolution";
    }

    Truecolor mNew(
        ImageFormat::NONE, 
        ImageType::TRUECOLOR,
        this->resolution, 
        level
    );
    
    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            RGBA temp;
            switch (o) {
                case (Operation::ADD) : {
                    temp.r = this->pixel[i][j].r + m->pixel[i][j].r;
                    temp.g = this->pixel[i][j].g + m->pixel[i][j].g;
                    temp.b = this->pixel[i][j].b + m->pixel[i][j].b;
                    break;
                }
                case (Operation::SUBTRACT) : {
                    temp.r = this->pixel[i][j].r - m->pixel[i][j].r;
                    temp.g = this->pixel[i][j].g - m->pixel[i][j].g;
                    temp.b = this->pixel[i][j].b - m->pixel[i][j].b;
                    break;
                }
                case (Operation::MULTIPLY) : {
                    temp.r = this->pixel[i][j].r * m->pixel[i][j].r;
                    temp.g = this->pixel[i][j].g * m->pixel[i][j].g;
                    temp.b = this->pixel[i][j].b * m->pixel[i][j].b;
                    break;
                }
                case (Operation::AND) : {
                    temp.r = this->pixel[i][j].r & m->pixel[i][j].r;
                    temp.g = this->pixel[i][j].g & m->pixel[i][j].g;
                    temp.b = this->pixel[i][j].b & m->pixel[i][j].b;
                    break;
                }
                case (Operation::OR) : {
                    temp.r = this->pixel[i][j].r | m->pixel[i][j].r;
                    temp.g = this->pixel[i][j].g | m->pixel[i][j].g;
                    temp.b = this->pixel[i][j].b | m->pixel[i][j].b;
                    break;
                }
                case (Operation::ADD_ABS) : {
                    temp.r = abs(this->pixel[i][j].r) + abs(m->pixel[i][j].r);
                    temp.g = abs(this->pixel[i][j].g) + abs(m->pixel[i][j].g);
                    temp.b = abs(this->pixel[i][j].b) + abs(m->pixel[i][j].b);
                    break;
                }
                case (Operation::DIVISION) : {
                    temp.r = (short) (float) this->pixel[i][j].r / m->pixel[i][j].r;
                    temp.g = (short) (float) this->pixel[i][j].g / m->pixel[i][j].g;
                    temp.b = (short) (float) this->pixel[i][j].b / m->pixel[i][j].b;
                    break;
                }
                default : {
                    break;
                }
            }
            mNew.pixel[i][j].r = Image::clipping(temp.r, level);
            mNew.pixel[i][j].g = Image::clipping(temp.g, level);
            mNew.pixel[i][j].b = Image::clipping(temp.b, level);
        }
    }

    return mNew;
}

Truecolor Truecolor::translastion(int m, int n) {
    Truecolor mNew(
        this->imageFormat,
        this->imageType,
        this->resolution, 
        this->level
    );

    int tempY = -n;
    for (int i = 0; i < resolution.height; i++) {
        int tempX = m;
        for (int j = 0; j < resolution.width; j++) {
            if (tempX >= 0 && tempX < resolution.width && tempY >= 0 && tempY < resolution.height) {
                mNew.pixel[tempY][tempX] = this->pixel[i][j];
            }
            tempX++;
        }
        tempY++;
    }

    return mNew;
}

Truecolor geometry(Geometry degree) {
    Resolution newResolution;

    switch (geo) {
        case (Geometry::ROTATION_90) :
        case (Geometry::ROTATION_270) :
        case (Geometry::MIRROR_X_Y) : {
            newResolution.height = this->resolution.width;
            newResolution.width = this->resolution.height;
            break;
        }
        default : {
            newResolution.height = this->resolution.height;
            newResolution.width = this->resolution.width;
            break;
        }
    }

    Truecolor mNew(
        this->imageFormat,
        this->imageType,
        newResolution, 
        this->level
    );

    int p = resolution.height - 1;
    for (int i = 0; i < resolution.height; i++, p--) {
        int q = resolution.width - 1;
        for (int j = 0; j < resolution.width; j++, q--) {
            switch (geo) {
                case (Geometry::ROTATION_0) : {
                    mNew.pixel[i][j] = this->pixel[i][j];
                    break;
                }
                case (Geometry::ROTATION_90) : {
                    mNew.pixel[q][i] = this->pixel[i][j];
                    break;
                }
                case (Geometry::ROTATION_180) : {
                    mNew.pixel[p][q] = this->pixel[i][j];
                    break;
                }
                case (Geometry::ROTATION_270) : {
                    mNew.pixel[q][p] = this->pixel[i][j];
                    break;
                }
                case (Geometry::FLIPPING_HORIZONTAL) : {
                    mNew.pixel[i][q] = this->pixel[i][j];
                    break;
                }
                case (Geometry::FLIPPING_VERTICAL) : {
                    mNew.pixel[p][j] = this->pixel[i][j];
                    break;
                }
                case (Geometry::MIRROR_CARTESIAN) : {
                    mNew.pixel[p][q] = this->pixel[i][j];
                    break;
                }
                case (Geometry::MIRROR_X_Y) : {
                    mNew.pixel[j][i] = this->pixel[i][j];
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    return mNew;
}

Histogram generateHistogram(PPMColorState color) {
    Histogram histogram;
    histogram.size = this->level + 1;
    histogram.value = new int[histogram.size];
    histogram.norm = new float[histogram.size];

    long totalPixel = this->resolution.height * this->resolution.width;

    for (int i = 0; i < histogram.size; i++) {
        histogram.value[i] = 0;
    }

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            if (color == PPMColorState::RED) {
                histogram.value[this->pixel[i][j].r] += 1;
            } else if (color == PPMColorState::GREEN) {
                histogram.value[this->pixel[i][j].g] += 1;
            } else if (color == PPMColorState::BLUE) {
                histogram.value[this->pixel[i][j].b] += 1;
            }
        }
    }

    int temp = 0;
    long sum = 0;
    for (int i = 0; i < histogram.size; i++) {
        temp = histogram.value[i] > temp ? histogram.value[i] : temp;
        sum += histogram.value[i] * i;
        histogram.norm[i] = (float) histogram.value[i] / totalPixel;
    }

    histogram.max = temp;
    histogram.mean = (float) sum / totalPixel;

    double var = 0;
    for (int i = 0; i < histogram.size; i++) {
        var += pow(i - histogram.mean, 2) * histogram.value[i];
    }

    histogram.var = var;
    histogram.std = pow(var, 0.5);

    return histogram;
}

Truecolor Truecolor::convolution(Convolution c, Padding pad, int size, float** kernel) {
    qInfo("convol");
    Monochrome mNew(
        ImageFormat::NONE,
        this->imageType,
        this->resolution, 
        this->level
    );

    if (size % 2 != 1) {
        throw "kernel must be odd";
    }

    int offset = (size - 1) / 2;
    int length = size * size;
    short * arrayr = new short[length];
    short * arrayg = new short[length];
    short * arrayb = new short[length];

    qInfo("convol");

    for (int i = 0; i < this->resolution.height; i++) {
        qInfo("convol");
        for (int j = 0; j < this->resolution.width; j++) {
            if (i >= offset && j >= offset && i < this->resolution.height - offset && j < this->resolution.width - offset) {
                RGBAFloat temp;
                temp.r = 0;
                temp.g = 0;
                temp.b = 0;
                int m = 0;
                for (int p = i - offset; m < size; p++, m++) {
                    int n = 0;
                    for (int q = j - offset; n < size; q++, n++) {
                        switch (c) {
                            case (Convolution::BASIC) : {
                                temp.r += this->pixel[p][q].r * kernel[m][n];
                                temp.g += this->pixel[p][q].g * kernel[m][n];
                                temp.b += this->pixel[p][q].b * kernel[m][n];
                                break;
                            }
                            case (Convolution::MEDIAN) : {
                                arrayr[m * size + n] = this->pixel[p][q].r;
                                arrayg[m * size + n] = this->pixel[p][q].g;
                                arrayb[m * size + n] = this->pixel[p][q].b;
                                break;
                            }
                        }
                    }
                }

                switch (c) {
                    case (Convolution::NO_CLIPPING) : {
                        mNew.pixel[i][j].r = (short) temp.r;
                        mNew.pixel[i][j].g = (short) temp.g;
                        mNew.pixel[i][j].b = (short) temp.b;
                        break;
                    }
                    case (Convolution::BASIC) : {
                        mNew.pixel[i][j].r = Image::clipping((short) temp.r, this->level);
                        mNew.pixel[i][j].g = Image::clipping((short) temp.g, this->level);
                        mNew.pixel[i][j].b = Image::clipping((short) temp.b, this->level);
                        break;
                    }
                    case (Convolution::MEDIAN) : {
                        sort(arrayr, arrayr + length);
                        sort(arrayg, arrayg + length);
                        sort(arrayb, arrayb + length);
                        mNew.pixel[i][j].r = arrayr[((length - 1)/2)];
                        mNew.pixel[i][j].g = arrayg[((length - 1)/2)];
                        mNew.pixel[i][j].b = arrayb[((length - 1)/2)];
                        break;
                    }
                }
            } else {
                switch (pad) {
                    case (Padding::SAME) : {
                        mNew.pixel[i][j] = this->pixel[i][j];
                        break;
                    }
                }
            }
        }
    }

    return mNew;
}