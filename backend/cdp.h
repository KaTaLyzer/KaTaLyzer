#include "variables.h"

#ifdef CDP_P
#include <stdio.h>
#include <sys/types.h>
#include <net/ethernet.h>
#include <mysql/mysql.h>
#include <errno.h>

struct cdph {
        u_int8_t version;
        u_int8_t ttl;
        u_int16_t check;
};

struct address {
        uint8_t protocol;
        uint8_t length;
        uint8_t pprotocol;
        uint16_t adr_len;
        uint32_t adress;
        struct address *p_next;
};

struct cdp_info {
        char *device_id;
        uint32_t number_IP;
        struct address *p_addres;
        char *port_id;
        u_int32_t capabil;
        char *version;
        char *platform;
};

struct cdp_struct {
        int empty;
        u_int8_t ttl;
        char *device_id;
        struct address *p_addres;
        char *port_id;
        u_int32_t capabil;
        char *version;
        char *platform;
        struct cdp_struct *p_next;
};

void cdp_protokol ( const u_char *pkt_data, struct cdp_struct *cdp_st );
void cdp_processing ( struct cdp_struct *cdp_st, MYSQL *conn );

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 0;
