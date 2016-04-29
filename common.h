#include <stdint.h>
#include <time.h>

struct nat_session {
    uint32_t remote_ip_; //远程ip
    uint32_t local_ip_; //本地ip
    uint32_t origin_ip_; //来源ip
    uint16_t remote_port_; //远程port
    uint16_t local_port_; //本地port
    uint16_t origin_port_; //来源port
    int16_t proto_; //协议号
    int32_t path_; //
    int32_t client_id_; //
    uint16_t icmp_id_; //icmp的id号
    uint16_t icmp_seq_; //icmp的seq号
    time_t last_; //记录时间
    time_t remove_;
};
