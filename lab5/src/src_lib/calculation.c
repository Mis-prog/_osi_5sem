//#include <stdio.h>
//#include <stdlib.h>
//
//#define MAX_COUNT 10
//
//struct data {
//    int n_point;
//    double temp, dx, x, y, z;
//    double time, temp_mean, dx_mean;
//};
//
//struct data *task1(double x, double y, double z);
//struct data *task2();

#include "calculation.h"

int optimal_i(const struct data *n_points);

struct data* calculation_point(double x, double y, double z, FILE *file2);

void calculation_parametr(FILE *file1, struct data *n_points);

struct data *task1(double x, double y, double z) {
    char name1[] = "../../data/BD 0 - 0.07 - 0.01.txt";
    char name2[] = "../../data/BD_Coords.txt";

    FILE *file1 = fopen(name1, "r");
    FILE *file2 = fopen(name2, "r");

    if (file1 == NULL && file2 == NULL) {
        printf("Не удалось открыть файлы\n");
        return 0;
    }

    struct data *n_points = calculation_point(x, y, z, file2);
    int data_n = optimal_i(n_points);
    calculation_parametr(file1, n_points);

    fclose(file1);
    fclose(file2);

    return n_points;
}

void calculation_parametr(FILE *file1, struct data *n_points) {
    int n_point = 0, i = 0;
    double time, temp, dx;
    while (fscanf(file1, "%lf\t%d\t%lf\t%lf", &time, &n_point, &temp, &dx) == 4) {
        if (n_point == n_points[i].n_point) {
            n_points->temp = temp;
            n_points->dx = dx;
        }
    }
}

struct data *calculation_point(double x, double y, double z, FILE *file2) {
    struct data *n_points = (struct data *) calloc(MAX_COUNT, sizeof(struct data));

    int n_point = 0, i = 0;
    double x_cur, y_cur, z_cur;
    while (fscanf(file2, "%d\t%lf\t%lf\t%lf", &n_point, &x_cur, &y_cur, &z_cur) == 4) {
        if (x == x_cur && y == y_cur && z_cur == z) {
            n_points[i].n_point = n_point;
            n_points[i].x = x;
            n_points[i].y = y;
            n_points[i].z = z;
            i++;
        }
    }
    n_points[i].n_point = -1;
    return n_points;
}

int optimal_i(const struct data *n_points) {
    int i = 0;
    while (n_points[i].n_point != -1) {
        i++;
    }
    return i;
}

struct data* task2() {
    char name1[] = "../../data/BD 0 - 0.07 - 0.01.txt";

    FILE *file1 = fopen(name1, "r");

    if (file1 == NULL) {
        printf("Не удалось открыть файл\n");
        return 0;
    }

    struct data *n_points = (struct data *) calloc(MAX_COUNT, sizeof(struct data));

    int n_point = 0, i = 0, dx_count = 0, temp_count = 0;
    double time, temp, dx, dx_mean = 0, temp_mean = 0;
    n_points[0].time = 0.01;
    while (fscanf(file1, "%lf\t%d\t%lf\t%lf", &time, &n_point, &temp, &dx) == 4) {
        if (n_points[i].time != time) {
//            printf("time: %f,sum_dx: %f,count_dx: %d",n_points[i].time,dx_mean,dx_count);
//            printf("time: %f,sum_temp: %f,count_temp: %d",n_points[i].time,temp_mean,temp_count);
            dx_mean=dx_mean/(double)dx_count;
//            printf(",time_mean: %.9f\n",dx_mean);
            temp_mean=temp_mean/(double)temp_count;
//            printf(",time_mean: %f\n",temp_mean);
            n_points[i].temp_mean=temp_mean;
            n_points[i].dx_mean=dx_mean;
            dx_count = 0, temp_count = 0;
            dx_mean = 0, temp_mean = 0;
            i++;
            n_points[i].time = time;
        }
        dx_mean+=dx;
        dx_count+=1;

        temp_mean+=temp;
        temp_count+=1;
    }
    return n_points;
}
