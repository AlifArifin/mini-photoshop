#include "truecolor.h"
#include "image_format.h"
#include "image_type.h"
#include "ppm_state.h"
#include "utils.h"
#include "operation.h"
#include "transformation.h"
#include "geometry.h"
#include "bmp.h"
#include <vector>
#include <stdio.h>
#include <QDebug>

using namespace std;

Truecolor::Truecolor(string filename, ImageFormat imageFormat, ImageType ImageType) : Image(imageFormat, imageType) {
    struct BMP bmp(filename.c_str());

    this->resolution.width = bmp.bmp_info_header.width;
    this->resolution.height = bmp.bmp_info_header.height;
    this->level = 255;

    int counter = 0;
    PPMColorState state = PPMColorState::RED;
    this->pixel = new RGBA*[this->resolution.height];
    for (int i = 0; i < this->resolution.height; i++) {
        this->pixel[i] = new RGBA[this->resolution.width];
        for (int j = 0; j < this->resolution.width; j++) {
            this->pixel[i][j].r = (short) bmp.data.at(counter++);
            this->pixel[i][j].g = (short) bmp.data.at(counter++);
            this->pixel[i][j].b = (short) bmp.data.at(counter++);
        }
    }
}

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
                        qInfo(to_string(pixels.size()).c_str());
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

