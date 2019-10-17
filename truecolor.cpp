#include "truecolor.h"
#include "image_format.h"
#include "image_type.h"
#include "ppm_state.h"
#include "utils.h"
#include "operation.h"
#include "transformation.h"
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