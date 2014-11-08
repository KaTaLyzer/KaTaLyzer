
#ifndef __TCP_H__
#define __TCP_H__

#include <sys/types.h>

struct main_struct;
struct packet_struct;
struct tcphdr;

struct tcp_struct{
        u_int16_t port1;
        u_int16_t port2;
};

extern int parse_tcp(struct main_struct *mains);
extern int proces_tcp_protocol(struct packet_struct *packet, struct tcphdr *tcp);

#endif