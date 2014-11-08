/* Copyright (c) 2006 Roman Benkovic
 * edited by Tomas Kovacik (2008)
 * benkovic_roman@post.sk *
 * tokosk16@yahoo.com*/

#ifndef __KATALYZER_H__
#define __KATALYZER_H__

#include <sys/queue.h>
#include <sys/types.h>

#include "sha1.h"

struct proto_struct;
struct packet_struct;
struct eth_struct;

enum proto_types {
        SSL,
        ETH,
        IP,
        TCP,
        UDP,
        _MAX_PROTO_TYPES
};

struct protocol_position_struct {
        enum proto_types type;
        u_int8_t *position;
};

struct main_struct{
        u_int8_t *pkt_data;
        u_int8_t *actual_pointer;
        size_t len;
        int position_number;
        struct protocol_position_struct protoco_position[_MAX_PROTO_TYPES];
        SHA1Context sha12;
        SHA1Context sha21;
        
        
        int (*next_protocol) (struct main_struct *mains);
        int (*set_protocol) (struct main_struct *mans);
        
        SLIST_HEAD(slisthead, packet_struct) packet_head;
        SLIST_HEAD(slisthead, eth_struct) eth_head;
};

inline void init_main_struct(struct main_struct *mains)
{
        mains->position_number = 0;
        mains->next_protocol = NULL;
        mains->set_protocol = NULL;
        
        SLIST_INIT(&mains->packet_head);
        SLIST_INIT(&mains->eth_head);
        
}

struct packet_struct {
        char set;
        u_int64_t hash12;
        u_int64_t hash21;
        unsigned int bytes12;
        unsigned int bytes21;
        SLIST_HEAD(slisthead, proto_struct) proto_head;
        SLIST_ENTRY(packet_struct) packet_list;
};

struct proto_struct {
        enum proto_types type;
        SLIST_ENTRY(proto_struct) proto_list;
        void *arg;
};

extern struct proto_struct *alloc_proto_struct(void);

extern struct packet_struct *get_packet(struct main_struct *mains,
                                 u_int64_t hash12, u_int64_t hash21);
extern int set_clasic_protocol(struct main_struct* mains);


#endif
