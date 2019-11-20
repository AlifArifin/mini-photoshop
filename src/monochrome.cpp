#include "monochrome.h"
#include "utils.h"
#include "pbm_state.h"
#include "pgm_state.h"
#include "operation.h"
#include "padding.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <QDebug>

using namespace std;

Monochrome::Monochrome() {

}

Monochrome::Monochrome(const Monochrome & monochrome) : Image(monochrome) {
    this->level = monochrome.level;
    this->pixel = new short*[this->resolution.height];
    for (int i = 0; i < this->resolution.height; i++) {
        this->pixel[i] = new short[this->resolution.width];
        for (int j = 0; j < this->resolution.width; j++) {
            this->pixel[i][j] = monochrome.pixel[i][j];
        }
    }
}

Monochrome::Monochrome(const Monochrome & monochrome, Resolution resTop, Resolution resBot) : Image(monochrome) {
    Resolution newRes;
    newRes.height = resBot.height - resTop.height + 1;
    newRes.width = resBot.width - resTop.width + 1;

    this->resolution = newRes;

    this->level = monochrome.level;
    this->pixel = new short*[this->resolution.height];
    for (int i = 0; i < this->resolution.height; i++) {
        this->pixel[i] = new short[this->resolution.width];
        for (int j = 0; j < this->resolution.width; j++) {
            this->pixel[i][j] = monochrome.pixel[resTop.height + i][resTop.width + j];
        }
    }
}

Monochrome::Monochrome(ImageFormat imageFormat, ImageType imageType) : Image(imageFormat, imageType) {
}

Monochrome::Monochrome(ImageFormat imageFormat, ImageType imageType, Resolution resolution, short level) : Image(imageFormat, imageType) {
    this->level = level;
    this->resolution = resolution;
    this->pixel = new short*[resolution.height];
    for (int i = 0; i < resolution.height; i++) {
        this->pixel[i] = new short[resolution.width];
        for (int j = 0; j < resolution.width; j++) {
            this->pixel[i][j] = 0;
        }
    }
}

Monochrome::~Monochrome() {
    for (int i = 0; i < this->resolution.height; i++) {
        delete [] pixel[i];
    }
    delete [] pixel;
}

short** Monochrome::getPixel() {
    return this->pixel;
}

void Monochrome::setPixel(short** pixel) {
    this->pixel = pixel;
}

void Monochrome::setIndividualPixel(int i, int j, short pixel) {
    this->pixel[i][j] = pixel;
}

short Monochrome::getIndividualPixel(int i, int j) {
    return this->pixel[i][j];
}

short Monochrome::getLevel() {
    return this->level;
}

void Monochrome::setLevel(short level) {
    this->level = level;
}

void Monochrome::save(string filename) {

    ofstream myfile;
    myfile.open ("D://ImageSample/Saved/" + filename);

    if (this->imageType == ImageType::BINARY) {
        myfile << "P1" << "\n";
        myfile << this->resolution.width << " " << this->resolution.height << "\n";
    } else {
        myfile << "P2" << "\n";
        myfile << this->resolution.width << " " << this->resolution.height << "\n";
        myfile << this->level << "\n";
    }

    int counter = 0;
    for (int i = 0; i < resolution.height; i++) {
        for (int j = 0; j < resolution.width; j++) {
            myfile << this->pixel[i][j] << " ";
            counter++;

            if (counter >= 17) {
                myfile << "\n";
                counter = 0;
            }
        }
    }

    myfile.close();
    qInfo("saved");
}

float ** Monochrome::initPixel(int h, int w) {
    float ** p = new float*[h];
    for (int i = 0; i < h; i++) {
        p[i] = new float[w];
        for (int j = 0; j < w; j++) {
            p[i][j] = 0;
        }
    }

    return p;
}