Truecolor::Truecolor(ImageFormat imageFormat, ImageType ImageType, Resolution resolution, int level) : Image(imageFormat, imageType) {
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

Truecolor::Truecolor(const Truecolor & truecolor) : Image(truecolor) {
    this->level = truecolor.level;
    this->pixel = new RGBA*[this->resolution.height];
    for (int i = 0; i < this->resolution.height; i++) {
        this->pixel[i] = new RGBA[this->resolution.width];
        for (int j = 0; j < this->resolution.width; j++) {
            this->pixel[i][j] = truecolor.pixel[i][j];
        }
    }
}

Truecolor::Truecolor(ImageFormat imageFormat, Resolution resolution, int level) : Image(imageFormat, ImageType::TRUECOLOR) {
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

Truecolor Truecolor::geometry(Geometry geo) {
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
                    mNew.pixel[q][p] = this->pixel[i][j];
                    break;
                }
                case (Geometry::ROTATION_180) : {
                    mNew.pixel[p][q] = this->pixel[i][j];
                    break;
                }
                case (Geometry::ROTATION_270) : {
                    mNew.pixel[q][i] = this->pixel[i][j];
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

Histogram Truecolor::generateHistogram(PPMColorState color) {
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
    Truecolor mNew(
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

Truecolor Truecolor::sharpening(Truecolor * lowPass, float alpha) {
    Truecolor highPass = this->operation(lowPass, Operation::SUBTRACT, this->level);
    Truecolor sharp = (this->brightening(alpha - 1, Operation::MULTIPLY)).operation(&highPass, Operation::ADD, this->level);

    return sharp;
}

Truecolor Truecolor::histogramLeveling() {
    Histogram hr = this->generateHistogram(PPMColorState::RED);
    Histogram hg = this->generateHistogram(PPMColorState::GREEN);
    Histogram hb = this->generateHistogram(PPMColorState::BLUE);
    Mapping mr = Image::histogramStretching(hr);
    Mapping mg = Image::histogramStretching(hg);
    Mapping mb = Image::histogramStretching(hb);

    Truecolor mNew(
        this->imageFormat,
        this->imageType,
        this->resolution, 
        this->level
    );

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            mNew.pixel[i][j].r = mr.value[this->pixel[i][j].r];
            mNew.pixel[i][j].g = mg.value[this->pixel[i][j].g];
            mNew.pixel[i][j].b = mb.value[this->pixel[i][j].b];
        }
    }

    return mNew;
}

Truecolor Truecolor::histogramSpecification(Histogram hr, Histogram hg, Histogram hb) {
    Histogram h2r = this->generateHistogram(PPMColorState::RED);
    Histogram h2g = this->generateHistogram(PPMColorState::GREEN);
    Histogram h2b = this->generateHistogram(PPMColorState::BLUE);
    
    if (h2r.size != hr.size) {
        throw "Have different size";
    }

    Mapping m1r = Image::histogramStretching(hr);
    Mapping m1g = Image::histogramStretching(hg);
    Mapping m1b = Image::histogramStretching(hb);
    Mapping m2r = Image::histogramStretching(h2r);
    Mapping m2g = Image::histogramStretching(h2g);
    Mapping m2b = Image::histogramStretching(h2b);

    Truecolor mNew(
        this->imageFormat,
        this->imageType,
        this->resolution, 
        this->level
    );

    float * cumulativer = new float[m1r.size];
    float * cumulativeg = new float[m1g.size];
    float * cumulativeb = new float[m1b.size];
    float incr = 0;
    float incg = 0;
    float incb = 0;
    for (int i = 0; i < m1r.size; i++) {
        incr += m1r.real[i];
        cumulativer[i] = incr;
        incg += m1g.real[i];
        cumulativeg[i] = incg;
        incb += m1b.real[i];
        cumulativeb[i] = incb;
    }

    short * newMappingr = new short[m1r.size];
    short * newMappingg = new short[m1g.size];
    short * newMappingb = new short[m1b.size];

    // make new mapping
    for (int i = 0; i < m2r.size; i++) {
        float min_deltar = 0;
        float nearestr = -1;
        float min_deltag = 0;
        float nearestg = -1;
        float min_deltab = 0;
        float nearestb = -1;
        for (int j = 0; j < m1r.size; j++) {
            float tempr = abs(m2r.value[i] - m1r.real[j]);
            float tempg = abs(m2g.value[i] - m1g.real[j]);
            float tempb = abs(m2b.value[i] - m1b.real[j]);
            if (nearestr < 0) {
                min_deltar = tempr;
                nearestr = m1r.real[j];
            } else if (tempr < min_deltar) {
                min_deltar = tempr;
                nearestr = m1r.real[j];
            }
            if (nearestg < 0) {
                min_deltag = tempg;
                nearestg = m1g.real[j];
            } else if (tempg < min_deltag) {
                min_deltag = tempg;
                nearestg = m1g.real[j];
            }
            if (nearestb < 0) {
                min_deltab = tempb;
                nearestb = m1b.real[j];
            } else if (tempb < min_deltab) {
                min_deltab = tempb;
                nearestb = m1b.real[j];
            }
        }
        newMappingr[i] = round(nearestr);
        newMappingg[i] = round(nearestg);
        newMappingb[i] = round(nearestb);
    }

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            mNew.pixel[i][j].r = newMappingr[this->pixel[i][j].r];
            mNew.pixel[i][j].g = newMappingg[this->pixel[i][j].g];
            mNew.pixel[i][j].b = newMappingb[this->pixel[i][j].b];
        }
    }

    return mNew;
}

Truecolor Truecolor::zoom(bool in) {
    if (in) {
        Resolution rNew;
        rNew.height = this->resolution.height * 2;
        rNew.width= this->resolution.width * 2;

        Truecolor mNew(
                    this->imageFormat,
                    this->imageType,
                    rNew,
                    this->level
                );

        for (int i = 0; i < this->resolution.height; i++) {
            for (int j = 0; j < this->resolution.width; j++) {
                mNew.pixel[i * 2][j * 2] = this->pixel[i][j];
                mNew.pixel[i * 2 + 1][j * 2] = this->pixel[i][j];
                mNew.pixel[i * 2][j * 2 + 1] = this->pixel[i][j];
                mNew.pixel[i * 2 + 1][j * 2 + 1] = this->pixel[i][j];
            }
        }

        return mNew;
    } else {
        Resolution rNew;
        rNew.height = this->resolution.height / 2;
        rNew.width= this->resolution.width / 2;

        Truecolor mNew(
                    this->imageFormat,
                    this->imageType,
                    rNew,
                    this->level
                );

        for (int i = 0; i < this->resolution.height - 1; i += 2) {
            for (int j = 0; j < this->resolution.width - 1; j += 2) {
                mNew.pixel[i / 2][j / 2].r = (short) ((
                            this->pixel[i][j].r +
                            this->pixel[i + 1][j].r +
                            this->pixel[i][j + 1].r +
                            this->pixel[i + 1][j + 1].r) / 4.0);

                mNew.pixel[i / 2][j / 2].g = (short) ((
                            this->pixel[i][j].g +
                            this->pixel[i + 1][j].g +
                            this->pixel[i][j + 1].g +
                            this->pixel[i + 1][j + 1].g) / 4.0);

                mNew.pixel[i / 2][j / 2].b = (short) ((
                            this->pixel[i][j].b +
                            this->pixel[i + 1][j].b +
                            this->pixel[i][j + 1].b +
                            this->pixel[i + 1][j + 1].b) / 4.0);
            }
        }

        return mNew;
    }
}

Truecolor Truecolor::contrastStretching(int a, int b, int ya, int yb, float alpha, float beta, float gamma) {
    Truecolor gNew(
        this->imageFormat,
        this->resolution, 
        this->level
    );

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            RGBA temp;
            if (this->pixel[i][j].r < a) {
                temp.r = (int) (alpha * this->pixel[i][j].r);
            } else if (this->pixel[i][j].r >= a && this->pixel[i][j].r < b) {
                temp.r = (int) (beta * (this->pixel[i][j].r - a) + ya);
            } else {
                temp.r = (int) (gamma * (this->pixel[i][j].r - b) + yb);
            }

            if (this->pixel[i][j].g < a) {
                temp.g = (int) (alpha * this->pixel[i][j].g);
            } else if (this->pixel[i][j].r >= a && this->pixel[i][j].g < b) {
                temp.g = (int) (beta * (this->pixel[i][j].r - a) + ya);
            } else {
                temp.g = (int) (gamma * (this->pixel[i][j].r - b) + yb);
            }

            if (this->pixel[i][j].b < a) {
                temp.b = (int) (alpha * this->pixel[i][j].r);
            } else if (this->pixel[i][j].b >= a && this->pixel[i][j].b < b) {
                temp.b = (int) (beta * (this->pixel[i][j].b - a) + ya);
            } else {
                temp.b = (int) (gamma * (this->pixel[i][j].b - b) + yb);
            }


            gNew.pixel[i][j].r = Image::clipping(temp.r, this->level);
            gNew.pixel[i][j].g = Image::clipping(temp.g, this->level);
            gNew.pixel[i][j].b = Image::clipping(temp.b, this->level);
        }
    }

    return gNew;
}
