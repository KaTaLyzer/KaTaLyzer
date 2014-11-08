
#include <netinet/ip.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#include "../katalyzer.h"
#include "udp.h"
#include "tcp.h"
#include "ip.h"

int parse_ip(struct main_struct* mains)
{
        struct iphdr *ip_header;
        int position;
        
        ip_header = (struct iphdr*) mains->actual_pointer;
        
        position = mains->position_number++;
        mains->set_protocol = set_clasic_protocol;
        mains->protoco_position[position].type = IP;
        mains->protoco_position[position].position = mains->actual_pointer;
        
        SHA1Input(mains->sha12, (u_int8_t*) &ip_header->daddr, 4);
        SHA1Input(mains->sha12, (u_int8_t*) &ip_header->saddr, 4);
        
        SHA1Input(mains->sha21, (u_int8_t*) &ip_header->saddr, 4);
        SHA1Input(mains->sha21, (u_int8_t*) &ip_header->daddr, 4);
        
        mains->actual_pointer += ip_header->ihl * 4;
        
        switch (ip_header->protocol) {
                case IPPROTO_TCP:
                        mains->next_protocol = parse_tcp;
                        break;
                        
                case IPPROTO_UDP:
                        mains->next_protocol = parse_udp;
                        break;
                        
                default:
                        mains->next_protocol = NULL;
                        break;
        }
        
        return 0;
}

static struct ip_struct *alloc_ip_struct(void)
{
        struct ip_struct *ip;
        
        ip = malloc(sizeof(struct ip_struct));
        if (ip == NULL) {
                syslog(LOG_WARNING, "Error malloc struct: %s", strerror(errno));
                return NULL;
        }
        
        return ip;
}

int proces_ip_struct(struct packet_struct *packet, struct iphdr *ip)
{
        struct proto_struct *proto;
        struct ip_struct *ips;
        
        proto = alloc_proto_struct();
        if (proto == NULL) {
                return -1;
        }
        
        ips = alloc_ip_struct();
        if (ips == NULL) {
               return -1;
        }
        
        ips->ip1 = ntohl(ip->daddr);
        ips->ip2 = ntohl(ip->saddr);
        
        proto->type = IP;
        proto->arg = ips;
        SLIST_INSERT_HEAD(&packet->proto_head, proto, proto_list);
        
        return 0;
}