string Monochrome::toString() {
    string i = Image::toString();
    stringstream ss;
    ss << i << "\nMonochrome\nLevel = " << this->level << "\nPixel" << endl;

    for (int i = 0; i < resolution.height; i++) {
        for (int j = 0; j < resolution.width; j++) {
            ss << pixel[i][j] << " ";
        }
        ss << "\n";
    }

    return ss.str();
}

Monochrome Monochrome::negative() {
    Monochrome mNew(
        this->imageFormat,
        this->imageType,
        this->resolution,
        this->level
    );

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            mNew.pixel[i][j] = this->level - this->pixel[i][j];
        }
    }

    return mNew;
}

Monochrome Monochrome::brightening(float b, Operation o) {
    Monochrome mNew(
        this->imageFormat,
        this->imageType,
        this->resolution, 
        this->level
    );

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            short temp;

            switch (o) {
                case (Operation::ADD) : {
                    temp = (short) (this->pixel[i][j] + b);
                    break;
                }
                case (Operation::SUBTRACT) : {
                    temp = (short) (this->pixel[i][j] - b);
                    break;
                }
                case (Operation::MULTIPLY) : {
                    temp = (short) (this->pixel[i][j] * b);
                    break;
                }
                case (Operation::DIVISION) : {
                    temp = (short) ((float) this->pixel[i][j] / b);
                    break;
                }
            }

            mNew.pixel[i][j] = Image::clipping(temp, level);
        }
    }

    return mNew;
}

Monochrome Monochrome::operation(Monochrome *m, Operation o, short level) {
    // check if same
    if (!Image::sameResolution(this->resolution, m->resolution)) {
        throw "Images have different resolution";
    }

    Monochrome mNew(
        ImageFormat::NONE, 
        level == 1 ? ImageType::BINARY : ImageType::GRAYSCALE, 
        this->resolution, 
        level
    );
    
    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            short temp;
            switch (o) {
                case (Operation::ADD) : {
                    temp = this->pixel[i][j] + m->pixel[i][j];
                    break;
                }
                case (Operation::SUBTRACT) : {
                    temp = this->pixel[i][j] - m->pixel[i][j];
                    break;
                }
                case (Operation::MULTIPLY) : {
                    temp = this->pixel[i][j] * m->pixel[i][j];
                    break;
                }
                case (Operation::AND) : {
                    temp = this->pixel[i][j] & m->pixel[i][j];
                    break;
                }
                case (Operation::OR) : {
                    temp = this->pixel[i][j] | m->pixel[i][j];
                    break;
                }
                case (Operation::ADD_ABS) : {
                    temp = abs(this->pixel[i][j]) + abs(m->pixel[i][j]);
                    break;
                }
                case (Operation::DIVISION) : {
                    temp = (short) (float)this->pixel[i][j] / m->pixel[i][j];
                    break;
                }
                default : {
                    break;
                }
            }
            mNew.pixel[i][j] = Image::clipping(temp, level);
        }
    }

    return mNew;
}

