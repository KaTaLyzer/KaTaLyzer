
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include "../katalyzer.h"
#include "eth.h"
#include "ip.h"
#include "udp.h"

int parse_udp(struct main_struct* mains)
{
        struct udphdr *udp;
        int positin;
        
        udp = (struct udphdr*) mains->actual_pointer;

        positin = mains->position_number++;
        mains->set_protocol = set_clasic_protocol;
        mains->protoco_position[positin].type = UDP;
        mains->protoco_position[positin].position = mains->actual_pointer;
        
        SHA1Input(mains->sha12, (u_int8_t*) &udp->dest, 2);
        SHA1Input(mains->sha12, (u_int8_t*) &udp->source, 2);
        
        SHA1Input(mains->sha21, (u_int8_t*) &udp->source, 2);
        SHA1Input(mains->sha21, (u_int8_t*) &udp->dest, 2);
        
        mains->next_protocol = NULL;
        
        
        return 0;
}

static struct udp_struct *alloc_udp_struct(void)
{
        struct udp_struct *udp;
        
        udp = malloc(sizeof(struct udp_struct));
        if (udp == NULL) {
                syslog(LOG_WARNING,"Error malloc struct: %s", strerror(errno));
                return NULL;
        }
        
        return udp;
}

int proces_udp_protocol(struct packet_struct *packet, struct udphdr *udp)
{
        struct udp_struct *udps;
        struct proto_struct *proto;
        
        proto = alloc_proto_struct();
        if (proto == NULL) {
                return -1;
        }
        
        udps = alloc_udp_struct();
        if (udps == NULL) {
                return -1;
        }
        
        udps->port1 = ntohs(udp->dest);
        udps->port2 = ntohs(udp->source);
        
        proto->type = UDP;
        proto->arg = udps;
        SLIST_INSERT_HEAD(&packet->proto_head, proto, proto_list);
        
        return 0;
}

int set_udp_protocol(struct main_struct* mains)
{
        struct ether_header *ether;
        struct iphdr *ip;
        struct udphdr *udp;
        struct packet_struct *packet;
        uint64_t *hash12, *hash21;
        int i, val;
        
        if (SHA1Result(&mains->sha12) == 0) {
                return -1;
        }
        
        hash12 = (uint64_t*) mains->sha12.Message_Digest;
        
        if (SHA1Result(&mains->sha21) == 0) {
                return -1;
        }
        
        hash21 = (uint64_t*) mains->sha21.Message_Digest;
        
        packet = get_packet(mains, *hash12, *hash21);
        if (packet == NULL) {
                return -1;
        }
        
        if (packet->hash12 == *hash12) {
                packet->bytes12 += mains->len;
        } else {
                packet->bytes21 += mains->len;
        }
        
        if (packet->set) {
                return 0;
        }
                
        for (i = 0; i < mains->position_number; i++) {
                switch (mains->protoco_position[i]) {
                        case ETH:
                                ether = (struct ether_header*)
                                mains->protoco_position[i].position;
                                val = proces_ethernet_struct(packet, ether);
                                break;
                                
                        case IP:
                                ip = (struct iphdr*)
                                mains->protoco_position[i].position;
                                val = proces_ip_struct(packet, ip);
                                break;
                                
                        case UDP:
                                udp = (struct udphdr*)
                                mains->protoco_position[i].position;
                                val = proces_udp_protocol(packet, udp);
                                break;
                }
                
                if (val < 0) {
                        return -1;
                }
        }
        
        packet->set = 1;

        return 0;
}


