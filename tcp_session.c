#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "tcp_session.h"
#include "probes.h"
#include "trans_session.h"

int tcp_init(void) {
    int servfd;
    struct sockaddr_in servaddr;
    socklen_t servaddr_len = sizeof(servaddr);
    memset(&servaddr, 0, servaddr_len);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(LISTEN_PORT);
    struct in_addr netseq_servaddr;
    inet_aton(SERV_ADDR, &netseq_servaddr);
    memcpy(&servaddr.sin_addr, &netseq_servaddr, sizeof(struct in_addr));

    if ((servfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        return -1;
    }

    if ((bind(servfd, (struct sockaddr *)&servaddr, servaddr_len)) == -1) {
        perror("bind()");
        return -1;
    }

    if ((listen(servfd, LISTENQ)) == -1) {
        perror("listen()");
        return -1;
    }

    printf("server listen on %s:%d.\n", SERV_ADDR, LISTEN_PORT);
    return servfd;
}

void *tcp_accept(void *arg) {
    int tcp_fd = *(int *)arg;
    int clifd, ret;
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len = sizeof(cliaddr);
    memset(&cliaddr, 0, cliaddr_len);

    for (; ; ) {
        clifd = accept(tcp_fd, (struct sockaddr *)&cliaddr, &cliaddr_len);
        if (clifd == -1) {
            perror("accept()");
            continue;
        }

        pthread_t probe_trd, trans_trd;
        pthread_mutex_t mutex;
        pthread_mutex_init(&mutex, NULL);

        struct client_info *cli_info = (struct client_info *)malloc(sizeof(struct client_info));
        memset(cli_info, 0, sizeof(struct client_info));
        cli_info->fd = clifd;
        cli_info->mode = PASSIVE;
        cli_info->addr = cliaddr.sin_addr.s_addr;
        cli_info->port = ntohs(cliaddr.sin_port);
        cli_info->flag = 1;
        cli_info->mutex = mutex;

        if ((ret = pthread_create(&probe_trd, NULL, probes_start, (void *)cli_info)) != 0) {
            perror("probes_start: pthread_create()");
            exit(-1);
        }

        if ((ret = pthread_create(&trans_trd, NULL, trans_data, (void *)cli_info)) != 0) {
            perror("trans_data: pthread_create()");
            exit(-1);
        }
        
    }
}
