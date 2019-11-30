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

Binary Grayscale::edgeDetection(EdgeDetection e, int t, int c, float lowT, float highT, float sigma) {
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
                    qInfo(to_string(mNew.pixel[i][j]).c_str());
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

            // for (int i = 0; i < mNew.resolution.height; i++) {
            //     for (int j = 0; j < mNew.resolution.width; j++) {
            //         if (mNew.pixel[i][j] >= t) {
            //             res.setIndividualPixel(i, j, 1);
            //         } else {
            //             res.setIndividualPixel(i, j, 0);
            //         }
            //     }
            // }

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
            kernelx = Monochrome::initPixel(3, 3);
            kernelx[0][0] = -1; kernelx[1][0] = c * -1; kernelx[2][0] = -1;
            kernelx[0][2] = 1; kernelx[1][2] = c * 1; kernelx[2][2] = 1;
            Grayscale x = this->convolution(Convolution::NO_CLIPPING, Padding::ZERO, 3, kernelx);

            kernely = Monochrome::initPixel(3, 3);
            kernely[0][0] = 1; kernely[0][1] = c * 1; kernely[0][2] = 1;
            kernely[2][0] = -1; kernely[2][1] = c * -1; kernely[2][2] = -1;
            Grayscale y = this->convolution(Convolution::NO_CLIPPING, Padding::ZERO, 3, kernely);
            
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
            Grayscale gauss = this->gaussianSmoothing(sigma);

            kernelx = Monochrome::initPixel(2, 2);
            kernelx[0][0] = 1; kernelx[0][1] =  0; 
            kernelx[1][0] = 0; kernelx[1][1] = -1; 
            Grayscale x = gauss.convolutionTopLeft(Convolution::NO_CLIPPING, 2, 2, kernelx);

            kernely = Monochrome::initPixel(2, 2);
            kernely[0][0] =  0; kernely[0][1] = 1; 
            kernely[1][0] = -1; kernely[1][1] = 0; 
            Grayscale y = gauss.convolutionTopLeft(Convolution::NO_CLIPPING, 2, 2, kernely);

            Grayscale sobel = x.operation(&y, Operation::ADD_ABS, this->level);
            Grayscale angle (
                this->imageFormat,
                this->resolution, 
                this->level
            );

            int max_pixel = 0;

            for (int i = 0; i < this->resolution.height; i++) {
                for (int j = 0; j < this->resolution.width; j++) {
                    if (max_pixel < sobel.pixel[i][j]) {
                        max_pixel = sobel.pixel[i][j];
                    }
                    if (x.pixel[i][j] <= 0.00001 && x.pixel[i][j] >= -0.00001) {
                        if (y.pixel[i][j] > 0) {
                            angle.pixel[i][j] = 90;
                        } else {
                            angle.pixel[i][j] = -90;
                        }
                    } else {
                        angle.pixel[i][j] = atan(y.pixel[i][j] / x.pixel[i][j]) * 180 / 3.14159265; 
                    }

                    if (angle.pixel[i][j] < 0) {
                        angle.pixel[i][j] += 180;
                    }
                }
            }

            for (int i = 0; i < 2; i++) {
                delete [] kernelx[i];
                delete [] kernely[i];
            }
            delete [] kernelx;
            delete [] kernely;

            Grayscale non_max (
                this->imageFormat,
                this->resolution, 
                this->level
            );

            for (int i = 1; i < angle.resolution.height - 1; i++) {
                for (int j = 1; j < angle.resolution.width - 1; j++) {

                    int q = 255;
                    int r = 255;

                    if ((0 <= angle.pixel[i][j] && angle.pixel[i][j] < 22.5) || (157.5 <= angle.pixel[i][j] && angle.pixel[i][j] <= 180)) {
                        q = sobel.pixel[i][j + 1];
                        r = sobel.pixel[i][j - 1];
                    } else if (22.5 <= angle.pixel[i][j] && angle.pixel[i][j] < 67.5) {
                        q = sobel.pixel[i + 1][j - 1];
                        r = sobel.pixel[i - 1][j + 1];
                    } else if (67.5 <= angle.pixel[i][j] && angle.pixel[i][j] < 112.5) {
                        q = sobel.pixel[i + 1][j];
                        r = sobel.pixel[i - 1][j];
                    } else if (112.5 <= angle.pixel[i][j] && angle.pixel[i][j] < 157.5) {
                        q = sobel.pixel[i - 1][j - 1];
                        r = sobel.pixel[i + 1][j + 1];
                    }

                    if (sobel.pixel[i][j] >= q && sobel.pixel[i][j] >= r) {
                        non_max.pixel[i][j] = sobel.pixel[i][j];
                    } else {
                        non_max.pixel[i][j] = 0;
                    }
                }
            }

            Grayscale double_threshold (
                this->imageFormat,
                this->resolution, 
                this->level
            );

            int highThreshold = (int) (max_pixel * highT);
            int lowThreshold = (int) (highThreshold * lowT);

            int weak = 25;
            int strong = 255;

            for (int i = 0; i < non_max.resolution.height; i++) {
                for (int j = 0; j < non_max.resolution.width; j++) {
                    if (non_max.pixel[i][j] >= highThreshold) {
                        double_threshold.pixel[i][j] = strong;
                    } else if (non_max.pixel[i][j] >= lowThreshold) {
                        double_threshold.pixel[i][j] = weak;
                    } else {
                        double_threshold.pixel[i][j] = 0;
                    }
                }
            }

            for (int i = 1; i < double_threshold.resolution.height - 1; i++) {
                for (int j = 1; j < double_threshold.resolution.width - 1; j++) {
                    if (double_threshold.pixel[i][j] == weak) {
                        if ((double_threshold.pixel[i + 1][j + 1] == strong) || 
                            (double_threshold.pixel[i + 1][j] == strong) ||
                            (double_threshold.pixel[i + 1][j - 1] == strong) ||
                            (double_threshold.pixel[i][j - 1] == strong) ||
                            (double_threshold.pixel[i - 1][j - 1] == strong) ||
                            (double_threshold.pixel[i - 1][j] == strong) ||
                            (double_threshold.pixel[i - 1][j + 1] == strong) ||
                            (double_threshold.pixel[i][j + 1] == strong)) {
                                res.setIndividualPixel(i, j, 1);
                            }
                    } else if (double_threshold.pixel[i][j] == strong) {
                        res.setIndividualPixel(i, j, 1);
                    }
                }
            }

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

Grayscale Grayscale::gaussianSmoothing(float sigma) {
    float ** kernelx = Monochrome::initPixel(5, 5);

    float normal = 1 / (2.0 * 3.14159265 * pow(sigma, 2));

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            kernelx[i][j] = exp(- ((pow(i - 2, 2) + pow(j - 2, 2)) / (2.0 * pow(sigma, 2)))) * normal;
        }
    }

    // kernelx[0][0] = 1/273.0; kernelx[0][1] =  4/273.0; kernelx[0][2] = 7/273.0; kernelx[0][3] =  4/273.0; kernelx[0][4] =  1/273.0;
    // kernelx[1][0] = 4/273.0; kernelx[1][1] = 16/273.0; kernelx[1][2] = 26/273.0; kernelx[1][3] = 16/273.0; kernelx[1][4] =  4/273.0;
    // kernelx[2][0] = 7/273.0; kernelx[2][1] = 26/273.0; kernelx[2][2] = 41/273.0; kernelx[2][3] = 26/273.0; kernelx[2][4] = 7/273.0;
    // kernelx[3][0] = 4/273.0; kernelx[3][1] = 16/273.0; kernelx[3][2] = 26/273.0; kernelx[3][3] = 16/273.0; kernelx[3][4] =  4/273.0;
    // kernelx[4][0] = 1/273.0; kernelx[4][1] =  4/273.0; kernelx[4][2] = 7/273.0; kernelx[4][3] =  4/273.0; kernelx[4][4] =  1/273.0;

    Grayscale x = this->convolution(Convolution::NO_CLIPPING, Padding::SAME, 5, kernelx);

    for (int i = 0; i < 5; i++) {
        delete [] kernelx[i];
    }
    delete [] kernelx;

    return x;
}

