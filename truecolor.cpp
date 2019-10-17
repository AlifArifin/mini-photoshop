#include "truecolor.h"
#include "image_format.h"
#include "image_type.h"
#include "ppm_state.h"
#include "utils.h"
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
