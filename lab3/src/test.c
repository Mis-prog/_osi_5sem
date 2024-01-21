#include <stdio.h>
#include <uuid/uuid.h>


int main() {
    uuid_t uuid;
    char uuid_str[37]; // 36 символов UUID и 1 символ для завершения строки

    // Генерируем UUID
    uuid_generate(uuid);

    // Преобразуем UUID в строку
    uuid_unparse(uuid, uuid_str);

    printf("%d",uuid);

    return 0;
}