Grayscale Grayscale::degreeEdgeDetection(EdgeDetection e, int t, int c) {
    Grayscale res(ImageFormat::NONE, this->resolution, 360);

    float ** kernelx;
    float ** kernely;

    switch (e) {
        case (EdgeDetection::PREWITT) :
            c = 1;
        case (EdgeDetection::SOBEL) : {
            kernelx = Monochrome::initPixel(3, 3);
            kernelx[0][0] = -1; kernelx[1][0] = c * -1; kernelx[2][0] = -1;
            kernelx[0][2] = 1; kernelx[1][2] = c * 1; kernelx[2][2] = 1;
            Grayscale x = this->convolution(Convolution::NO_CLIPPING, Padding::ZERO, 3, kernelx);

            kernely = Monochrome::initPixel(3, 3);
            kernely[0][0] = 1; kernely[0][1] = c * 1; kernely[0][2] = 1;
            kernely[2][0] = -1; kernely[2][1] = c * -1; kernely[2][2] = -1;
            Grayscale y = this->convolution(Convolution::NO_CLIPPING, Padding::ZERO, 3, kernely);
            
            for (int i = 0; i < this->resolution.height; i++) {
                for (int j = 0; j < this->resolution.width; j++) {
                    if (x.pixel[i][j] <= 0.00001 && x.pixel[i][j] >= -0.00001) {
                        if (y.pixel[i][j] > 0) {
                            res.pixel[i][j] = 90;
                        } else {
                            res.pixel[i][j] = -90;
                        }
                    } else {
                        res.pixel[i][j] = atan(y.pixel[i][j] / x.pixel[i][j]) * 180 / 3.14159265; 
                    }

                    if (res.pixel[i][j] < 0) {
                        res.pixel[i][j] += 180;
                    }
                }
            }

            for (int i = 0; i < 3; i++) {
                delete [] kernelx[i];
                delete [] kernely[i];
            }
            delete [] kernelx;
            delete [] kernely;

            break;
        }
    }

    return res;
}
