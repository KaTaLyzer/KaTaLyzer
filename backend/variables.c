#include "variables.h"

int wait = 1;
char *interface_from_argument;
//char *interface_from_argument[25]="";
int b_interface_from_argument = 0;

//debug variables
int debug = 0;
int i_dbg_level = 1;
char log_name[DLZKA+1] = "/var/log/katalyzer";
char *s_tmp_log;
char *s_tmp_str;
//int totalog = 0;

//configuration variables
int i_is_configured = 0;
int b_is_iface;

//global definitions for update to mysql database
int unix_time;
int processing_time; 
time_t actual_time;
int pocet_B=0;
char Eor8[2];
uint64_t MAC_adr_S;
uint64_t  MAC_adr_D;
char net_proto[10];
uint32_t IP_adr_S;
uint32_t IP_adr_D;
char *IPV6_adr_S;
char *IPV6_adr_D;
char TCP_UDP[10];
//char protocol[35];
int protocol=0;
int s_protocol=0;
int d_protocol=0;
int interval=0;
int ppaketov=0;

/////////////////////////////////////////////////////////////////////////
// arrays where numbers of Bytes and frames are storred for each protocol
/////////////////////////////////////////////////////////////////////////

ZACIATOK_P z_protokoly;
PAIR_ARRAY *z_pair_array;

///////////////
// PAIRING DATA
///////////////

unsigned int parovacie_pole__IP_adresa[500];
char parovacie_pole__MAC_adresa[500][13];
int velkost_parovacieho_pola=0; // je to pocet vsetkych doteraz so mnou komunikujucich IP adries

/*
std::map <uint32_t, uint64_t> pair_array_S;
std::map <uint32_t, uint64_t> pair_array_D;
*/

// flag - premenna, ktorou sa kontroluje ci som uz zapisal v danom intervale data do DB alebo este len cakaju
int flag = 0;  // ak FALSE - data zapisane este neboli

// beggining_time - time of beginning of measuring the traffic - is changed after data is written into database by adding value of casovac_zapisu to previous value of beggining_time; casovac_zapisu is read from config file
time_t beggining_time;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//      premenne pre nacitanie konfiguraku
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FILE *f_config;                         // pointer pre konfiguracny subor
char *config_name = "my_config.conf";	//nazov konfiguracneho suboru
char str[256];                          // str - retazec pre ukladanie riadkov z konfiguraku
char * pch;                             // pch - pomocny pointer na retaze, kt. potrebujem pre rozdelenie riadka podla =, napr: TCP_PORT=80 - aby som vedel vyextrahovat tu 80-tku

char interface[DLZKA+1]; 		// variable for saving interface which we will be measuring on

char db_host[DLZKA+1];                  // sem sa ulozi server, na kt. sa ideme pripajat
char db_name[DLZKA+1];                  // sem sa ulozi nazov databazy
char db_user[DLZKA+1];                  //uzivatel, pod ktorym sa ideme do DB pripajat
char db_pass[DLZKA+1];                  // heslo
int casovac_zapisu;                     // udaj je v sekundach - nacitany z koniguraku


int protocol_eth=1;                     //bude sa robit aj graf pre ETHERNET? default=1 -> ano; ak je v konfigu =0, zakaze sa tento graf
int protocol_8023=1;                    // povolenie grafu pre 802.3....... pod aj ostatne tieto premenne
int protocol_arp=1;
int protocol_rarp=1;
int protocol_ip=1;
//int protocol_ipv6=1;
int protocol_igmp=1;
int protocol_icmp=1;
int protocol_ipx=1;
int protocol_tcp=1;
int protocol_udp=1;

int pole_TCP_portov[20];                // sem nacitam cisla portov, pre ktore sa budu robit grafy - TCP a UDP
int i_TCP=0;                            // inkrementacna premenna pre posuvanie sa v poli TCP portov
int pole_UDP_portov[20];
int i_UDP=0;

struct ether_header *ethh; //ethernetova hlavicka
struct iphdr *iph;      //ip hlavicka
struct ip6_hdr *iphv6;	//ip_6 hlavicka
struct tcphdr *tcph;     //tcp hlavicka
struct udphdr *udph;    //udp hlavicka
struct arphdr *arph;	//arp header


pcap_t *fp;

#ifdef SNMP_P
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////      premenne pre SNMP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int protocol_snmp=0;			//ak nezachyti, ziandnu snmp komunikaciu tak nevytvori snmp tabluku
SNMP_STRUK sn;

#endif

//SIP
int protocol_sip=0;
SIPMSG *sipmsgs;
SIPMSG *first;