Monochrome Monochrome::translastion(int m, int n) {
    Monochrome mNew(
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

Monochrome Monochrome::geometry(Geometry geo) {
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

    Monochrome mNew(
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

Histogram Monochrome::generateHistogram() {
    Histogram histogram;
    histogram.size = this->level + 1;
    histogram.value = new int[histogram.size];
    histogram.norm = new float[histogram.size];

    long totalPixel = this->resolution.height * this->resolution.width;
    histogram.total = totalPixel;

    for (int i = 0; i < histogram.size; i++) {
        histogram.value[i] = 0;
    }

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            histogram.value[this->pixel[i][j]] += 1;
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

    histogram.var = (float) var/totalPixel;
    histogram.std = pow(histogram.var, 0.5);

    return histogram;
}

Monochrome Monochrome::convolution(Convolution c, Padding pad, int size, float** kernel) {
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
    short * array = new short[length];

    qInfo("convol");

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            if (i >= offset && j >= offset && i < this->resolution.height - offset && j < this->resolution.width - offset) {
                float temp = 0;
                int m = 0;
                for (int p = i - offset; m < size; p++, m++) {
                    int n = 0;
                    for (int q = j - offset; n < size; q++, n++) {
                        switch (c) {
                            case (Convolution::NO_CLIPPING) : 
                            case (Convolution::BASIC) : {
                                temp += this->pixel[p][q] * kernel[m][n];
                                break;
                            }
                            case (Convolution::MEDIAN) : {
                                array[m * size + n] = this->pixel[p][q];
                                break;
                            }
                        }
                    }
                }

                switch (c) {
                    case (Convolution::NO_CLIPPING) : {
                        mNew.pixel[i][j] = (short) temp;
                        break;
                    }
                    case (Convolution::BASIC) : {
                        mNew.pixel[i][j] = Image::clipping((short) temp, this->level);
                        break;
                    }
                    case (Convolution::MEDIAN) : {
                        sort(array, array + length);
                        mNew.pixel[i][j] = array[((length - 1)/2)];
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

    qInfo("convol end");

    return mNew;
}

Monochrome Monochrome::convolutionTopLeft(Convolution c, int sizeX, int sizeY, float** kernel) {
    Monochrome mNew(
        this->imageFormat,
        this->imageType,
        this->resolution, 
        this->level
    );

    for (int i = 0; i < this->resolution.height - sizeX; i++) {
        for (int j = 0; j < this->resolution.width - sizeY; j++) {
            float temp = 0;
            for (int p = 0; p < sizeX; p++) {
                for (int q = 0; q < sizeY; q++) {
                    temp += this->pixel[i + p][j + q] * kernel[p][q];
                }
            }
            switch (c) {
                case (Convolution::NO_CLIPPING) : {
                    mNew.pixel[i][j] = (short) temp;
                    break;
                }
                default : {
                    mNew.pixel[i][j] = Image::clipping((short) temp, this->level);
                    break;
                }
            }
        }
    }

    return mNew;
}

Monochrome Monochrome::sharpening(Monochrome * lowPass, float alpha) {
    Monochrome highPass = this->operation(lowPass, Operation::SUBTRACT, this->level);
    Monochrome sharp = (this->brightening(alpha, Operation::MULTIPLY)).operation(&highPass, Operation::ADD, this->level);

    return sharp;
}

Monochrome Monochrome::histogramLeveling() {
    Histogram h = this->generateHistogram();
    Mapping m = Image::histogramStretching(h);

    Monochrome mNew(
        this->imageFormat,
        this->imageType,
        this->resolution, 
        this->level
    );

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            mNew.pixel[i][j] = m.value[this->pixel[i][j]];
        }
    }

    return mNew;
}

Monochrome Monochrome::histogramSpecification(Histogram h) {
    Histogram h2 = this->generateHistogram();
    
    if (h2.size != h.size) {
        throw "Have different size";
    }

    Mapping m1 = Image::histogramStretching(h);
    Mapping m2 = Image::histogramStretching(h2);

    Monochrome mNew(
        this->imageFormat,
        this->imageType,
        this->resolution, 
        this->level
    );

    float * cumulative = new float[m1.size];
    float inc = 0;
    for (int i = 0; i < m1.size; i++) {
        inc += m1.real[i];
        cumulative[i] = inc;
    }

    short * newMapping = new short[m1.size];

    // make new mapping
    for (int i = 0; i < m2.size; i++) {
        float min_delta = 0;
        float nearest = -1;
        for (int j = 0; j < m1.size; j++) {
            float temp = abs(m2.value[i] - m1.real[j]);
            if (nearest < 0) {
                min_delta = temp;
                nearest = m1.real[j];
            } else if (temp < min_delta) {
                min_delta = temp;
                nearest = m1.real[j];
            }
        }
        newMapping[i] = round(nearest);
    }

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            mNew.pixel[i][j] = newMapping[this->pixel[i][j]];
        }
    }

    return mNew;
}

Monochrome Monochrome::zoom(bool in) {
    if (in) {
        Resolution rNew;
        rNew.height = this->resolution.height * 2;
        rNew.width= this->resolution.width * 2;

        Monochrome mNew(
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

        Monochrome mNew(
                    this->imageFormat,
                    this->imageType,
                    rNew,
                    this->level
                );

        for (int i = 0; i < this->resolution.height - 1; i += 2) {
            for (int j = 0; j < this->resolution.width - 1; j += 2) {
                mNew.pixel[i / 2][j / 2] = (short) ((
                            this->pixel[i][j] +
                            this->pixel[i + 1][j] +
                            this->pixel[i][j + 1] +
                            this->pixel[i + 1][j + 1]) / 4.0);
            }
        }

        return mNew;
    }
}

Monochrome Monochrome::showBoundaryBox(Monochrome * m, float rat_top, float rat_bot) {
    qInfo("boundary box");
    if (!Image::sameResolution(this->resolution, m->resolution)) {
        throw "Images have different resolution";
    }

    Monochrome mNew(*this);

    short ** box = new short*[m->level];
    for (int i = 0; i < m->level; i++) {
        box[i] = new short[4];
        for (int j = 0; j < 4; j++) {
            box[i][j] = -1;
        }
    }

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            if (m->pixel[i][j] != 0) {
                short t = m->pixel[i][j];
                // qInfo(to_string(t).c_str());
                if (box[t][0] == -1) {
                    box[t][0] = i;
                    box[t][1] = j;
                    box[t][2] = i;
                    box[t][3] = j;
                }
                
                if (box[t][0] > i) {
                    box[t][0] = i;
                }

                if (box[t][1] > j) {
                    box[t][1] = j;
                }

                if (box[t][2] < i) {
                    box[t][2] = i;
                }

                if (box[t][3] < j) {
                    box[t][3] = j;
                }
            }
        }
    }

    for (int k = 0; k < m->level; k++) {
        if (box[k][0] != -1) {
            // qInfo((to_string(box[k][0]) + " " + to_string(box[k][2])).c_str());
            float ratio = (float) (box[k][3] - box[k][1]) / (box[k][2] - box[k][0]);
            if ((ratio > rat_bot && ratio < rat_top) || (rat_bot < 0)) {
                for (int i = box[k][0]; i <= box[k][2]; i++) {
                    for (int j = box[k][1]; j <= box[k][3]; j++) {
                        if (i == box[k][0] || i == box[k][2]) {
                            mNew.pixel[i][j] = 1;
                        }
                        if (j == box[k][1] || j == box[k][3]) {
                            mNew.pixel[i][j] = 1;
                        }
                    }
                }
            }
        }
    }

    return mNew;   
}


vector<Monochrome*> Monochrome::boundaryBox(Monochrome * m, Monochrome * real, float rat_top, float rat_bot) {
    qInfo("boundary box");
    if (!Image::sameResolution(this->resolution, m->resolution)) {
        throw "Images have different resolution";
    }

    Monochrome mNew(*this);

    qInfo("boundary box");

    short ** box = new short*[m->level];
    for (int i = 0; i < m->level; i++) {
        box[i] = new short[4];
        for (int j = 0; j < 4; j++) {
            box[i][j] = -1;
        }
    }

    qInfo("boundary box");

    qInfo(to_string(m->level).c_str());

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            if (m->pixel[i][j] != 0) {
                short t = m->pixel[i][j];
                // qInfo(to_string(t).c_str());
                if (box[t][0] == -1) {
                    box[t][0] = i;
                    box[t][1] = j;
                    box[t][2] = i;
                    box[t][3] = j;
                }
                
                if (box[t][0] > i) {
                    box[t][0] = i;
                }

                if (box[t][1] > j) {
                    box[t][1] = j;
                }

                if (box[t][2] < i) {
                    box[t][2] = i;
                }

                if (box[t][3] < j) {
                    box[t][3] = j;
                }
            }
        }
    }

    qInfo("boundary box");

    vector<Monochrome*> vec;

    for (int k = 0; k < m->level; k++) {
        if (box[k][0] != -1) {
            // qInfo((to_string(box[k][0]) + " " + to_string(box[k][2])).c_str());
            float ratio = (float) (box[k][3] - box[k][1]) / (box[k][2] - box[k][0]);
            if ((ratio > rat_bot && ratio < rat_top) || (rat_bot < 0)) {
                // for (int i = box[k][0]; i <= box[k][2]; i++) {
                //     for (int j = box[k][1]; j <= box[k][3]; j++) {
                //         if (i == box[k][0] || i == box[k][2]) {
                //             mNew.pixel[i][j] = 1;
                //         }
                //         if (j == box[k][1] || j == box[k][3]) {
                //             mNew.pixel[i][j] = 1;
                //         }
                //     }
                // }
                Resolution resTop, resBot;
                resTop.height = box[k][0];
                resTop.width = box[k][1];
                resBot.height = box[k][2];
                resBot.width = box[k][3];
                vec.push_back(new Monochrome(*real, resTop, resBot));
            }
        }
    }

    qInfo("boundary box");

    return vec;
}

