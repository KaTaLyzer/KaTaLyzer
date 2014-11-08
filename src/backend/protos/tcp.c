
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "../katalyzer.h"
#include "tcp.h"

int parse_tcp(struct main_struct* mains)
{
        struct tcphdr *tcp;
        int positin;
        
        tcp = (struct tcphdr*) mains->actual_pointer;

        positin = mains->position_number++;
        mains->set_protocol = set_clasic_protocol;
        mains->protoco_position[positin].type = UDP;
        mains->protoco_position[positin].position = mains->actual_pointer;
                
        SHA1Input(&mains->sha12, (u_int8_t*) &tcp->dest, 2);
        SHA1Input(&mains->sha12, (u_int8_t*) &tcp->source, 2);
        
        SHA1Input(&mains->sha21, (u_int8_t*) &tcp->source, 2);
        SHA1Input(&mains->sha21, (u_int8_t*) &tcp->dest, 2);
        
        mains->next_protocol = NULL;
        
        return 0;
}

static struct tcp_struct *alloc_tcp_struct(void)
{
        struct tcp_struct *tcp;
        
        tcp = malloc(sizeof(struct udp_struct));
        if (tcp == NULL) {
                syslog(LOG_WARNING,"Error malloc struct: %s", strerror(errno));
                return NULL;
        }
        
        return tcp;
}

int proces_tcp_protocol(struct packet_struct *packet, struct tcphdr *tcp)
{
        struct tcp_struct *tcps;
        struct proto_struct *proto;
        
        proto = alloc_proto_struct();
        if (proto == NULL) {
                return -1;
        }
        
        tcps = alloc_tcp_struct();
        if (tcps == NULL) {
                return -1;
        }
        
        tcps->port1 = ntohs(tcp->dest);
        tcps->port2 = ntohs(tcp->source);
        
        proto->type = TCP;
        proto->arg = tcps;
        SLIST_INSERT_HEAD(&packet->proto_head, proto, proto_list);
        
        return 0;
}


