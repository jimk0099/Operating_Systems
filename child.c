/**
 * \file child.c
 * \author Aggelos Kolaitis <neoaggelos@gmail.com>
 * \date 2021-03-26
 * \brief execv() example, child process.
 */

#define RED "\033[31m"
#define GREEN "\033[32m"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

char x[100];

void message_open() {
    printf(GREEN "[ID=%s/PID=%d/TIME=Xs] The gates are open!\n", x, getpid());
}

void message_closed() {
    printf(RED "[ID=%s/PID=%d/TIME=Xs] The gates are closed!\n", x, getpid());
}

int main(int argc, char **argv) {
  strcpy(x, argv[1]);
  if (strcmp(argv[2],"t") == 0) {
    signal(SIGUSR1, message_open);
    sleep(10);
    exit(0);
  }
  else {
    signal(SIGUSR1, message_closed);
    sleep(10);
    exit(0);
  }
  return 0;
} 
