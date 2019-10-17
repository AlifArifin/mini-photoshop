#include "binary.h"
#include <iostream>
#include "image_format.h"
#include "image_type.h"
#include <vector>
#include "pbm_state.h"
#include "utils.h"
#include <fstream>
#include <QDebug>
#include <cstring>

Binary::Binary() {

}

Binary::Binary(ifstream * file, ImageFormat imageFormat, ImageType imageType) : Monochrome(imageFormat, imageType) {
    Resolution res;
    string fileline;
    int p, q;
    vector<string> pixels;

    if (!file->is_open()) {
        throw "Error, cannot open file";
    }

    switch (imageFormat) {
        case (ImageFormat::PBM) : {
            PBMState pbmState = PBMState::PBM_CODE;
            this->level = 1;
            while (getline(*file, fileline)) {
                switch (pbmState) {
                    case (PBMState::PBM_CODE) : {
                        pbmState = PBMState::PBM_SIZE;
                        break;
                    }
                    case (PBMState::PBM_SIZE) : {
                        if (!isComment(fileline)) {
                            vector<string> words = Utils::split(fileline, ' ');

                            if (words.size() >= 2) {
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

                            pbmState = PBMState::PBM_PIXEL;
                        }
                        break;
                    }
                    case (PBMState::PBM_PIXEL) : {
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
                    default:
                        break;
                }
            }
            break;
        }
    }
}

Binary::Binary(ImageFormat imageFormat, Resolution resolution) : Monochrome(imageFormat, ImageType::BINARY, resolution, 1) {

}

Binary::Binary(const Binary & binary) : Monochrome(binary) {

}

Binary::Binary(const Monochrome & monochrome) : Monochrome(monochrome) {

}

Binary Binary::Not() {
    Binary bNew(ImageFormat::NONE, this->resolution);

    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {
            bNew.pixel[i][j] = 1 - this->pixel[i][j];
        }
    }

    return bNew;
}
