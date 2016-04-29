#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "probes.h" 
#include "tcp_session.h"

static void passive_handler(struct client_info *cli_info) {
    int ret;
    int tcp_fd = cli_info->fd;
    fd_set fds;
    struct timeval timer;
    timer.tv_sec = 10;
    timer.tv_usec = 0;

    struct in_addr addr_info;
    addr_info.s_addr = cli_info->addr;
    char *ip_buf = inet_ntoa(addr_info);

    printf("%s:%d has connected.\n", ip_buf, cli_info->port);

    char buffer[BUFFSIZE];
    for (; ; ) { 
        FD_ZERO(&fds);
        FD_SET(tcp_fd, &fds);
        int maxfd = tcp_fd + 1;
        struct timeval timer_;
        memcpy(&timer_, &timer, sizeof(struct timeval));

        ret = select(maxfd, &fds, NULL, NULL, &timer_);
        if (ret == -1) {
            perror("select()");
            exit(1);
        } else if (ret == 0) {
            pthread_mutex_lock(&cli_info->mutex);
            cli_info->flag = 0;
            pthread_mutex_unlock(&cli_info->mutex);
            printf("time out!client %s:%d died.\n", ip_buf, cli_info->port);
            close(tcp_fd);
            break;
        } else if (ret == 1) { 
            if (read(tcp_fd, buffer, sizeof(buffer)) > 0)
                printf("%s:%d still alive.\n", ip_buf, cli_info->port);
            else {
                pthread_mutex_lock(&cli_info->mutex);
                cli_info->flag = 0;
                pthread_mutex_unlock(&cli_info->mutex);
                printf("%s:%d client died.\n", ip_buf, cli_info->port);
                close(tcp_fd);
                break;
            }
        } else {
            printf("unexpected error\n");
        }
    }
}

static void active_handler(struct client_info *cli_info) {
    char send_buffer[] = "hello";
    long len = strlen(send_buffer);
    for (; ; ) {
        if (write(cli_info->fd, send_buffer, len) != len) {
            continue;
        }
        sleep(30);
    }
}

void *probes_start(void *args) {
    struct client_info *ptr_client_info = (struct client_info *)args;
    switch (ptr_client_info->mode) {
        case PASSIVE:
            passive_handler(ptr_client_info);
            break;
        case ACTIVE:
            active_handler(ptr_client_info);
            break;
    }
    pthread_exit(NULL);
}
