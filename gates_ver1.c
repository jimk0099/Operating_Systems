#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>

#define DEFAULT "\033[30;1m"
#define RED "\033[31;1m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define GRAY "\033[38;1m"

/* File Descriptors for stdin and stdout */
#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2

/* Arbitrary buffer size */
#define BUFFER_SIZE 64

/* User read-write, group read, others read */
#define PERMS 0644

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("too many arguments");
        exit(-1);
    }
    char buffer[strlen(argv[1])];
    int j = snprintf(buffer, strlen(argv[1])+1, "%s\n", argv[1]);
    printf("%s\ncount = %d\n", buffer, j-1);
    for (int i = 0; buffer[i] != '\0'; i++) {
        if ((buffer[i] == 't' || buffer[i] != 'f') && (buffer[i] != 't' || buffer[i] == 'f')) {
            perror("wrong arguments");
            exit(-1);
        }
    }
    return 0;
}