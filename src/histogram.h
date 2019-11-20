#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

struct Histogram {
    int* value;
    float* norm;
    int size;
    int max;
    float mean;
    double var;
    double std;
    int total;
};

#endif
