#include <stdio.h>
#include <time.h>

int main() {
    time_t currentTime;
    struct tm *localTime;

    // Получаем текущее время
    currentTime = time(NULL);

    // Преобразуем время в локальное представление
    localTime = localtime(&currentTime);

    // Выводим текущее время в формате часы:минуты:секунды
    printf("Текущее время: %02d:%02d:%02d\n",
           localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    return 0;
}
