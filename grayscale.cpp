#include "grayscale.h"
#include "image_format.h"
#include "image_type.h"
#include "pgm_state.h"
#include "utils.h"
#include "transformation.h"
#include "binary.h"
#include <iostream>
#include <vector>
#include <QDebug>

Grayscale::Grayscale(ifstream * file, ImageFormat imageFormat, ImageType imageType) : Monochrome(imageFormat, imageType) {
    qInfo("create grayscale");
    Resolution res;
    string fileline;
    int p, q;
    vector<string> pixels;

    if (!file->is_open()) {
        throw "Error, cannot open file";
    }

    int debug = 0;

    switch (imageFormat) {
        case (ImageFormat::PGM) : {
            qInfo("create grayscale");
            PGMState pgmState = PGMState::PGM_CODE;
            while (getline(*file, fileline)) {
                debug++;
                //qInfo(to_string(debug).c_str());
                switch (pgmState) {
                    case (PGMState::PGM_CODE) : {
                        pgmState = PGMState::PGM_SIZE;
                        break;
                    }
                    case (PGMState::PGM_SIZE) : {
                        if (!isComment(fileline)) {
                            vector<string> words = Utils::split(fileline, ' ');

                            if (words.size() == 2) {
                                res.width = stoi(words[0]);
                                res.height = stoi(words[1]);
                            }
                            this->resolution = res;

                            this->pixel = new short*[res.height];
                            for (int i = 0; i < res.height; i++) {
                                this->pixel[i] = new short[res.width];
                            }

                            // init
                            p = 0;
                            q = 0;

                            pgmState = PGMState::PGM_LEVEL;
                        }
                        break;
                    }
                    case (PGMState::PGM_LEVEL) : {
                        qInfo("create grayscale");
                        this->level = stoi(fileline);
                        pgmState = PGMState::PGM_PIXEL;
                        break;
                    }
                    case (PGMState::PGM_PIXEL) : {
                        pixels = Utils::split(fileline, ' ');
                        for (size_t i = 0; i < pixels.size(); i++) {
                            this->pixel[p][q] = stoi(pixels[i]);
                            if (q >= res.width - 1) {
                                p++;
                                q = 0;
                            } else {
                                q++;
                            }
                        }
                        break;
                    }
                    default :
                        break;
                }
            }
            break;
        } case (ImageFormat::RAW_IMAGE) : {
            this->level = 255;
            int counter = 0;
            while (getline(*file, fileline)) {
                if (counter == 0) {
                    vector<string> words = Utils::split(fileline, ' ');

                    if (words.size() == 2) {
                        res.width = stoi(words[0]);
                        res.height = stoi(words[1]);
                    }
                    this->resolution = res;

                    this->pixel = new short*[res.height];
                    for (int i = 0; i < res.height; i++) {
                        this->pixel[i] = new short[res.width];
                    }

                    // init
                    p = 0;
                    q = 0;
                } else {
                    pixels = Utils::split(fileline, ' ');
                    for (size_t i = 0; i < pixels.size(); i++) {
                        this->pixel[p][q] = stoi(pixels[i]);
                        if (q >= res.width - 1) {
                            p++;
                            q = 0;
                        } else {
                            q++;
                        }
                    }
                }
                counter++;
            }
            break;
        }
    }
}

Grayscale::Grayscale(ImageFormat imageFormat, Resolution resolution, int level) : Monochrome(imageFormat, ImageType::GRAYSCALE, resolution, level) {

}

Grayscale::Grayscale(const Grayscale & grayscale) : Monochrome(grayscale) {

}

Grayscale::Grayscale(const Monochrome & monochrome) : Monochrome(monochrome) {

}

Grayscale Grayscale::contrastStretching(int a, int b, int ya, int yb, float alpha, float beta, float gamma) {
    qInfo("contrast");
    Grayscale gNew(
        this->imageFormat,
        this->resolution, 
        this->level
    );

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            short temp;
            if (this->pixel[i][j] < a) {
                temp = (int) (alpha * this->pixel[i][j]);
            } else if (this->pixel[i][j] >= a && this->pixel[i][j] < b) {
                temp = (int) (beta * (this->pixel[i][j] - a) + ya);
            } else {
                temp = (int) (gamma * (this->pixel[i][j] - b) + yb);
            }

            gNew.pixel[i][j] = Image::clipping(temp, this->level);
        }
    }

    return gNew;
}

Grayscale Grayscale::transformation(Transformation t, float c, float gamma) {
    Grayscale gNew(
        this->imageFormat,
        this->resolution, 
        this->level
    );

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            short temp;

            switch (t) {
                case (Transformation::LOG) : {
                    temp = (int) c * log(1 + this->pixel[i][j]);
                    break;
                }
                case (Transformation::INVERSE_LOG) : {
                    temp = (int) pow(exp(1 + this->pixel[i][j]), (float) 1 / c) - 1;
                    break;
                }
                case (Transformation::POWER) : {
                    temp = (int) c * pow(this->pixel[i][j], gamma);
                    break;
                }
            }

            gNew.pixel[i][j] = Image::clipping(temp, this->level);
        }
    }
    return gNew;
}

Grayscale Grayscale::slicing(int a, int b, int max) {
    Grayscale gNew(
        this->imageFormat,
        this->resolution, 
        this->level
    );

    if (max > level) {
        max = level;
    }

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            short temp;

            if (this->pixel[i][j] >= a && this->pixel[i][j] <= b) {
                gNew.pixel[i][j] = max;
            } else {
                gNew.pixel[i][j] = this->pixel[i][j];
            }
        }  
    }

    return gNew;
} 

vector<Binary*> Grayscale::bitSlicing() {
    // asumsi 1 pixel = 8 bit
    vector<Binary*> vec;
    vec.push_back(new Binary(ImageFormat::NONE, this->resolution));
    vec.push_back(new Binary(ImageFormat::NONE, this->resolution));
    vec.push_back(new Binary(ImageFormat::NONE, this->resolution));
    vec.push_back(new Binary(ImageFormat::NONE, this->resolution));
    vec.push_back(new Binary(ImageFormat::NONE, this->resolution));
    vec.push_back(new Binary(ImageFormat::NONE, this->resolution));
    vec.push_back(new Binary(ImageFormat::NONE, this->resolution));
    vec.push_back(new Binary(ImageFormat::NONE, this->resolution));

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            if (this->pixel[i][j] & 1) {
                vec[0]->setIndividualPixel(i, j, 1);
            }
            if (this->pixel[i][j] & 2) {
                vec[1]->setIndividualPixel(i, j, 1);
            }
            if (this->pixel[i][j] & 4) {
                vec[2]->setIndividualPixel(i, j, 1);
            }
            if (this->pixel[i][j] & 8) {
                vec[3]->setIndividualPixel(i, j, 1);
            }
            if (this->pixel[i][j] & 16) {
                vec[4]->setIndividualPixel(i, j, 1);
            }
            if (this->pixel[i][j] & 32) {
                vec[5]->setIndividualPixel(i, j, 1);
            }
            if (this->pixel[i][j] & 64) {
                vec[6]->setIndividualPixel(i, j, 1);
            }
            if (this->pixel[i][j] & 128) {
                vec[7]->setIndividualPixel(i, j, 1);
            }
        }
    }

    return vec;
}
