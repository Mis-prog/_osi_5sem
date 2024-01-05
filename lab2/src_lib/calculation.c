#include <stdio.h>
#include <stdlib.h>

const int MAX_COUNT = 10;

struct data {
    int n_point;
    double temp, dx, x, y, z;
};

int optimal_i(const struct data *n_points);

int *calculation_point(double x, double y, double z, FILE *file2);

void calculation_parametr(FILE *file1, struct data *n_points);

struct data *task1(double x, double y, double z) {
    char name1[] = "BD 0 - 0.07 - 0.01.txt";
    char name2[] = "BD_Coords.txt";

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

int *calculation_point(double x, double y, double z, FILE *file2) {
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

void task2(){

}


int main() {
    return 0;
}
