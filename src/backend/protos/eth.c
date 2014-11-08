

#include <sys/queue.h>
#include <net/ethernet.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#include "../katalyzer.h"
#include "ip.h"
#include "eth.h"


int parse_ethernet(struct main_struct *mains)
{
        struct ether_header *eth;
        int position;
        
        eth = (struct ether_header *) mains->actual_pointer;
        
        position = mains->position_number++;
        mains->set_protocol = set_clasic_protocol;
        mains->protoco_position[position].type = ETH;
        mains->protoco_position[position].position = mains->actual_pointer;
        
        SHA1Input(mains->sha12, eth->ether_dhost, 6);
        SHA1Input(mains->sha12, eth->ether_shost, 6);
        
        SHA1Input(mains->sha21, eth->ether_shost, 6);
        SHA1Input(mains->sha21, eth->ether_dhost, 6);
        
        mains->actual_pointer += sizeof(struct ether_header);
        
        switch (eth->ether_type) {
                case ETHERTYPE_IP:
                        mains->next_protocol = parse_ip;
                        break;
                        
                default:
                        mains->next_protocol = NULL;
                        break;
        }
        
        return 0;
}

static struct eth_struct *alloc_eth_struct(void)
{
        struct eth_struct *eth;
        
        eth = malloc(sizeof(struct eth_struct));
        if (eth == NULL) {
                syslog(LOG_WARNING, "Error malloc struct: %s", strerror(errno));
                return NULL;
        }
        
        return eth;
}

int proces_ethernet_struct(struct packet_struct *packet, struct ether_header *eth)
{
        struct eth_struct *eths;
        struct proto_struct *proto;
        
        proto = alloc_proto_struct();
        if (proto == NULL) {
                return -1;
        }
        
        eths = alloc_eth_struct();
        if (eths == NULL) {
                return -1;
        }
        
        memcpy(eths->mac1, eth->ether_dhost, ETH_LEN);
        memcpy(eths->mac2, eth->ether_shost, ETH_LEN);
        
        proto->type = ETH;
        proto->arg = eths;
        SLIST_INSERT_HEAD(&packet->proto_head, proto, proto_list);
        
        return 0;
}