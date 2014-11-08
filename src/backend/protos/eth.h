
#ifndef __ETH_H__
#define __ETH_H__

#include <sys/types.h>
#include <sys/queue.h>

#define ETH_LEN 6

struct main_struct;
struct packet_struct;
struct ether_header;

struct eth_struct{
        u_int8_t mac1[ETH_LEN];
        u_int8_t mac2[ETH_LEN];
        SLIST_ENTRY(eth_struct) eth_list;
};

extern int parse_ethernet(struct main_struct *mains);
extern int proces_ethernet_struct(struct packet_struct *packet, struct ether_header *eth);

#endif
