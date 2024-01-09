#include <stdio.h>
#include <stdlib.h>

#define MAX_COUNT 10

struct data {
    int n_point;
    double temp, dx, x, y, z;
    double time, temp_mean, dx_mean;
};

struct data *task1(double x, double y, double z);
struct data *task2();