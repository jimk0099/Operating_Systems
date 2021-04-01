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

/* Arbitrary buffer size */
#define BUFFER_SIZE 64

/* User read-write, group read, others read */
#define PERMS 0644

int j = 0, w = 0;
pid_t pidtable[100];
char buffer[100];
bool gate_state[100];

void check_neg(int ret, const char *msg) {
  if (ret < 0) {
    perror(msg);
    exit(EXIT_FAILURE);
  }
}

void describe_wait_status(pid_t pid, int status) {
  if (pid < 1) {
    perror("wait() call failed");
  }

  if (pid == 0) {
    printf("Nothing happened");
  }

  if (WIFSTOPPED(status)) {
    printf("Child with PID %d stopped\n", pid);
  } else if (WIFCONTINUED(status)) {
    printf("Child with PID %d continued\n", pid);
  } else if (WIFEXITED(status)) {
    printf("Child with PID %d exited with status code %d\n", pid,
           WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("Child with PID %d terminated by signal %d with status code %d\n",
           pid, WSTOPSIG(status), WEXITSTATUS(status));
  }
}

void forker(int nprocesses) {
    pid_t pid;
    
    if(nprocesses > 0) {
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(-1);
        }
        else if (pid == 0) {
            //Child stuff here
            char str[3];
            sprintf(str, "%d", w);
            if(buffer[w] == 't') {
                char *const argv[] = {"./child", str, "t", NULL};
                int status = execv("./child", argv);
                check_neg(status, "Failed to create child");
            }
            else {
                char *const argv[] = {"./child", str, "f", NULL};
                int status = execv("./child", argv);
                check_neg(status, "Failed to create child");
            }
        }
        else if (pid > 0) {
            //Parent Code
            pidtable[w] = pid;

            if (buffer[w] == 't') {
              gate_state[w] = true;
            }
            else {
              gate_state[w] = false;
            }

            sleep(1);

            if (gate_state[w]) {
              printf(MAGENTA "[PARENT/PID=%d] Created child %d (PID=%d) and initial state 't'\n", getpid(), w, pidtable[w]);
            }
            else {
            printf(MAGENTA "[PARENT/PID=%d] Created child %d (PID=%d) and initial state 'f'\n", getpid(), w, pidtable[w]);
            }

            w++;
            forker(nprocesses - 1);
        }
    }
}

int main(int argc, char *argv[]) {

    //CHECK ARGUMENTS_________________________________________________________________________________________
    
    if (argc != 2) {
        perror("too many arguments or not enough arguments");
        exit(-1);
    }
    buffer[strlen(argv[1])];
    j = snprintf(buffer, strlen(argv[1])+1, "%s\n", argv[1]);
    //printf("%s\ncount = %d\n", buffer, j-1);                                                                        //prints argv string & number of children                                                      
    for (int i = 0; buffer[i] != '\0'; i++) {
        if ((buffer[i] == 't' || buffer[i] != 'f') && (buffer[i] != 't' || buffer[i] == 'f')) {
            perror("wrong arguments");
            exit(-1);
        }
    }

    //___________________________________________________________________________________________________________

    forker(j-1);

    for (int i = 0; i < j-1; i++) {
      kill(pidtable[i], SIGUSR1);
      sleep(1);
    }

    for (int i = 0; i < j-1; i++) {
      wait(NULL);
    }
    
    return 0;
}