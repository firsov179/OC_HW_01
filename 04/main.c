#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <search.h>

const int maxSize = 20000;

int main(int argc, char *argv[]) {

    int fd1;
    int fd2;
    char input[maxSize];
    mknod("readChannel", S_IFIFO | 0666, 0);
    mknod("writeChannel", S_IFIFO | 0666, 0);
    pid_t pid;
    int size, x;
    char chars[maxSize]; // массив встречающихся символов
    int count[maxSize]; // число вхождений соответствующего символа
    char res[maxSize]; // строка для вывода
    char buff[maxSize];
    int n = 0;
    switch(pid=fork()) {
        case -1:
            perror("fork"); /* произошла ошибка */
            exit(1); /*выход из родительского процесса*/
        case 0:
            if ((x = open(argv[1], O_RDONLY)) < 0) {
                printf("Can\'t open input file\n");
                exit(-1);
            }
            if((fd1 = open("readChannel", O_WRONLY)) < 0){
                printf("Can\'t open FIFO for writting\n");
                exit(-1);
            }
            size = read(x, input, maxSize);
            if (size == -1) {
                printf("Can\'t read input file\n");
                exit(-1);
            }
            size = write(fd1, input, size);
            close(fd1);
            // Первый поток выводит ответ в файл
            // Имя файлв - 1-й аргумент командной стороки
            if ((x = open(argv[2], O_WRONLY)) < 0) {
                printf("Can\'t open output file\n");
                exit(-1);
            }
            if((fd2 = open("writeChannel", O_RDONLY)) < 0){
                printf("Can\'t open FIFO for reading\n");
                exit(-1);
            }
            size = read(fd2, buff, maxSize);
            if (write(x, buff, size) == -1) {
                printf("Can\'t read input file\n");
                exit(-1);
            }
            close(fd2);
            exit(0);
        default:
            if((fd1 = open("readChannel", O_RDONLY)) < 0){
                printf("Can\'t open FIFO for reading\n");
                exit(-1);
            }
            size = read(fd1, input, maxSize);
            close(fd1);
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
            if((fd2 = open("writeChannel", O_WRONLY)) < 0){
                printf("Can\'t open FIFO for writting\n");
                exit(-1);
            }
            size = write(fd2, res, size);
            close(fd2);
    }
    return 0;
}