int Monochrome::boundaryBoxCount(Monochrome * m, float rat_top, float rat_bot) {
    qInfo("boundary box");
    if (!Image::sameResolution(this->resolution, m->resolution)) {
        throw "Images have different resolution";
    }

    int middle = this->resolution.height / 3;

    Monochrome mNew(*this);

    qInfo("boundary box");

    short ** box = new short*[m->level];
    for (int i = 0; i < m->level; i++) {
        box[i] = new short[4];
        for (int j = 0; j < 4; j++) {
            box[i][j] = -1;
        }
    }

    qInfo("boundary box");

    qInfo(to_string(m->level).c_str());

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            if (m->pixel[i][j] != 0) {
                short t = m->pixel[i][j];
                // qInfo(to_string(t).c_str());
                if (box[t][0] == -1) {
                    box[t][0] = i;
                    box[t][1] = j;
                    box[t][2] = i;
                    box[t][3] = j;
                }
                
                if (box[t][0] > i) {
                    box[t][0] = i;
                }

                if (box[t][1] > j) {
                    box[t][1] = j;
                }

                if (box[t][2] < i) {
                    box[t][2] = i;
                }

                if (box[t][3] < j) {
                    box[t][3] = j;
                }
            }
        }
    }

    qInfo("boundary box");

    int count = 0;

    for (int k = 0; k < m->level; k++) {
        if (box[k][0] != -1) {
            float ratio = (float) (box[k][3] - box[k][1]) / (box[k][2] - box[k][0]);
            if (((ratio > rat_bot && ratio < rat_top) || (rat_bot < 0)) && (box[k][0] < middle && box[k][2] > middle)) {
                count++;
            }
        }
    }

    qInfo("boundary box");

    return count;
}

