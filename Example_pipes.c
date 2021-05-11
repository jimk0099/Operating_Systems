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
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>

#define DEFAULT "\033[30;1m"
#define RED "\033[31;1m"
#define GREEN "\033[32;1m"
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

/* Arbitrary rd_num size */
#define rd_num_SIZE 64

/* User read-write, group read, others read */
#define PERMS 0644

#define READ 0
#define WRITE 1


int main(int argc, char* argv[]) {
    if (argc != 3)  {
        perror("Wrong arguments");
        exit(-1);
    }
    int numberofchildren = atoi(argv[1]);
    int paragontiko = atoi(argv[2]);
    if (paragontiko > 16) {
        perror("Number is too big");
        exit(-1);
    }
    int pipes[numberofchildren + 1][2], counter, k, rd_num, wr_num;
    pid_t pid;

    //Create pipes
    for (int i = 0; i < numberofchildren + 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("error creating pipes");
            exit(-1);
        }
    }

    //Check arguments
    char str1[10], str2[10];
    strcpy(str1,argv[1]);
    strcpy(str2,argv[2]);
    int j = 0;
    while(str1[j] != '\0') {
        if(isdigit(str1[j]) == 0) {
            perror("not numbers");
            exit(-1);
        }
        j++;
    }
    j = 0;
    while(str2[j] != '\0') {
        if(isdigit(str2[j]) == 0) {
            perror("not numbers");
            exit(-1);
        }
        j++;
    }

    //Check if given number for factorial is 0 or 1
    if (paragontiko == 0 || paragontiko == 1){
        printf("PARENT(%d): Factorial is %d\n", getpid(), 1);
        exit(0);
    }

    //Create children
    printf("Father's Pid = %d\n", getpid());
    for (int i = 0; i < numberofchildren; i++) {
        pid = fork();
        usleep(50000);
        if (pid < 0) {
            perror("Error with fork");
            exit(-1);
        }
        else if (pid == 0) {
        //Child Code
            //printf("CHILD(%d)=%d\n", i, getpid());
            counter = i + 1;
            k = counter;
            if (counter == 1) {
            //Code for first child
                close(pipes[0][WRITE]); //1st child closes father's write
                if (read(pipes[0][READ], &rd_num, sizeof(rd_num)) == -1) { //1st child reads what father sent
                    perror("reading the pipe failed!");
                    exit(-1);
                }
                wr_num = k * rd_num;
                close(pipes[1][READ]);
                if (write(pipes[1][WRITE], &wr_num, sizeof(wr_num)) == -1) {
                    perror("writing in the pipe failed!");
                    exit(-1);
                }
                k = k + numberofchildren; //go to calculate next paragontiko 
            }
            while(k <= paragontiko) { //for next children
                if (counter == 1) {
                    close(pipes[numberofchildren][WRITE]);
                    if (read(pipes[numberofchildren][READ], &rd_num, sizeof(rd_num)) == -1) {
                        perror("reading the pipe failed!");
                        exit(-1);
                    }
                }
                else {
                    close(pipes[counter-1][WRITE]); //Close previous writing end
                    if (read(pipes[counter-1][READ], &rd_num, sizeof(rd_num)) == -1) {
                        perror("reading pipe failed!");
                        exit(-1);
                    }
                }

                //Children Messages
                wr_num = rd_num *k;
                if (counter < numberofchildren) {
				    printf("CHILD(%d) read %d and give %d CHILD(%d)\n", counter-1, rd_num, wr_num, counter);
				}
                else {
				    printf("CHILD(%d) read %d and give %d CHILD(%d)\n", counter-1, rd_num, wr_num, 0);
                }

                //Check if we found the factorial
                if (k == paragontiko) {     //found it
                    printf("CHILD(%d): Factorial is %d\n", counter, wr_num);
                    break;
                }
                else {
                    k = k + numberofchildren;
                    close(pipes[counter][READ]);
                    if (write(pipes[counter][WRITE], &wr_num, sizeof(wr_num)) == -1) {
                        perror("writing in the pipe failed!");
                        exit(-1);
                    }
                }
            }
            exit(0);
        }
    }

    //Father Code
    for (int j = 0; j < numberofchildren + 1; j++) {
        close(pipes[j][READ]);
        if (j != 0) {
            close(pipes[j][WRITE]);
        }
    }
    wr_num = 1;
    printf("Parent sent %d\n", wr_num);
    if (write(pipes[0][WRITE], &wr_num, sizeof(long int)) == -1) {
        perror("writing in the pipe failed!");
        exit(-1);
    }
    close(pipes[0][WRITE]);
    for (int j = 0; j < numberofchildren; j++) {
        wait(NULL);
    }
    return 0;
}