/* Copyright (c) 2006 Roman Benkovic
 * modified by Tomas Kovacik  & Stanislav Bocinec (2008) 
 * modified by Roman Bronis & Andrej Kozemcak (2009)
 * modified by Martin Hyben (2010)
 * benkovic_roman@post.sk *
 * tokosk16@yahoo.com *
 * svacko@gmail.com *
 * roman.bronis@gmail.com *
 * akozemcak@gmail.com *
 * hyben.martin@gmail.com *
*/

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/queue.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "protos/eth.h"
#include "protos/ip.h"
#include "protos/tcp.h"
#include "protos/udp.h"
#include "katalyzer.h"


int l2p = -1;

int main(int argc, char **argv)
{
        struct main_struct mains;
        
        openlog("KaTaLyzer", 0, LOG_DAEMON);
       
        init_main_struct(&mains);
        
        
        
}

void dispatcher(struct main_struct *mains)
{
        char *pkt_data;
        int pkt_length;
        
        SHA1Reset(&mains->sha12);
        SHA1Reset(&mains->sha21);
        
        mains->next_protocol = parse_ethernet;
        
        while (mains->next_protocol) {
                mains->next_protocol(mains);
        }
        
        if (mains->set_protocol == NULL) {
                return;
        }
        
        mains->set_protocol(mains);
        
}

struct proto_struct *alloc_proto_struct(void)
{
        struct proto_struct *proto;
        
        proto = malloc(sizeof(struct proto_struct));
        if (proto == NULL) {
                syslog(LOG_WARNING, "Error alloc memory: %s", strerror(errno));
                return NULL;
        }
        
        return proto;
}

struct packet_struct *get_packet(struct main_struct *mains,
                                 u_int64_t hash12, u_int64_t hash21)
{
        struct packet_struct *packet1, *packet2;
        
        packet1 = NULL;
        SLIST_FOREACH(packet1, &mains->packet_head, proto_head) {
                if (packet1->hash12 == hash12 || packet1->hash12 == hash21 ||
                        packet1->hash21 == hash12 || packet1->hash21 == hash21)
                {
                        return packet1;
                }
        }
        
        packet2 = malloc(sizeof(struct packet_struct));
        if (packet2 == NULL) {
                syslog(LOG_WARNING, "Cannot alloc memory: %s", strerror(errno));
                return NULL;
        }
        
        packet2->set = 0;
        packet2->bytes12 = 0;
        packet2->bytes21 = 0;
        packet2->hash12 = hash12;
        packet2->bytes21 = hash21;
        
        SLIST_INIT(packet->proto_head);
        
        if (packet1) {
                SLIST_INSERT_AFTER(packet1, packet2, packet_list);
        } else {
                SLIST_INSERT_HEAD(&mains->packet_head, packet, packet_list);
        }
        
}

int set_clasic_protocol(struct main_struct* mains)
{
        struct ether_header *ether;
        struct iphdr *ip;
        struct udphdr *udp;
        struct tcphdr *tcp;
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
                                
                        case TCP:
                                tcp = (struct tcphdr*)
                                mains->protoco_position[i].position;
                                val = proces_tcp_protocol(packet, tcp);
                                break;
                }
                
                if (val < 0) {
                        return -1;
                }
        }
        
        packet->set = 1;

        return 0;
}
