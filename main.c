#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "probes.h" 
#include "tcp_session.h"
#include "trans_session.h"

int main() {
    int fd = tcp_init();
    if (fd == -1) {
        printf("TCP session init failed.\n");
        exit(-1);
    } 
    pthread_t tcp_trd;
    pthread_create(&tcp_trd, NULL, tcp_accept, (void *)&fd);

    sleep(10000);
    return 0;
}
