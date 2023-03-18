#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <search.h>

const int maxSize = 20000;

int main(int argc, char *argv[]) {

    int fd1[2];
    int fd2[2];
    char input[maxSize];

    pipe(fd1);
    pipe(fd2);
    pid_t pid;
    int size, n;
    char chars[maxSize]; // массив встречающихся символов
    int count[maxSize]; // число вхождений соответствующего символа
    char res[maxSize]; // строка для вывода
    char buff[maxSize];
    switch(pid=fork()) {
        case -1:
            perror("fork"); /* произошла ошибка */
            exit(1); /*выход из родительского процесса*/
        case 0:
            if ((fd1[0] = open(argv[1], O_RDONLY)) < 0) {
                printf("Can\'t open input file\n");
                exit(-1);
            }
            size = read(fd1[0], input, maxSize);
            if (size == -1) {
                printf("Can\'t read input file\n");
                exit(-1);
            }
            size = write(fd1[1], input, size);
            close(fd1[0]);
            close(fd1[1]);
            // Первый поток выводит ответ в файл
            // Имя файлв - 1-й аргумент командной стороки
            if ((fd2[1] = open(argv[2], O_WRONLY)) < 0) {
                printf("Can\'t open output file\n");
                exit(-1);
            }
            size = read(fd2[0], buff, maxSize);
            if (write(fd2[1], buff, size) == -1) {
                printf("Can\'t read input file\n");
                exit(-1);
            }
            close(fd2[0]);
            close(fd2[1]);
            exit(0);
        default:
            size = read(fd1[0], input, maxSize);
            close(fd1[0]);
            close(fd1[1]);
            for (int i = 0; i < maxSize && input[i] != 0; ++i) {
                if (input[i] < 32) {
                    // символ не отображаемый
                    continue;
                }
                int j = 0;
                for (; j < n && chars[j] != input[i]; ++j) {
                    // ищем встречался ли символ раньше
                }
                if (j == n) {
                    // если не встречался, то добавляем
                    chars[j] = input[i];
                    count[j] = 1;
                    n++;
                } else {
                    // если встречался, то увеличиваем колличество вхождений
                    count[j]++;
                }
            }
            for (int i = 0; i < n; ++i) {
                char cur[20];
                sprintf(cur, "%c: %d\n", chars[i], count[i]);
                sprintf(res, "%s%s", res, cur);
            }
            size = 0; // считаем длину выходной строки
            while (res[size] != 0) {
                size++;
            }

            size = write(fd2[1], res, size);
            close(fd2[0]);
            close(fd2[1]);
    }
    return 0;
}

