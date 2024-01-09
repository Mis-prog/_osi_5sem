#include "stdio.h"

int main(){
    char name1[] = "../data/BD 0 - 0.07 - 0.01.txt";
    char name2[] = "../data/BD_Coords.txt";

    FILE *file1 = fopen(name1, "r");
    FILE *file2 = fopen(name2, "r");

    if (file1 == NULL && file2 == NULL) {
        printf("Не удалось открыть файлы\n");
        return 0;
    }
    printf("Удалось открыть файлы\n");
    fclose(file1);
    fclose(file2);
}