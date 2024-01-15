#include <stdio.h>
#include <stdlib.h>

#define MAX_COUNT 10

#ifndef CALCULATION_H
#define CALCULATION_H

#ifdef __cplusplus-
extern "C" {
#endif

struct data {
    int n_point;
    double temp, dx, x, y, z;
    double time, temp_mean, dx_mean;
};
struct data *task1(double x, double y, double z);
struct data *task2();


#ifdef __cplusplus
}
#endif

#endif // CALCULATION_H