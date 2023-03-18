#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

const int maxSize = 20;

int readAndWrite(int argc, char *argv[]) {
    int x, fd1, fd2, size;
    char input[maxSize], buff[maxSize];
    if ((x = open(argv[1], O_RDONLY)) < 0) {
        printf("Can\'t open input file\n");
        return -1;
    }
    if((fd1 = open("readChannel", O_WRONLY)) < 0){
        printf("Can\'t open FIFO for writting\n");
        return -1;
    }
    while (true) {
        size = read(x, input, maxSize);
        if (size == -1) {
            printf("Can\'t read input file\n");
            return -1;
        }
        size = write(fd1, input, size);
        if (size != maxSize) {
            break;
        }
    }
    close(fd1);
    // Первый поток выводит ответ в файл
    // Имя файлв - 1-й аргумент командной стороки
    if ((x = open(argv[2], O_WRONLY)) < 0) {
        printf("Can\'t open output file\n");
        return -1;
    }
    if((fd2 = open("writeChannel", O_RDONLY)) < 0){
        printf("Can\'t open FIFO for reading\n");
        return -1;
    }
    while (true) {
        size = read(fd2, buff, maxSize);
        if (write(x, buff, size) == -1) {
            printf("Can\'t read input file\n");
            return -1;
        }
        if (size != maxSize) {
            break;
        }
    }
    close(fd2);
    return 0;
}

int getAns() {
    int fd1, fd2, size, n = 0;
    char input[maxSize];
    char chars[20000]; // массив встречающихся символов
    int count[20000]; // число вхождений соответствующего символа
    char res[20000]; // строка для вывода
    if ((fd1 = open("readChannel", O_RDONLY)) < 0) {
        printf("Can\'t open FIFO for reading\n");
        return -1;
    }
    while (true) {
        size = read(fd1, input, maxSize);
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
        if (size != maxSize) {
            break;
        }
    }
    close(fd1);
    for (int i = 0; i < n; ++i) {
        char cur[maxSize];
        sprintf(cur, "%c: %d\n", chars[i], count[i]);
        sprintf(res, "%s%s", res, cur);
    }
    if((fd2 = open("writeChannel", O_WRONLY)) < 0){
        printf("Can\'t open FIFO for writting\n");
        return -1;
    }
    int ind = 0;
    size = 0; // считаем длину выходной строки
    while (res[size] != 0) {
        size++;
    }
    while (true) {
        char cur[maxSize];
        int i = 0;
        for (; i < maxSize && ind < size; ++i, ++ind) {
            cur[i] = res[ind];
        }
        write(fd2, cur, i);
        if (ind == size) {
            break;
        }
    }
    close(fd2);
    return 0;
}


int main(int argc, char *argv[]) {

    char input[maxSize];
    mknod("readChannel", S_IFIFO | 0666, 0);
    mknod("writeChannel", S_IFIFO | 0666, 0);

    switch(fork()) {
        case -1:
            perror("fork"); // произошла ошибка
            exit(1); // выход из родительского процесса
        case 0:
            getAns();
            return 0;
        default:
            switch(fork()) {
                case -1:
                    perror("fork"); // произошла ошибка
                    exit(1); // выход из родительского процесса
                case 0:
                    readAndWrite(argc, argv);
                    return 0;
                default:
                    return 0;
            }
    }
}
