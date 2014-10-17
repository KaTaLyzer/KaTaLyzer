#ifndef __PREMENNE_H__
#define __PREMENNE_H__

//#include <config.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip6.h>
#include "mysql/mysql.h"

#include "ksocket.h"
#include <pcap.h>


#define DLZKA 128
#define NUMBER_OF_TCP_PORTS 20
#define NUMBER_OF_UDP_PORTS 20
#define IPV6SIZE 4
#define DLZKA_POLA_P 11

extern char isoffline;
extern char isfirsttime;

extern int wait;
extern char *interface_from_argument;
//extern char interface_from_argument[25];
extern int b_interface_from_argument;
extern char is_ipv6ext;

// debug variables
extern int debug;
extern int i_dbg_level;
extern char log_name[DLZKA+1];
extern char *s_tmp_log;
extern char *s_tmp_str;
extern int totalog;

//config variables
extern int i_is_configured;
extern int b_is_iface;

//global structure for IP adress and MAC addres
typedef struct zaznamy {
        struct zaznamy *p_next;		// pointer to next structure
        uint64_t mac_s;
        uint64_t mac_d;
        uint32_t ip_s;
        uint32_t ip_d;
        unsigned int *ipv6_s;
        unsigned int *ipv6_d;
        unsigned long int pocet_B;
        unsigned long int pocet_ramcov;
        int spracovany[4];
}ZAZNAMY;

//global structure for name of the protocol
typedef struct protokoly {
        int empty;
        char is_ipv6;
        char protokol[9];		// name of the protocol
        ZAZNAMY *zoznam;		// pointer to stucture ZAZNAMY
        struct protokoly *p_next;	// pointer to next structure
} PROTOKOLY;

//help structure
typedef struct zaciatok_p {
        int empty;
        PROTOKOLY *p_protokoly;
}ZACIATOK_P;


//global structure for pair array
typedef struct pair_array {
        int empty;
        char is_ipv6;
        uint64_t mac_s;
        uint64_t mac_d;
        uint32_t ip_s;
        uint32_t ip_d;
        unsigned int *ipv6_s;
        unsigned int *ipv6_d;
        struct pair_array *p_next;
}PAIR_ARRAY;


//global definitions for update to mysql database
extern int unix_time;
extern int processing_time;
extern time_t actual_time;
extern int pocet_B;
extern char Eor8[2];
extern uint64_t MAC_adr_S;
extern uint64_t MAC_adr_D;
extern char net_proto[10];
extern uint32_t IP_adr_S;
extern uint32_t IP_adr_D;
extern unsigned int *IPV6_adr_S;
extern unsigned int *IPV6_adr_D;
extern char trans_layer[10];
extern int protocol;
extern int s_protocol;
extern int d_protocol;
extern int interval;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////n
// DATA-LINK VRSTVA
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern ZACIATOK_P z_protokoly;
extern PAIR_ARRAY *z_pair_array;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PAROVACIE UDAJE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned int parovacie_pole__IP_adresa[500];
extern char parovacie_pole__MAC_adresa[500][13];
extern int velkost_parovacieho_pola; // je to pocet vsetkych doteraz so mnou komunikujucich IP adries

// flag - premenna, ktorou sa kontroluje ci som uz zapisal v danom intervale data do DB alebo este len cakaju
extern int flag;  // ak FALSE - data zapisane este neboli

extern time_t beggining_time;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//      premenne pre nacitanie konfiguraku
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern 	       FILE *f_config;                         // pointer pre konfiguracny subor
extern         char *config_name;			//nazov konfiguracneho suboru
extern         char str[256];                          // str - retazec pre ukladanie riadkov z konfiguraku
extern         char * pch;                             // pch - pomocny pointer na retaze, kt. potrebujem pre rozdelenie riadka podla =, napr: TCP_PORT=80 - aby som vedel vyextrahovat tu 80-tku

extern char interface[DLZKA+1];	// variable for saving interface which we will be measuring on

extern         char db_host[DLZKA+1];                  // sem sa ulozi server, na kt. sa ideme pripajat
extern         int db_port;		       // port na server
extern         char db_name[DLZKA+1];                  // sem sa ulozi nazov databazy
extern         char db_user[DLZKA+1];                  //uzivatel, pod ktorym sa ideme do DB pripajat
extern         char db_pass[DLZKA+1];                  // heslo

extern         int casovac_zapisu;                     // udaj je v sekundach - nacitany z koniguraku



extern         int protocol_eth;                     //bude sa robit aj graf pre ETHERNET? default=1 -> ano; ak je v konfigu =0, zakaze sa tento graf
extern         int protocol_8023;                    // povolenie grafu pre 802.3....... pod aj ostatne tieto premenne
extern         int protocol_sll;
extern         int protocol_arp;
extern         int protocol_rarp;
extern 	       int protocol_ip;
extern 	       int protocol_ipv6;
extern         int protocol_igmp;
extern         int protocol_icmp;
extern         int protocol_ipx;
extern         int protocol_tcp;
extern         int protocol_udp;

extern         int pole_TCP_portov[20];                // sem nacitam cisla portov, pre ktore sa budu robit grafy - TCP a UDP
extern         int i_TCP;                            // inkrementacna premenna pre posuvanie sa v poli TCP portov
extern         int pole_UDP_portov[20];
extern         int i_UDP;

extern struct ether_header *ethh; //ethernetova header
extern struct iphdr *iph;       //ip header
extern struct ip6_hdr *iphv6;	//ip_6 header
extern struct tcphdr *tcph;     //tcp header
extern struct udphdr *udph;     //udp header
extern struct arphdr *arph;	//arp header

extern pcap_t *fp;

#endif
