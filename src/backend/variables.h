#ifndef __PREMENNE_H__
#define __PREMENNE_H__

#include <config.h>
#include "time.h"
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

#ifndef _PCAP
#include "socket/ksocket.h"
#else
#include <pcap.h>
#endif

//#define CDP_P
//#define SNMP_P
//#define _SIP
//#define NETFLOW


#define CHCEM_POCTY

#define DLZKA 128
#define NUMBER_OF_TCP_PORTS 20
#define NUMBER_OF_UDP_PORTS 20
#define IPV6SIZE 4
#define DLZKA_POLA_P 11

#ifdef _DEBUG_WRITE
#define NAME_FILE "dt.txt"
#define NAME_FILE2 "sub.txt"
#define NAME_FILE3 "cronovanie.txt"
#endif

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

//pointer to thread
typedef struct kthread {
        struct kthread *p_next;
        struct kthread *p_previous;
        char run;
        pthread_t zapdb;
}KTHREAD;

extern KTHREAD *p_thread;

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
#ifdef NETFLOW
typedef struct zaciatok_p {
        int empty;
	int cas;
        PROTOKOLY *p_protokoly;
	struct zaciatok_p *p_next;
}ZACIATOK_P;
#else
typedef struct zaciatok_p {
        int empty;
        PROTOKOLY *p_protokoly;
}ZACIATOK_P;
#endif

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

#ifdef CHCEM_POCTY
extern int ppaketov;
extern int pocetpaketov;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////n
// DATA-LINK VRSTVA
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern ZACIATOK_P z_protokoly;
#ifdef NETFLOW
extern ZACIATOK_P *z_pom_protokoly;
#endif
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
extern         char db_name[DLZKA+1];                  // sem sa ulozi nazov databazy
extern         char db_user[DLZKA+1];                  //uzivatel, pod ktorym sa ideme do DB pripajat
extern         char db_pass[DLZKA+1];                  // heslo

extern         int casovac_zapisu;                     // udaj je v sekundach - nacitany z koniguraku



extern         int protocol_eth;                     //bude sa robit aj graf pre ETHERNET? default=1 -> ano; ak je v konfigu =0, zakaze sa tento graf
extern         int protocol_8023;                    // povolenie grafu pre 802.3....... pod aj ostatne tieto premenne
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

#ifdef _PCAP
extern pcap_t *fp;
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//      premenne pre NetFlow a sFlow meranie
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NETFLOW
extern int flow_pocet_B;
extern uint64_t flow_MAC_adr_S;
extern uint64_t flow_MAC_adr_D;
extern uint32_t flow_IP_adr_S;
extern uint32_t flow_IP_adr_D;
extern unsigned int *flow_IPV6_adr_S;
extern unsigned int *flow_IPV6_adr_D;
extern int flow_pocet_paketov;
extern int flow_protocol;
extern int flow_src_port;
extern int flow_dst_port;
extern uint32_t exporter_IP;
extern int template_timeout;
extern int is_ip_configured;
extern int flow_capttime;
extern int flow_flag;


typedef struct nflow9_template {
	unsigned int source_ID;
	unsigned int template_ID;
	int ip_s_pos;
	int ip_s_len;
	int ip_d_pos;
	int ip_d_len;
	int ipv6_s_pos;
	int ipv6_s_len;
	int ipv6_d_pos;
	int ipv6_d_len;
	int mac_s_pos;
	int mac_s_len;
	int mac_d_pos;
	int mac_d_len;
	int pocet_B_pos;
	int pocet_B_len;
	int pocet_ramcov_pos;
	int pocet_ramcov_len;
        int protocol_pos;
	int protocol_len;
        int src_port_pos;
	int src_port_len;
        int dst_port_pos;
	int dst_port_len;
	int offset;
	time_t add_time;
	struct nflow9_template *next;
} NFLOW9_TEMPLATE;
     
typedef struct nflow9_buffer {
    unsigned int ID;
    int flowset_len;
    int capture_time;
    int database_time;
    u_char *datagram;
    struct nflow9_buffer *next;
} NFLOW9_BUFFER;
     
extern NFLOW9_TEMPLATE *n9_tmpl;
extern NFLOW9_TEMPLATE *n9_tmpl_first;
extern NFLOW9_TEMPLATE *novy;
extern NFLOW9_TEMPLATE *n9_tmpl_akt;
extern NFLOW9_TEMPLATE *selected_tmpl;
extern NFLOW9_BUFFER *n9_buf_akt;
extern NFLOW9_BUFFER *n9_buf_first;
extern NFLOW9_BUFFER *n9_buf_pom;
#endif

#ifdef SNMP_P
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////      premenne pre SNMP
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern  int protocol_snmp;

typedef struct snmp_struk {
        int empty;
        int bytes;
        int number;
        uint32_t IP_S;
        uint32_t IP_D;
        char *p_snmp;
        struct snmp_struk *p_next;
} SNMP_STRUK;

extern SNMP_STRUK sn;
#endif

//SIP

typedef struct sipmsg {
        int time;
        int type;
        int ip_s[4];
        int ip_d[4];
        int comm;
        char name[1500];
        char *callid;
        int pridany;
        struct sipmsg *next;
} SIPMSG;

extern SIPMSG *sipmsgs;
extern SIPMSG *first;
extern int protocol_sip;

#endif
