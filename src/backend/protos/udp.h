
#ifndef __UDP_H__
#define __UDP_H__

#include <sys/types.h>

struct main_struct;
struct packet_struct;
struct udp_struct;

struct udp_struct{
        u_int16_t port1;
        u_int16_t port2;
};

extern int parse_udp(struct main_struct *mains);
extern int proces_udp_protocol(struct packet_struct *packet, struct udphdr *udp);

#endif