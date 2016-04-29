#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "trans_session.h"
#include "tcp_session.h"
#include "common.h"

void *trans_data(void *args) {
    struct client_info *cli_info = (struct client_info *)args;

    int servfd;
    struct sockaddr_in servaddr;
    socklen_t servaddr_len = sizeof(servaddr);
    memset(&servaddr, 0, servaddr_len);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = cli_info->addr;
    servaddr.sin_port = htons(DST_PORT);

    struct in_addr addr_info;
    addr_info.s_addr = cli_info->addr;
    char *ip_buf = inet_ntoa(addr_info);

    if ((servfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket()");
        pthread_exit((void *)-1);
    }

    struct nat_session nat_data;
    memset(&nat_data, 0, sizeof(struct nat_session));
    int ret;

    for (; ; ) {
        pthread_mutex_lock(&cli_info->mutex);
        if (cli_info->flag == 1) {
            pthread_mutex_unlock(&cli_info->mutex);
            if ((ret = sendto(servfd, &nat_data, sizeof(nat_data), 0, (struct sockaddr *)&servaddr, servaddr_len)) == sizeof(nat_data)) {
                printf("send %d bytes to %s:%d ok!\n", ret, ip_buf, DST_PORT); 
                sleep(3);
            }
        } else if (cli_info->flag == 0) {
            pthread_mutex_unlock(&cli_info->mutex);
            sleep(1);
            printf("%s:%d send over.\n", ip_buf, DST_PORT);
            free(cli_info); //释放线程资源
            pthread_exit(NULL);
        }
    }
}
