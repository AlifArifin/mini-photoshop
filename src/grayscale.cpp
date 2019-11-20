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

Grayscale::Grayscale() {
    
}

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

Grayscale::Grayscale(const Binary & binary) : Monochrome(binary) {
    this->imageType = ImageType::GRAYSCALE;
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

Binary Grayscale::edgeDetection(EdgeDetection e, int t, int c) {
    Binary res(ImageFormat::NONE, this->resolution);

    float ** kernelx;
    float ** kernely;

    switch (e) {
        case (EdgeDetection::GRADIENT) : {
            kernelx = Monochrome::initPixel(2, 1);
            kernelx[0][0] = -1; kernelx[1][0] = 1;
            Grayscale x = this->convolutionTopLeft(Convolution::NO_CLIPPING, 2, 1, kernelx);

            qInfo(to_string(x.pixel[0][0]).c_str());

            kernely = Monochrome::initPixel(1, 2);
            kernely[0][0] = -1; kernely[0][1] = 1;
            Grayscale y = this->convolutionTopLeft(Convolution::NO_CLIPPING, 1, 2, kernely);

            qInfo(to_string(y.pixel[0][0]).c_str());

            Grayscale mNew = x.operation(&y, Operation::ADD_ABS, this->level);

            for (int i = 0; i < mNew.resolution.height; i++) {
                for (int j = 0; j < mNew.resolution.width; j++) {
                    if (mNew.pixel[i][j] >= t) {
                        res.setIndividualPixel(i, j, 1);
                    } else {
                        res.setIndividualPixel(i, j, 0);
                    }
                }
            }

            for (int i = 0; i < 2; i++) {
                delete [] kernelx[i];
            }
            delete [] kernelx;

            for (int i = 0; i < 1; i++) {
                delete [] kernely[i];
            }
            delete [] kernely;

            break;
        }
        case (EdgeDetection::LAPLACE) : {
            kernelx = Monochrome::initPixel(3, 3);
            kernelx[0][0] = 0; kernelx[0][1] = 1;  kernelx[0][2] = 0;
            kernelx[1][0] = 1; kernelx[1][1] = -4; kernelx[1][2] = 1;
            kernelx[2][0] = 0; kernelx[2][1] = 1;  kernelx[2][2] = 0;

            Grayscale mNew = this->convolution(Convolution::NO_CLIPPING, Padding::ZERO, 3, kernelx);

            qInfo(to_string(mNew.pixel[0][0]).c_str());

            qInfo(mNew.toString().c_str());

            // x
            short pxl_bef = 0;
            for (int i = 0; i < mNew.resolution.height; i++) {
                for (int j = 0; j < mNew.resolution.width; j++) {
                    if (j != 0) {
                        if ((pxl_bef < 0 && mNew.pixel[i][j] > 0) || (pxl_bef > 0 && mNew.pixel[i][j] < 0)) {
                            res.setIndividualPixel(i, j, 1);
                        }
                    }
                    pxl_bef = mNew.pixel[i][j];
                }
            }

            // y
            for (int j = 0; j < mNew.resolution.width; j++) {
                for (int i = 0; i < mNew.resolution.height; i++) {
                    if (i != 0) {
                        if ((pxl_bef < 0 && mNew.pixel[i][j] > 0) || (pxl_bef > 0 && mNew.pixel[i][j] < 0)) {
                            res.setIndividualPixel(i, j, 1);
                        }
                    }
                    pxl_bef = mNew.pixel[i][j];
                }
            }

            for (int i = 0; i < 3; i++) {
                delete [] kernelx[i];
            }
            delete [] kernelx;

            break;
        }
        case (EdgeDetection::L_O_G) : {
            kernelx = Monochrome::initPixel(5, 5);
            kernelx[0][0] =  0; kernelx[0][1] =  0; kernelx[0][2] = -1; kernelx[0][3] =  0; kernelx[0][4] =  0;
            kernelx[1][0] =  0; kernelx[1][1] = -1; kernelx[1][2] = -2; kernelx[1][3] = -1; kernelx[1][4] =  0;
            kernelx[2][0] = -1; kernelx[2][1] = -2; kernelx[2][2] = 16; kernelx[2][3] = -2; kernelx[2][4] = -1;
            kernelx[3][0] =  0; kernelx[3][1] = -1; kernelx[3][2] = -2; kernelx[3][3] = -1; kernelx[3][4] =  0;
            kernelx[4][0] =  0; kernelx[4][1] =  0; kernelx[4][2] = -1; kernelx[4][3] =  0; kernelx[4][4] =  0;

            Grayscale mNew = this->convolution(Convolution::NO_CLIPPING, Padding::ZERO, 5, kernelx);

            // x
            short pxl_bef = 0;
            for (int i = 0; i < mNew.resolution.height; i++) {
                for (int j = 0; j < mNew.resolution.width; j++) {
                    if (j != 0) {
                        if ((pxl_bef < 0 && mNew.pixel[i][j] > 0) || (pxl_bef > 0 && mNew.pixel[i][j] < 0)) {
                            res.setIndividualPixel(i, j, 1);
                        }
                    }
                    pxl_bef = mNew.pixel[i][j];
                }
            }

            for (int j = 0; j < mNew.resolution.width; j++) {
                for (int i = 0; i < mNew.resolution.height; i++) {
                    if (i != 0) {
                        if ((pxl_bef < 0 && mNew.pixel[i][j] > 0) || (pxl_bef > 0 && mNew.pixel[i][j] < 0)) {
                            res.setIndividualPixel(i, j, 1);
                        }
                    }
                    pxl_bef = mNew.pixel[i][j];
                }
            }

            for (int i = 0; i < 5; i++) {
                delete [] kernelx[i];
            }
            delete [] kernelx;
            break;
        }
        case (EdgeDetection::PREWITT) :
            c = 1;
        case (EdgeDetection::SOBEL) : {
            qInfo("sobel");
            
            kernelx = Monochrome::initPixel(3, 3);
            kernelx[0][0] = -1; kernelx[1][0] = c * -1; kernelx[2][0] = -1;
            kernelx[0][2] = 1; kernelx[1][2] = c * 1; kernelx[2][2] = 1;
            Grayscale x = this->convolution(Convolution::NO_CLIPPING, Padding::ZERO, 3, kernelx);

            qInfo("sobel");
            
            kernely = Monochrome::initPixel(3, 3);
            kernely[0][0] = 1; kernely[0][1] = c * 1; kernely[0][2] = 1;
            kernely[2][0] = -1; kernely[2][1] = c * -1; kernely[2][2] = -1;
            Grayscale y = this->convolution(Convolution::NO_CLIPPING, Padding::ZERO, 3, kernely);
            
            qInfo("sobel");
            
            Grayscale mNew = x.operation(&y, Operation::ADD_ABS, this->level);

            qInfo("sobel");
            
            for (int i = 0; i < mNew.resolution.height; i++) {
                for (int j = 0; j < mNew.resolution.width; j++) {
                    if (mNew.pixel[i][j] >= t) {
                        res.setIndividualPixel(i, j, 1);
                    } else {
                        res.setIndividualPixel(i, j, 0);
                    }
                }
            }

            qInfo("sobel");

            for (int i = 0; i < 3; i++) {
                delete [] kernelx[i];
                delete [] kernely[i];
            }
            delete [] kernelx;
            delete [] kernely;

            break;
        }
        case (EdgeDetection::ROBERTS) : {
            kernelx = Monochrome::initPixel(2, 2);
            kernelx[0][0] = 1; kernelx[0][1] =  0; 
            kernelx[1][0] = 0; kernelx[1][1] = -1; 
            Grayscale x = this->convolutionTopLeft(Convolution::NO_CLIPPING, 2, 2, kernelx);

            kernely = Monochrome::initPixel(2, 2);
            kernely[0][0] =  0; kernely[0][1] = 1; 
            kernely[1][0] = -1; kernely[1][1] = 0; 
            Grayscale y = this->convolutionTopLeft(Convolution::NO_CLIPPING, 2, 2, kernely);

            Grayscale mNew = x.operation(&y, Operation::ADD_ABS, this->level);

            qInfo(to_string(t).c_str());

            for (int i = 0; i < mNew.resolution.height; i++) {
                for (int j = 0; j < mNew.resolution.width; j++) {
                    if (mNew.pixel[i][j] >= t) {
                        res.setIndividualPixel(i, j, 1);
                    } else {
                        res.setIndividualPixel(i, j, 0);
                    }
                }
            }

            for (int i = 0; i < 2; i++) {
                delete [] kernelx[i];
                delete [] kernely[i];
            }
            delete [] kernelx;
            delete [] kernely;

            break;
        }
        case (EdgeDetection::CANNY) : {
            kernelx = Monochrome::initPixel(5, 5);
            kernelx[0][0] = 1/273.0; kernelx[0][1] =  4/273.0; kernelx[0][2] = 7/273.0; kernelx[0][3] =  4/273.0; kernelx[0][4] =  1/273.0;
            kernelx[1][0] = 4/273.0; kernelx[1][1] = 16/273.0; kernelx[1][2] = 26/273.0; kernelx[1][3] = 16/273.0; kernelx[1][4] =  4/273.0;
            kernelx[2][0] = 7/273.0; kernelx[2][1] = 26/273.0; kernelx[2][2] = 41/273.0; kernelx[2][3] = 26/273.0; kernelx[2][4] = 7/273.0;
            kernelx[3][0] = 4/273.0; kernelx[3][1] = 16/273.0; kernelx[3][2] = 26/273.0; kernelx[3][3] = 16/273.0; kernelx[3][4] =  4/273.0;
            kernelx[4][0] = 1/273.0; kernelx[4][1] =  4/273.0; kernelx[4][2] = 7/273.0; kernelx[4][3] =  4/273.0; kernelx[4][4] =  1/273.0;

            Grayscale x = this->convolution(Convolution::NO_CLIPPING, Padding::SAME, 5, kernelx);

            for (int i = 0; i < 5; i++) {
                delete [] kernelx[i];
            }
            delete [] kernelx;

            Binary res2 = x.edgeDetection(EdgeDetection::SOBEL, t, c);

            return res2;

            break;
        }
    }

    return res;
}

Binary Grayscale::binarySegmentation(int t) {
    Binary res(ImageFormat::NONE, this->resolution);

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            if (this->pixel[i][j] >= t) {
                res.setIndividualPixel(i, j, 1);
            } else {
                res.setIndividualPixel(i, j, 0);
            }
        }
    }

    return res;
}
