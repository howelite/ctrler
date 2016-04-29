#ifndef TCP_SESSION_H 
#define TCP_SESSION_H 

#include <stdint.h>
#include <pthread.h>
#define LISTEN_PORT 12345
#define SERV_ADDR "10.0.0.18"
#define LISTENQ 5

enum probe_mode {
    PASSIVE,
    ACTIVE
};

struct client_info {
    int fd;
    enum probe_mode mode;
    int flag;
    uint32_t addr;
    uint16_t port;
    pthread_mutex_t mutex;
};

int tcp_init(void);
void *tcp_accept(void *arg);

#endif
