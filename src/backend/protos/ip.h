
#ifndef __IP_H__
#define __IP_H__

#include <sys/types.h>

struct main_struct;
struct packet_struct;
struct iphdr;

struct ip_struct{
        u_int32_t ip1;
        u_int32_t ip2;
};

extern int parse_ip(struct main_struct *mains);
extern int proces_ip_struct(struct packet_struct *packet, struct iphdr *ip);

#endif
