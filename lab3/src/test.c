#include <stdio.h>
#include <time.h>

void writeLog(int userId, const char* userCommand, int serverResponse) {
}

int main() {
    // Пример использования функции записи в лог
    writeLog(123, "GET /index.html", 200);
    writeLog(456, "POST /login", 500);

    return 0;
}