vector<Monochrome*> Monochrome::boundaryBoxPlate(Monochrome * m, float rat_top, float rat_bot) {
    qInfo("boundary box");
    if (!Image::sameResolution(this->resolution, m->resolution)) {
        throw "Images have different resolution";
    }

    int middle = this->resolution.height / 3;

    Monochrome mNew(*this);

    qInfo("boundary box");

    short ** box = new short*[m->level];
    for (int i = 0; i < m->level; i++) {
        box[i] = new short[4];
        for (int j = 0; j < 4; j++) {
            box[i][j] = -1;
        }
    }

    qInfo("boundary box");

    qInfo(to_string(m->level).c_str());

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            if (m->pixel[i][j] != 0) {
                short t = m->pixel[i][j];
                // qInfo(to_string(t).c_str());
                if (box[t][0] == -1) {
                    box[t][0] = i;
                    box[t][1] = j;
                    box[t][2] = i;
                    box[t][3] = j;
                }
                
                if (box[t][0] > i) {
                    box[t][0] = i;
                }

                if (box[t][1] > j) {
                    box[t][1] = j;
                }

                if (box[t][2] < i) {
                    box[t][2] = i;
                }

                if (box[t][3] < j) {
                    box[t][3] = j;
                }
            }
        }
    }

    qInfo("boundary box");
    
    vector<Monochrome*> vec;
    vector<int> res_width;
    vector<int> boun_height;

    for (int k = 0; k < m->level; k++) {
        if (box[k][0] != -1) {
            // qInfo((to_string(box[k][0]) + " " + to_string(box[k][2])).c_str());
            float ratio = (float) (box[k][3] - box[k][1]) / (box[k][2] - box[k][0]);
            if (((ratio > rat_bot && ratio < rat_top) || (rat_bot < 0)) && (box[k][0] < middle && box[k][2] > middle)) {
                // for (int i = box[k][0]; i <= box[k][2]; i++) {
                //     for (int j = box[k][1]; j <= box[k][3]; j++) {
                //         if (i == box[k][0] || i == box[k][2]) {
                //             mNew.pixel[i][j] = 1;
                //         }
                //         if (j == box[k][1] || j == box[k][3]) {
                //             mNew.pixel[i][j] = 1;
                //         }
                //     }
                // }
                Resolution resTop, resBot;
                resTop.height = box[k][0];
                resTop.width = box[k][1];
                resBot.height = box[k][2];
                resBot.width = box[k][3];

                vec.push_back(new Monochrome(*this, resTop, resBot));
                res_width.push_back(resTop.width);
                boun_height.push_back(resBot.height - resTop.height);
            }
        }
    }

    qInfo("boundary box");

    vector<int> deletedIndex;

    int boun_size = boun_height.size() - 1;

    if (boun_size > 0) {
        for (int i = boun_height.size() - 1; i >= 0; i--) {
            int count = 0;
            for (int j = 0; j < boun_height.size(); j++) {
                count += abs (boun_height[i] - boun_height[j]);
            }

            qInfo(to_string(count / boun_size).c_str());
            qInfo(to_string(this->resolution.height / 5).c_str());

            if (count / boun_size > (this->resolution.height / 5)) {
                deletedIndex.push_back(i);
            }
        }
    }

    qInfo("boundary box");

    for (int i = 0; i < deletedIndex.size(); i++) {
        vec.erase(vec.begin() + deletedIndex[i]);
        res_width.erase(res_width.begin() + deletedIndex[i]);
    }

    

    qInfo("boundary box");

    vector<Monochrome*> vec2;
    
    while (vec.size() > 0) {
        int min_x = -1;
        int min_idx = -1;
        for (int i = 0; i < vec.size(); i++) {
            if (i == 0) {
                min_x = res_width[i];
                min_idx = i;
            }

            if (min_x > res_width[i]) {
                min_x = res_width[i];
                min_idx = i;
            }
        }

        vec2.push_back(vec[min_idx]);
        vec.erase(vec.begin() + min_idx);
        res_width.erase(res_width.begin() + min_idx);
    }

    qInfo("boundary box");

    return vec2;
}

Monochrome Monochrome::resizePixels(Resolution res) {
    Monochrome mNew(
        this->imageFormat,
        this->imageType,
        res,
        this->level
    );

    // EDIT: added +1 to account for an early rounding problem
    int x_ratio = (int)((this->resolution.width << 16) / res.width) + 1;
    int y_ratio = (int)((this->resolution.height << 16) / res.height) + 1;
    //int x_ratio = (int)((w1<<16)/w2) ;
    //int y_ratio = (int)((h1<<16)/h2) ;
    int x2, y2 ;

    for (int i = 0;i < res.height; i++) {
        for (int j = 0; j < res.width; j++) {
            x2 = ((j * x_ratio)>>16) ;
            y2 = ((i * y_ratio)>>16) ;
            mNew.pixel[i][j] = this->pixel[y2][x2] ;
        }                
    }               

    return mNew;
}
