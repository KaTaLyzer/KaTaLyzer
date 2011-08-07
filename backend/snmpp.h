#ifndef SNMPP
#define SNMPP

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/ip.h>	/*struct iphdr*/
#include <netinet/udp.h>
#include <net/ethernet.h>	/*struct ether_header*/
#include <netinet/in.h>
#include "mysql/mysql.h"
#include "variables.h"

#ifdef SNMP_P
#include <arpa/inet.h>
#include <libtasn1.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/tools.h>


extern void snmp_protokol(const u_char *pkt_data,SNMP_STRUK *sn);
SNMP_STRUK *create_snmp();
void procesing_snmp(SNMP_STRUK s, MYSQL *conn);
#endif

#endif
