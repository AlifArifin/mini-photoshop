#include "binary.h"
#include <iostream>
#include "image_format.h"
#include "image_type.h"
#include <vector>
#include <set>
#include "pbm_state.h"
#include "utils.h"
#include <fstream>
#include <QDebug>
#include <cstring>
#include <algorithm>

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

Binary Binary::thinning() {
    Binary bNew(*this);

    int offset = 1;

    int * per = new int[9];
    bool end = true;

    do {
        end = true;
        for (int i = 1; i < bNew.resolution.height - 1; i++) {
            for (int j = 1; j < bNew.resolution.width - 1; j++) {
                if (bNew.pixel[i][j] == 1) {
                    int count = 0;
                    for (int p = i - 1; p <= i + 1; p++) {
                        for (int q = j - 1; q <= j + 1; q++) {
                            if (bNew.pixel[p][q] == 1) {
                                count++;
                            }
                        }
                    }

                    if (count > 2 && count < 8) {
                        int trans = 0;

                        per[0] = bNew.pixel[i - 1][j];
                        per[1] = bNew.pixel[i - 1][j + 1];
                        per[2] = bNew.pixel[i][j + 1];
                        per[3] = bNew.pixel[i + 1][j + 1];
                        per[4] = bNew.pixel[i + 1][j];
                        per[5] = bNew.pixel[i + 1][j - 1];
                        per[6] = bNew.pixel[i][j - 1];
                        per[7] = bNew.pixel[i - 1][j - 1];
                        per[8] = bNew.pixel[i - 1][j];

                        for (int i = 0; i < 8; i++) {
                            if (per[i] == 0 && per[i + 1] == 1) {
                                trans++;
                            }
                        }

                        if (trans == 1) {
                            bNew.pixel[i][j] = 0;
                            end = false;
                        }
                    }
                }
            }
        }
    } while (!end);

    return bNew;
}

Monochrome Binary::cclTwoPass() {
    qInfo("ccl");
    vector<set<short>> linked;
    Monochrome labels(
        this->imageFormat,
        ImageType::GRAYSCALE,
        this->resolution,
        255
    );

    linked.push_back({0});

    short currlabel = 1;
    set<short>::iterator it;

    qInfo("ccl");
    
    // first pass
    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {            
            if (this->pixel[i][j]) {
                set<short> neighbors;
                
                if (i - 1 >= 0 && j - 1 >= 0) {
                    if (this->pixel[i - 1][j - 1]) {
                        neighbors.insert(labels.getIndividualPixel(i - 1, j - 1));
                    }
                }

                if (i - 1 >= 0) {
                    if (this->pixel[i - 1][j]) {
                        neighbors.insert(labels.getIndividualPixel(i - 1, j));
                    }
                }

                if (i - 1 >= 0 && j + 1 < this->resolution.width) {
                    if (this->pixel[i - 1][j + 1]) {
                        neighbors.insert(labels.getIndividualPixel(i - 1, j + 1));
                    }
                }

                if (j - 1 > 0) {
                    if (this->pixel[i][j - 1]) {
                        neighbors.insert(labels.getIndividualPixel(i, j - 1));
                    }
                }

                // if neighbors is empty set
                if (neighbors.empty()) {
                    linked.push_back({currlabel});
                    labels.setIndividualPixel(i, j, currlabel);
                    currlabel++;
                } else {
                    short min = -1;
                    for (it = neighbors.begin(); it != neighbors.end(); ++it)
                    {
                        linked[*it].insert(neighbors.begin(), neighbors.end());

                        if (min == -1) {
                            min = *it;
                        } else if (*it < min) {
                            min = *it;
                        }
                    }
                    labels.setIndividualPixel(i, j, min);
                }
            }
        }
    }

    vector<short> dict_cll;

    dict_cll.push_back(0);

    for (int i = 1; i < linked.size(); i++) {
        short min = -1;
        for (it = linked[i].begin(); it != linked[i].end(); ++it) {
            if (min == -1) {
                min = *it;
            } else if (*it < min) {
                min = *it;
            }
        }
        dict_cll.push_back(min);
    }

    // second pass
    for (int i = 0; i < this->resolution.height; i++) {
        for (int j = 0; j < this->resolution.width; j++) {            
            if (this->pixel[i][j]) {
                labels.setIndividualPixel(i, j, dict_cll.at(labels.getIndividualPixel(i, j)));
            }
        }
    }

    labels.setLevel(currlabel);

    return labels;
}