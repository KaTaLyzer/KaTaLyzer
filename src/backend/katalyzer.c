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

#include <signal.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>

#include "katalyzer.h"
#include "processing.h"
#include "variables.h"
#include "wait_function.h"
#include "read_conf.h"
#include "errdef.h"
#include "cronovanie.h"
#ifdef SOCK
#include "ksocket.h"
#endif

#define POCTY_SUBOR "spaketov.txt"

//struktura na pretiahnutie dat cez thread - nechutne, ale lepsie riesenie ma zatial nenapadlo
typedef struct {
    ZACIATOK_P *p;
} PRETAH;

int l2p = -1;

int main(int argc, char **argv)
{
#ifdef PCAP
    char errbuf[PCAP_ERRBUF_SIZE];
    char offilename[255];
    int snaplen = 65535;
#endif
    int i_is_config = 0;
    IPV6_adr_D = NULL;
    IPV6_adr_S = NULL;

    int o;

#ifdef PCAP
    while ((o = getopt(argc, argv, ":hwc:df:")) != -1) {
#endif
#ifdef SOCK
    while ((o = getopt(argc, argv, ":hwc:d")) != -1) {
#endif
	    switch (o) {
	        case 'h':
	            help();
	            exit(OK);
	        case 'w':
	            waitt = 0;		// if we do not want to wait for new minute, we start analyzator with parameter 'w'
	            break;
	        case 'c':
	            i_is_config = 1;
	            config_name = optarg;
	            break;
	        case 'd':
	            debug = 1;
	            break;
#ifdef PCAP
	        case 'f':
	            printf("WARNING: DANGEROUS FUNCTION. PRESS CTRL+C TO EXIT OR ANY KEY TO CONTINUE.\n");
	            getchar();
	            isoffline = 1;
	            strcpy(offilename, optarg);
	            break;
#endif
	        default:
	            help();
	            exit(OK);
	    }
    }

    if (!i_is_config) {
	    fprintf(stderr,"Error: please specify config file!\n");
	    help();
	    exit(ERR_NO_CONF);
    }
    s_tmp_log = (char *) malloc(5000 * sizeof(char));
    s_tmp_str = (char *) malloc(200 * sizeof(char));

    printf("KaTaLyzer backend started, time: %ld\n",time(&actual_time));
    if(debug) fprintf(stderr,"Debug mode is enabled!!!\n");


// CONF read
    read_conf();		// function for reading configuration from config file   

// SOCK init
#ifdef SOCK
    struct k_capture c;
    c.name = NULL;

    if (raw_init(&c, interface)) {
	    fprintf(stderr, "Error: Can not create socket\n");
	    return -1;
    }
    
    // FIXME: determine L2 type
    char tmpiffname[1000];
    sprintf(tmpiffname,"/sys/class/net/%s/type",interface);
    fprintf(stderr,"Opening file: %s\n",tmpiffname);
    FILE *ift;
    ift = fopen(tmpiffname,"r");
    char * line = NULL;
    size_t len = 0;
    getline(&line, &len, ift);
    fclose(ift);
    l2p = atoi(line);
#endif

// PCAP init
#ifdef PCAP
    if (isoffline) {
	    FILE *offile;
	    offile = fopen(offilename, "r");
	    if (offile == NULL) {
	        fprintf(stderr, "Error: opening offline file\n");
	        return -1;
	    }
	    if ((fp = pcap_fopen_offline(offile, errbuf)) == NULL) {
	        fprintf(stderr, "Error: pcap offline failure\n");
	        return -1;
	    }
    } else {
	    if ((fp = pcap_open_live(interface, snaplen, 1, 0, errbuf)) == NULL) {
	        fprintf(stderr, "Error capturing traffic on %s network interface: %s", interface, errbuf);
	        exit(ERR_OPEN_IF);
	    }
    }
#endif

    // we wait after opening network adapter - we do not need to wait for error message if we are not able to open the adapter
    if (waitt == 1) waiting();		// here we wait until new minute beggins

#ifdef PCAP
    if (!isoffline) time(&beggining_time);	// beggining_time is time of start of the program. it is incremented by casovac_zapisu timer after each "casovac_zapisu" seconds
#endif
    z_protokoly.empty = 1;	//array is free
    z_protokoly.p_protokoly = NULL;
    z_pair_array = NULL;

#ifdef SOCK
	fprintf(stderr,"SOCK loop\n");
	k_loop(&c, dispatcher_handler);
#endif
#ifdef PCAP
    fprintf(stderr,"PCAP loop\n");
    pcap_loop(fp, 0, dispatcher_handler, NULL);
    pcap_close(fp);
#endif
    printf("End\n");

    return (OK);
}

void help()
{
    printf("\nKaTaLyzer\n");
    printf
	("Usage: ./katalyzer [-h] [-w] [-c config_file] [-d] [-f file]\n");
    printf("-h print this help -h\n");
    printf("-w disable waiting for new minute after start of analyzator (e.g. in case we debug this program)\n");
    printf("-c sets path to configuration file (e.g. /tmp/my_config.conf); do not use space in the path\n");
    printf("-d sets debug mode on\n");
#ifdef PCAP
    printf("-f sets offline mode and path to offline capture file. WARNING: DANGEROUS");
#endif
}

#ifdef SOCK
void dispatcher_handler(const struct k_header *header, const u_char *pkt_data)
#endif
#ifdef PCAP
void dispatcher_handler(u_char *dump, const struct pcap_pkthdr *header, const u_char *pkt_data)
#endif
{ 
    char protokol[DLZKA_POLA_P];	// pomocna premena

#ifdef SOCK
    static struct dev_time *head_dt = NULL;
    set_head_dt(header, head_dt);
#endif

    set_param(header);

// FIXME: get rid of this
    int lng_type; // L2 length/type value
    ethh = (struct ether_header *) pkt_data;	// we store Ether_header
    lng_type = ntohs(ethh->ether_type);	// lng_type - LENGHT/TYPE value

#ifdef PCAP
    int l2t = pcap_datalink(fp);
    if(l2t==1) { // Ethernet
        eth2_frame(pkt_data, lng_type);
    } else if (l2t==113) { // Linux SLL
        lng_type = (pkt_data[14]*256)+pkt_data[15];
        sll_frame(pkt_data, lng_type);
    } else { // 802.3
        ieee802(pkt_data, lng_type);
    }
#endif
#ifdef SOCK
    // FIXME: do it better
    if(l2p==1) { // Ethernet or 802.3
        if (lng_type > 1500) eth2_frame(pkt_data, lng_type); // inspect as Ethernet 2 frame
        else ieee802(pkt_data, lng_type);	// inspect as IEEE 802.3 frame
    } else if (l2p==512) { //PPP
        //lng_type = (pkt_data[14]*256)+pkt_data[15];
        int i=0;        
        for(i=0;i<5;i++) fprintf(stderr,"%.2X ",pkt_data[i]);
        fprintf(stderr,"\n");
        sll_frame(pkt_data, 0);
    }
    // OLD WAY
    /*
    if (lng_type > 1500) eth2_frame(pkt_data, lng_type); // inspect as Ethernet 2 frame
    else ieee802(pkt_data, lng_type);	// inspect as IEEE 802.3 frame
    */
#endif

    parse_data_link(protokol);
    parse_network_layer(protokol);
    parse_transport_layer(protokol);
    parse_application_layer(protokol);
    pair_ip_with_mac();

    // flag - were data written into DB in this time interval? 
    if ((flag == 0 && (actual_time - beggining_time) >= casovac_zapisu) || ((actual_time - beggining_time) >= casovac_zapisu * 2)) database(NULL, header);
    
    // 10 seconds is considered enough time interval as we measure with minimum resolution of 60seconds and it is very probable that there will be another frame comming in that 10 seconds - it will set 'flag' again so we can insert data into DB again after'casovac_zapisu' 
    if (flag == 1 && actual_time - beggining_time >= 10) flag = 0;

    clear_variables();

}

// ???
#ifdef SOCK
void set_head_dt(const struct k_header *header, struct dev_time *head_dt)
{
    if ((header->dt != NULL) && (head_dt == NULL)) {
	    if ((head_dt = (struct dev_time *) malloc(sizeof(struct dev_time))) == NULL) {
	        fprintf(stderr, "k_loop(): Error malloc struct dev_time: %s", strerror(errno));
	        exit(1);
	    }

	    head_dt->ts = header->dt->ts;

	    if ((head_dt->name_do = (char *) malloc(strlen(header->dt->name_do) * sizeof(char))) == NULL) {
	        fprintf(stderr, "k_loop(): Error malloc dt->name: %s",
		    strerror(errno));
	        exit(1);
	    }
	    strcpy(head_dt->name_do, header->dt->name_do);

	    if ((head_dt->name_z = (char *) malloc(strlen(header->dt->name_z) * sizeof(char))) == NULL) {
	        fprintf(stderr, "k_loop(): Error malloc old_interface: %s",
		    strerror(errno));
	        exit(1);
	    }
	    strcpy(head_dt->name_z, header->dt->name_z);
    }
}
#endif


void set_param(const void *arg)
{
#ifdef SOCK
    struct k_header *header;
    header = (struct k_header *) arg;
#endif
#ifdef PCAP
    struct pcap_pkthdr *header;
    header = (struct pcap_pkthdr *) arg;
#endif

    MAC_adr_S = 0;
    MAC_adr_D = 0;
    IP_adr_S = 0;
    IP_adr_D = 0;
    is_ipv6ext = 0;
    if (IPV6_adr_D)
	free(IPV6_adr_D);
    IPV6_adr_D = NULL;
    if (IPV6_adr_S)
	free(IPV6_adr_S);
    IPV6_adr_S = NULL;


    if (isoffline) {
	    if (!isfirsttime) {
	        beggining_time = header->ts.tv_sec;
	        beggining_time /= 10;
	        beggining_time *= 10;
	        isfirsttime = 1;
	    }
	    unix_time = header->ts.tv_sec;
	    actual_time = header->ts.tv_sec;
    } else unix_time = time(&actual_time);	// unix_time - variable for inserting time info into DB

    pocet_B = header->len;	// pocet_B - number of Bytes captured in frame - needed for counting amount of traffic

}

void parse_data_link(char *protokol)
{
    /// ETHERNET
    if (protocol_eth == 1 && strcmp(Eor8, "E") == 0) {	// if we specified to watch ETHERNET II frames in config file and we catched 'E'(it means ETHERNET II frame), we adjust particular statistics
	    strcpy(protokol, "E");
	    m_protokoly(&z_protokoly, protokol);
    }
    /// 802.3
    if (protocol_8023 == 1 && strcmp(Eor8, "8") == 0) {
	    strcpy(protokol, "8");
	    m_protokoly(&z_protokoly, protokol);
    }
    /// Linux SLL
    if (protocol_sll == 1 && strcmp(Eor8, "S") == 0) {
        //fprintf(stderr,"SLL tu SOM\n");
        strcpy(protokol, "S");
        m_protokoly(&z_protokoly, protokol);
    }
}

void parse_network_layer(char *protokol)
{
    /// IP protocol
    if (protocol_ip == 1 && strcmp(net_proto, "IP") == 0) {
	    strcpy(protokol, "IP");
	    m_protokoly(&z_protokoly, protokol);
    }

    if (protocol_ipv6 == 1 && strcmp(net_proto, "IPv6") == 0) {
	    strcpy(protokol, "IPv6");
	    m_protokoly(&z_protokoly, protokol);
    }
    /// ARP
    if (protocol_arp == 1 && strcmp(net_proto, "ARP") == 0) {
	    strcpy(protokol, "ARP");
	    m_protokoly(&z_protokoly, protokol);
    }
    /// RARP
    if (protocol_rarp == 1 && strcmp(net_proto, "RARP") == 0) {
	    strcpy(protokol, "RARP");
	    m_protokoly(&z_protokoly, protokol);
    }
    /// IPX protocol
    if (protocol_ipx == 1 && strcmp(net_proto, "IPX") == 0) {
	    strcpy(protokol, "IPX");
	    m_protokoly(&z_protokoly, protokol);
    }
}

void parse_transport_layer(char *protokol)
{
    /// TCP protocol
    if (protocol_tcp == 1 && strcmp(trans_layer, "TCP") == 0) {
	    strcpy(protokol, "TCP");
	    m_protokoly(&z_protokoly, protokol);
    }
    /// UDP protocol
    if (protocol_udp == 1 && strcmp(trans_layer, "UDP") == 0) {
	    strcpy(protokol, "UDP");
	    m_protokoly(&z_protokoly, protokol);
    }
    ///  ICMP PROTOKOL
    if (protocol_icmp == 1 && strcmp(trans_layer, "ICMP") == 0) {
	    strcpy(protokol, "ICMP");
	    m_protokoly(&z_protokoly, protokol);
    }
    ///  ICMPv6 PROTOKOL
    if (protocol_icmp == 1 && strcmp(trans_layer, "ICMPv6") == 0) {
	    strcpy(protokol, "ICMPv6");
	    m_protokoly(&z_protokoly, protokol);
    }
    /// IGMP protokol
    if (protocol_igmp == 1 && strcmp(trans_layer, "IGMP") == 0) {
	    strcpy(protokol, "IGMP");
	    m_protokoly(&z_protokoly, protokol);
    }
}

void parse_application_layer(char *protokol)
{
    int j;
    for (j = 0; j < i_TCP; j++) {
	    // variable 'protocol' contains source port number information, druhy_protocol contains destination port number information
	    // if either one of them is the one we are watching, we adjust statistics
	    if (s_protocol == pole_TCP_portov[j] || d_protocol == pole_TCP_portov[j]) {
	        if (s_protocol == pole_TCP_portov[j]) sprintf(protokol, "TCP%d", s_protocol);
	        if (d_protocol == pole_TCP_portov[j]) sprintf(protokol, "TCP%d", d_protocol);
	        m_protokoly(&z_protokoly, protokol);
	    }
    }

    ////////////
    // UDP ports
    ////////////
    for (j = 0; j < i_UDP; j++) {
	    if (s_protocol == pole_UDP_portov[j] || d_protocol == pole_UDP_portov[j]) {
	        if (s_protocol == pole_UDP_portov[j]) sprintf(protokol, "UDP%d", s_protocol);
	        if (d_protocol == pole_UDP_portov[j]) sprintf(protokol, "UDP%d", d_protocol);
	        m_protokoly(&z_protokoly, protokol);
	    }
    }
}

void pair_ip_with_mac(void)
{
    if (z_pair_array == NULL) {
	    z_pair_array = (PAIR_ARRAY *) malloc(sizeof(PAIR_ARRAY));
	    z_pair_array->mac_d = MAC_adr_D;
	    z_pair_array->mac_s = MAC_adr_S;
	    if (is_ipv6ext) {
	        z_pair_array->is_ipv6 = is_ipv6ext;
	        z_pair_array->ipv6_d = IPV6_adr_D;
	        z_pair_array->ipv6_s = IPV6_adr_S;
	    } else {
	        z_pair_array->is_ipv6 = is_ipv6ext;
	        z_pair_array->ip_d = ntohl(IP_adr_D);
	        z_pair_array->ip_s = ntohl(IP_adr_S);
	    }
	    z_pair_array->p_next = NULL;
    } else {
	    PAIR_ARRAY *pom_array;
	    int find = 0;
	    for (pom_array = z_pair_array; pom_array != NULL;
	         pom_array = pom_array->p_next) {
	        find = 0;
	        if ((pom_array->mac_s == MAC_adr_S) && (pom_array->mac_d == MAC_adr_D)) {
		        if ((!is_ipv6ext && (pom_array->is_ipv6 == is_ipv6ext) && (pom_array->ip_s == ntohl(IP_adr_S)) && (pom_array->ip_d == ntohl(IP_adr_D))) || (is_ipv6ext && (pom_array->is_ipv6 == is_ipv6ext) && (compare_IPv6(pom_array->ipv6_s, IPV6_adr_S) && compare_IPv6(pom_array->ipv6_d,IPV6_adr_D)))) {
		            find = 1;
		            break;
		        }
	        }
	    }
	    if (!find) {
	        for (pom_array = z_pair_array; pom_array->p_next != NULL; pom_array = pom_array->p_next) ;
	        pom_array->p_next = (PAIR_ARRAY *) malloc(sizeof(PAIR_ARRAY));
	        pom_array = pom_array->p_next;
	        pom_array->mac_d = MAC_adr_D;
	        pom_array->mac_s = MAC_adr_S;
	        if (is_ipv6ext) {
		        pom_array->is_ipv6 = is_ipv6ext;
		        pom_array->ipv6_d = IPV6_adr_D;
		        pom_array->ipv6_s = IPV6_adr_S;
	        } else {
		        pom_array->is_ipv6 = is_ipv6ext;
		        pom_array->ip_d = ntohl(IP_adr_D);
		        pom_array->ip_s = ntohl(IP_adr_S);
	        }
	        pom_array->p_next = NULL;
	    }
    }
}

void set_time(void)
{
    if ((actual_time - beggining_time) >= casovac_zapisu * 2) processing_time += 60;
    else processing_time = unix_time;

    //ak je prilis velky rozdiel medzi aktualnym casom a poslednim ulozenim do databazy tak nastavi aktualny cas
    if ((actual_time - beggining_time) >= casovac_zapisu * 2)
	beggining_time = actual_time - casovac_zapisu;
}

void connect_to_db(MYSQL * conn)
{
    char prikaz[20000];
    if (mysql_real_connect(conn, db_host, db_user, db_pass, NULL, db_port, NULL,CLIENT_MULTI_STATEMENTS) == NULL) {
	    fprintf(stderr, "Failed to connect to MYSQL database: Error: %s\n",mysql_error(conn));
	    exit(ERR_MYSQL_CON);
    }
    //creating DB if it does not exist and connecting to it
    sprintf(prikaz, "CREATE DATABASE IF NOT EXISTS %s; USE %s;", db_name,db_name);
    if (mysql_query(conn, prikaz)) {
	    fprintf(stderr, "Failed to create MYSQL database %s: %s\n",db_name, mysql_error(conn));
	    exit(ERR_MYSQL_DB_CREATE);
    }
    mysql_next_result(conn);
}

void create_db(MYSQL * conn, struct dev_time *head_dt, const void *arg)
{
    char prikaz[20000];
    int ii;
    PROTOKOLY *p_prot;

#ifdef SOCK
    struct k_header *header;
    header = (struct k_header *) arg;
/*#endif
#ifdef PCAP
    struct pcap_pkthdr *header;
    header = (struct pcap_pkthdr *) arg;
#endif

#ifdef SOCK*/
    if ((header->interface_auto) && (head_dt != NULL)) {
	    sprintf(prikaz,"CREATE TABLE IF NOT EXISTS `INTERFACE_time` ( `time` int(11) NOT NULL, `interface_z` varchar(255) NOT NULL, `interface_do` varchar(255) NOT NULL) ENGINE=MyISAM DEFAULT CHARSET=utf8;");
	    if (mysql_query(conn, prikaz)) {
	        fprintf(stderr,"Failed to create INTERFACE_time table in MYSQL database %s: %s\n",db_name, mysql_error(conn));
	        exit(ERR_MYSQL_DB_CREATE);
	    }
	    mysql_next_result(conn);

	    sprintf(prikaz,"INSERT INTO `INTERFACE_time` (time, interface_z, interface_do) VALUES (\"%d\", \"%s\", \"%s\")",(int)head_dt->ts.tv_sec, head_dt->name_z, head_dt->name_do);
	    if (mysql_query(conn, prikaz)) {
	        fprintf(stderr,"Failed to insert into INTERFACE_time table in MYSQL database %s: %s\nPrikaz: %s\n",db_name, mysql_error(conn), prikaz);
	        exit(ERR_MYSQL_DB_CREATE);
	    }
	    mysql_next_result(conn);

	    if (head_dt != NULL) {
	        if (head_dt->name_z != NULL) {
		        free(head_dt->name_z);
		        head_dt->name_z = NULL;
	        }
	        if (head_dt->name_do) {
		        free(head_dt->name_do);
		        head_dt->name_do = NULL;
	        }
	        free(head_dt);
	        head_dt = NULL;
	    }

    }
#endif

    if (!(z_protokoly.empty)) {
	    for (p_prot = z_protokoly.p_protokoly; p_prot != NULL; p_prot = p_prot->p_next) {
	        if (!p_prot->is_ipv6)
		    sprintf(prikaz,
			    "CREATE TABLE IF NOT EXISTS %s_1m_time (`time` int(10) unsigned NOT NULL default '0',`IP_id` int(10) unsigned NOT NULL default '0',`MAC_id` int(10) unsigned NOT NULL default '0',`IP_SD_id` int(10) unsigned NOT NULL default '0',`MAC_SD_id` int(10) unsigned NOT NULL default '0',PRIMARY KEY (`time`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_1m_IP(`id` int(10) unsigned NOT NULL auto_increment,`IP` int unsigned default '0',`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` mediumint(8) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;\
            CREATE TABLE IF NOT EXISTS %s_1m_MAC(`id` int(10) unsigned NOT NULL auto_increment,`MAC` bigint unsigned default '0',`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` mediumint(8) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D`mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_1m_IP_SD(`id` int(10) unsigned NOT NULL auto_increment,`IP_1` int unsigned default '0',`IP_2` int unsigned default '0',`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` mediumint(8) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;\
            CREATE TABLE IF NOT EXISTS %s_1m_MAC_SD(`id` int(10) unsigned NOT NULL auto_increment,`MAC_1` bigint unsigned default '0',`MAC_2` bigint unsigned default '0',`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` mediumint(8) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;",
			    p_prot->protokol, p_prot->protokol, p_prot->protokol, p_prot->protokol, p_prot->protokol);
	        else
		    sprintf(prikaz,
			    "CREATE TABLE IF NOT EXISTS %s_v6_1m_time (`time` int(10) unsigned NOT NULL default '0',`IP_id` int(10) unsigned NOT NULL default '0',`MAC_id` int(10) unsigned NOT NULL default '0',`IP_SD_id` int(10) unsigned NOT NULL default '0',`MAC_SD_id` int(10) unsigned NOT NULL default '0',PRIMARY KEY (`time`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_v6_1m_IP(`id` int(10) unsigned NOT NULL auto_increment,`IP` char(32),`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` mediumint(8) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;\
            CREATE TABLE IF NOT EXISTS %s_v6_1m_MAC(`id` int(10) unsigned NOT NULL auto_increment,`MAC` bigint unsigned default '0',`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` mediumint(8) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D`mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_v6_1m_IP_SD(`id` int(10) unsigned NOT NULL auto_increment,`IP_1` char(32), `IP_2` char(32),`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` mediumint(8) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;\
            CREATE TABLE IF NOT EXISTS %s_v6_1m_MAC_SD(`id` int(10) unsigned NOT NULL auto_increment,`MAC_1` bigint unsigned default '0',`MAC_2` bigint unsigned default '0',`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` mediumint(8) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;",
			    p_prot->protokol, p_prot->protokol, p_prot->protokol, p_prot->protokol, p_prot->protokol);

	        if (mysql_query(conn, prikaz)) {
		        fprintf(stderr,"Failed to create %s tables in MYSQL database %s: %s\nPrikaz: %s\n",p_prot->protokol, db_name, mysql_error(conn),	prikaz);
		        exit(ERR_MYSQL_TBL_CREATE);
	        }
	        // 5 is number of commands sent into DB - we have to release results of their execution
	        for (ii = 0; ii < 5; ii++) mysql_next_result(conn);
	    }
    }

    if (z_pair_array != NULL) {
	    sprintf(prikaz,"CREATE TABLE IF NOT EXISTS IPlist (`IP` int unsigned default '0', `MAC` bigint unsigned default '0',  PRIMARY KEY(`IP`)) ENGINE=MyISAM;");
	    if (mysql_query(conn, prikaz)) {
	        fprintf(stderr,"Failed to create IPlist tables in MYSQL database %s: %s\n",db_name, mysql_error(conn));
	        exit(ERR_MYSQL_TBL_CREATE);
	    }
	    mysql_next_result(conn);

	    sprintf(prikaz,"CREATE TABLE IF NOT EXISTS IPv6list (`IP` char(32), `MAC` bigint unsigned default '0',  PRIMARY KEY(`IP`)) ENGINE=MyISAM;");
	    if (mysql_query(conn, prikaz)) {
	        fprintf(stderr,"Failed to create IPlist tables in MYSQL database %s: %s\n",db_name, mysql_error(conn));
	        exit(ERR_MYSQL_TBL_CREATE);
	    }
	    mysql_next_result(conn);
    }
}

void insert_data_to_table(MYSQL * conn)
{
    char prikaz[20000];

    if (z_pair_array != NULL) {
	    PAIR_ARRAY *pom_array;
	    for (pom_array = z_pair_array; pom_array != NULL; pom_array = pom_array->p_next) {
	        if (!pom_array->is_ipv6) {
		        sprintf(prikaz,"INSERT INTO IPlist (IP, MAC) VALUES ('%u','%lu')ON DUPLICATE KEY UPDATE MAC='%lu';",pom_array->ip_s, pom_array->mac_s, pom_array->mac_s);
		        if (mysql_query(conn, prikaz)) fprintf(stderr,"Failed to insert data into MYSQL database %s: %s\n",db_name, mysql_error(conn));
		        sprintf(prikaz,"INSERT INTO IPlist (IP, MAC) VALUES ('%u','%lu')ON DUPLICATE KEY UPDATE MAC='%lu';",pom_array->ip_d, pom_array->mac_d,pom_array->mac_d);
		        if (mysql_query(conn, prikaz)) fprintf(stderr,"Failed to insert data into MYSQL database %s: %s\n",db_name, mysql_error(conn));
	        } else {
		        sprintf(prikaz,"INSERT INTO IPv6list (IP, MAC) VALUES ('%08x%08x%08x%08x','%lu')ON DUPLICATE KEY UPDATE MAC='%lu';", pom_array->ipv6_s[0], pom_array->ipv6_s[1], pom_array->ipv6_s[2], pom_array->ipv6_s[3], pom_array->mac_s, pom_array->mac_s);
		        if (mysql_query(conn, prikaz)) fprintf(stderr,"Failed to insert data into MYSQL database %s: %s\n",db_name, mysql_error(conn));
		        sprintf(prikaz,"INSERT INTO IPv6list (IP, MAC) VALUES ('%08x%08x%08x%08x','%lu')ON DUPLICATE KEY UPDATE MAC='%lu';", pom_array->ipv6_d[0], pom_array->ipv6_d[1], pom_array->ipv6_d[2], pom_array->ipv6_d[3], pom_array->mac_d, pom_array->mac_d);
		        if (mysql_query(conn, prikaz)) fprintf(stderr,"Failed to insert data into MYSQL database %s: %s\n", db_name, mysql_error(conn));
	        }
	    }
    }
}

void flush_data_into_db(void)
{
    ZACIATOK_P *z_protokoly2;
    PRETAH *pretah1;
    pthread_t thread;

    z_protokoly2 = malloc(sizeof(ZACIATOK_P));
    if (z_protokoly2 == NULL) {
	    fprintf(stderr, "Error in function flush_data_into_db: %s\n",strerror(errno));
	    exit(-1);
    }

    pretah1 = malloc(sizeof(PRETAH));
    if (pretah1 == NULL) {
	    fprintf(stderr, "Error in function flush_data_into_db: %s\n",strerror(errno));
	    exit(-1);
    }
    //vytvorenie "kopie" z_protokoly do z_protokoly2
    z_protokoly2->empty = 0;
    z_protokoly2->p_protokoly = z_protokoly.p_protokoly;
    z_protokoly.empty = 1;
    z_protokoly.p_protokoly = NULL;

    if(debug) fprintf(stderr, "Debug: zapis do DB\n");

    pretah1->p = z_protokoly2;
    if (pthread_create(&thread, NULL, zapis_do_DB_protokoly, (void *) pretah1)) fprintf(stderr, "Error in function phtread_create: %s\n", strerror(errno));
}

void clear_pairing_array()
{
    PAIR_ARRAY *pom_array, *pom_array1;

    if (z_pair_array != NULL) {
	    for (pom_array = z_pair_array; pom_array != NULL;) {
	        pom_array1 = pom_array->p_next;
	        pom_array->p_next = NULL;
	        free(pom_array);
	        pom_array = pom_array1;
	    }
    }
    z_pair_array = NULL;
}

void sum_table()
{
    pthread_t cron;
    int casy[4] = { 5, 30, 2, 1 }, i = 0;
    int modu[4] = { 5, 30, 120, 1440 };
    int rozd[4] = { 1, 5, 30, 120 };
    int *arg;

    arg = malloc(sizeof(int));
    if (arg == NULL) {
	    fprintf(stderr, "Error in function sum_table: %s\n",strerror(errno));
	    exit(-1);
    }

    interval++;

    for (i = 0; i < 4; i++) {
	    if (i == 0) fprintf(stderr, "interval:%d\n", interval);
	    if ((interval % modu[i] == rozd[i]) && (interval != rozd[i])) {
	        *arg = casy[i];
	        switch (pthread_create(&cron, NULL, cronovanie, (void *) arg)) {
	            case -1:
		            fprintf(stderr, "zlyhal thread\n");
		            break;
	            case 0:
		            fprintf(stderr, "\ncronovanie(%d) [DONE]\n", casy[i]);
		            break;
	            default:
		            break;
	        }
	    }
    }
}

void clear_variables()
{
    unix_time = 0;
    pocet_B = 0;
    Eor8[0] = 0;
    MAC_adr_S = 0;
    MAC_adr_D = 0;
    net_proto[0] = 0;
    trans_layer[0] = 0;
    protocol = 0;
    s_protocol = 0;
    d_protocol = 0;
}

void database(struct dev_time *head_dt, const void *arg)
{
    MYSQL *conn = NULL;
    conn = mysql_init(NULL);
#ifdef SOCK
    struct k_header *header;
    header = (struct k_header *) arg;
#endif
#ifdef PCAP
    struct pcap_pkthdr *header;
    header = (struct pcap_pkthdr *) arg;
#endif
    set_time();

//////////////////////////////////////////////////////////////////////
// DB
//////////////////////////////////////////////////////////////////////

    connect_to_db(conn);

    // creating DB tables 
    create_db(conn, head_dt, header);

    // inserting data into table IPlist
    insert_data_to_table(conn);

    // FIXME ???
    mysql_close(conn);

    flush_data_into_db();

    flag = 1; // after processing the frame we just processed we set 'flag' to true which indicates that data in this time interval were already processed and in this second we are not going to insert any more data into DB - we will do it after passing 'casovac_zapisu' seconds

    // clear pairing array
    clear_pairing_array();

    beggining_time += casovac_zapisu;	// add 'casovac_zapisu' to 'begginig_time' means that we already processed the data for last time interval(which started at 'beggining_time'), we are going to calculate statistics for ner time interval

    // we predict that time interval between incoming two frames will not be bigger than 'casovac_zapisu' - it is very unlikely to happen(minimum ARP packets arrive very often)

    //////////////
    //SUM TABLES
    //////////////

    sum_table();
}

void eth2_frame(const u_char * pkt_data, int type)
{
    // we received ETHERNET II frame - we need to set variable Eor8
    sprintf(Eor8, "E");

    int i;
    MAC_adr_S = 0;
    MAC_adr_D = 0;
    for (i = 0; i < ETH_ALEN; i++) {
	    MAC_adr_S <<= 8;
	    MAC_adr_S += ethh->ether_shost[i];
    }

    for (i = 0; i < ETH_ALEN; i++) {
	    MAC_adr_D <<= 8;
	    MAC_adr_D += ethh->ether_dhost[i];
    }

    net_protokol(type, net_proto);

    if (type == 2048) ip_protokol(pkt_data, 0);	// if we received IP or ARP protocol, analysis continues
    if (type == 34525) ipv6_protokol(pkt_data, 0);	//IPv6 to be implemented
    if (type == 2054) arp_protokol(pkt_data);
}

void sll_frame(const u_char * pkt_data, int type) {
    fprintf(stderr,"SLL tu SOM pri parse\n");
    sprintf(Eor8, "S");
    net_protokol(type, net_proto);
    if (type == 2048) ip_protokol(pkt_data, 2);	// if we received IP or ARP protocol, analysis continues
    if (type == 34525) ipv6_protokol(pkt_data, 2);	//IPv6 to be implemented
    if (type == 2054) arp_protokol(pkt_data);
}

void arp_protokol(const u_char * pkt_data)
{
    //    int l_prot,op_code;
    int i, j;
    arph = (struct arphdr *) (pkt_data + sizeof(struct ether_header));

    //    l_prot=ntohs(arph->ar_hrd);
    //    op_code=ntohs(arph->ar_op);

    MAC_adr_S = 0;
    MAC_adr_D = 0;
    j = (sizeof(struct ether_header) + sizeof(struct arphdr));
    for (i = 0; i < ETH_ALEN; i++) {
	    MAC_adr_S <<= 8;
	    MAC_adr_S += pkt_data[j + i];
    }
    j = (sizeof(struct ether_header) + sizeof(struct arphdr) + arph->ar_hln + arph->ar_pln);
    for (i = 0; i < ETH_ALEN; i++) {
	    MAC_adr_D <<= 8;
	    MAC_adr_D += pkt_data[j + i];
    }

    //sprintf(MAC_adr,"%.2x%.2x%.2x%.2x%.2x%.2x",pkt_data[22],pkt_data[23],pkt_data[24],pkt_data[25],pkt_data[26],pkt_data[27]);

    IP_adr_D = 0;
    IP_adr_S = 0;

    j = (sizeof(struct ether_header) + sizeof(struct arphdr) + arph->ar_hln + arph->ar_pln);
    j--;
    for (i = 0; i < 4; i++) {
	    IP_adr_S <<= 8;
	    IP_adr_S += pkt_data[j - i];
    }
    j = (sizeof(struct ether_header) + sizeof(struct arphdr) + arph->ar_hln * 2 + arph->ar_pln * 2);
    j--;
    for (i = 0; i < 4; i++) {
	    IP_adr_D <<= 8;
	    IP_adr_D += pkt_data[j - i];
    }
}

void ip_protokol(const u_char * pkt_data, int len)
{
    int protocol;

    is_ipv6ext = 0;

    iph = (struct iphdr *) (pkt_data + (sizeof(struct ether_header)) + len);	//store IP header structure
    protocol = iph->protocol;	//protocol inside IP packet

    IP_adr_S = iph->saddr;
    IP_adr_D = iph->daddr;

    //fprintf(stderr,"IP S:%d D:%d\n",iph->saddr,iph->daddr);

    trans_protokol(protocol, trans_layer);

    len = iph->ihl * 4;

    if (protocol == 6) tcp_protokol(pkt_data, len);	//we continue analysis in case there is TCP, UDP or ICMP protocol inside IP packet
    //    if(protocol==1) icmp_protokol(pkt_data,len);
    if (protocol == 17) udp_protokol(pkt_data, len);
}

void ipv6_protokol(const u_char * pkt_data, int len)
{
    int protocol, i;
    char end = 0;
    unsigned int *ipv6_pom;
    struct ip6_hbh *ip6hop;
    struct ip6_rthdr *ip6r;
    struct ip6_frag *ip6f;
    struct ip6_dest *ip6d;

    is_ipv6ext = 1;

    iphv6 = (struct ip6_hdr *) (pkt_data + (sizeof(struct ether_header)) + len);
    protocol = iphv6->ip6_ctlun.ip6_un1.ip6_un1_nxt;

    len = sizeof(struct ip6_hdr);

    //prejdeme celu hlavicku ipv6
    for (;;) {
	    switch (protocol) {
	        case IPPROTO_HOPOPTS:
	            ip6hop = (struct ip6_hbh *) (pkt_data + (sizeof(struct ether_header)) + len);
	            protocol = ip6hop->ip6h_nxt;
	            len += sizeof(struct ip6_hbh);
	            break;
	        case IPPROTO_ROUTING:
	            ip6r = (struct ip6_rthdr *) (pkt_data + (sizeof(struct ether_header)) + len);
	            protocol = ip6r->ip6r_nxt;
	            len += sizeof(struct ip6_rthdr);
	            break;
	        case IPPROTO_FRAGMENT:
	            ip6f = (struct ip6_frag *) (pkt_data + (sizeof(struct ether_header)) + len);
	            protocol = ip6f->ip6f_nxt;
	            len += sizeof(struct ip6_frag);
	            break;
	        case IPPROTO_ESP:
	            ip6d = (struct ip6_dest *) (pkt_data + (sizeof(struct ether_header)) + len);
	            protocol = ip6d->ip6d_nxt;
	            len += sizeof(struct ip6_dest);
	            break;
	        case IPPROTO_AH:
	            ip6d = (struct ip6_dest *) (pkt_data + (sizeof(struct ether_header)) + len);
	            protocol = ip6d->ip6d_nxt;
	            len += sizeof(struct ip6_dest);
	            break;
	        case IPPROTO_DSTOPTS:
	            ip6d = (struct ip6_dest *) (pkt_data + (sizeof(struct ether_header)) + len);
	            protocol = ip6d->ip6d_nxt;
	            len += sizeof(struct ip6_dest);
	            break;
	        case IPPROTO_GRE:
	            ip6d = (struct ip6_dest *) (pkt_data + (sizeof(struct ether_header)) + len);
	            protocol = ip6d->ip6d_nxt;
	            len += sizeof(struct ip6_dest);
	            break;
	        case IPPROTO_TCP:
	            trans_protokol(protocol, trans_layer);
	            tcp_protokol(pkt_data, len);	//we continue analysis in case there is TCP, UDP or ICMP protocol inside IP packet
	            end = 1;
	            break;
	        case IPPROTO_UDP:
	            trans_protokol(protocol, trans_layer);
	            udp_protokol(pkt_data, len);
	            end = 1;
	            break;
	        case IPPROTO_ICMPV6:
	            strcpy(trans_layer, "ICMPv6");
	            end = 1;
	            break;
	        case IPPROTO_NONE:
	            end = 1;
	        default:
	            end = 1;
	            break;
	    }
	    //end ipv6 header
	    if (end) break;
    }

    if ((IPV6_adr_D = (unsigned int *) malloc(sizeof(unsigned int) * IPV6SIZE)) == NULL) {
	    fprintf(stderr, "Error malloc: %s\n", strerror(errno));
	    return;
    }
    if ((IPV6_adr_S = (unsigned int *) malloc(sizeof(unsigned int) * IPV6SIZE)) == NULL) {
	    fprintf(stderr, "Error malloc: %s\n", strerror(errno));
	    return;
    }

    ipv6_pom = (unsigned int *) &iphv6->ip6_src;

    for (i = 0; i < 4; i++) IPV6_adr_S[i] = ntohl(ipv6_pom[i]);

    ipv6_pom = (unsigned int *) &iphv6->ip6_dst;

    for (i = 0; i < 4; i++) IPV6_adr_D[i] = ntohl(ipv6_pom[i]);


}

void trans_protokol(int number, char *protokols)
{				//according to number from IP header we assign name of the protocol to the number; adding new protocol takes just a few seconds (ICMP RFC 792)
    char protokol[5];
    switch (number) {
        case 1:
	        sprintf(protokol, "ICMP");
	        break;
        case 2:
	        sprintf(protokol, "IGMP");
	        break;
        case 6:
	        sprintf(protokol, "TCP");
	        break;
        case 8:
	        sprintf(protokol, "IGP");
	        break;
        case 9:
	        sprintf(protokol, "EGP");
	        break;
        case 17:
	        sprintf(protokol, "UDP");
	        break;
        default:
	        sprintf(protokol, "%d", number);
	        break;
    }
    strcpy(protokols, protokol);
}


void tcp_protokol(const u_char * pkt_data, int len)
{
    unsigned int s_port, d_port;
    //    unsigned int port_id,sn_port,dn_port;

    tcph = (struct tcphdr *) (pkt_data + sizeof(struct ether_header) + len);

    s_port = ntohs(tcph->source);
    //    sn_port=tcph->source;
    d_port = ntohs(tcph->dest);
    //    dn_port=tcph->dest;

    /*
     *    a=14+h_len;                                     //a->begining of TCP header
     *    s_port=pkt_data[a]*256+pkt_data[a+1];           //source port
     *    sn_port=pkt_data[a+1]*256+pkt_data[a];          //network order
     *    d_port=pkt_data[a+2]*256+pkt_data[a+3];         //destination port
     *    dn_port=pkt_data[a+3]*256+pkt_data[a+2];        //network order
     */

    s_protocol = s_port;
    d_protocol = d_port;
}

/*
void icmp_protokol(const u_char *pkt_data,int h_len)
{
	int a,type,code;
	a=14+h_len;
	type=pkt_data[a];
	code=pkt_data[a+1];
	if(type==8 && code==0) sprintf(other_info,"Echo request");
	else if(type==0 && code==0) sprintf(other_info,"Echo response");
	else if(type==10 && code ==0) sprintf(other_info,"Routing request");
	else if(type==9 && code==0) sprintf(other_info,"Routing response");
	else sprintf(other_info,"Type:%d Code:%d",type,code);
}
*/

void udp_protokol(const u_char * pkt_data, int len)
{
    int s_port, d_port;
    //    int sn_port,dn_port;

    udph = (struct udphdr *) (pkt_data + sizeof(struct ether_header) + len);

    s_port = ntohs(udph->source);
    //    sn_port=udph->source;           //network order
    d_port = ntohs(udph->dest);
    //    dn_port=udph->dest;             //network order
    /*
     *    a=14+h_len;                                     //a->begining of UDP header
     *    s_port=pkt_data[a]*256+pkt_data[a+1];
     *    sn_port=pkt_data[a+1]*256+pkt_data[a];          //network order
     *    d_port=pkt_data[a+2]*256+pkt_data[a+3];
     *    dn_port=pkt_data[a+3]*256+pkt_data[a+2];        //network order
     */

    s_protocol = s_port;
    d_protocol = d_port;
}

/*analysis of IEEE 802.3 frame*/
void ieee802(const u_char * pkt_data, int type)
{
    sprintf(Eor8, "8");		//it is 802.3 frame ->  we need to set variable Eor8

    MAC_adr_S = 0;
    MAC_adr_D = 0;
    int i;
    for (i = 0; i < ETH_ALEN; i++) {
	    MAC_adr_S *= 256;
	    MAC_adr_S += ethh->ether_shost[i];
    }

    for (i = 0; i < ETH_ALEN; i++) {
	    MAC_adr_D *= 256;
	    MAC_adr_D += ethh->ether_dhost[i];
    }

    //sprintf(MAC_adr,"%.2x%.2x%.2x%.2x%.2x%.2x",pkt_data[6],pkt_data[7],pkt_data[8],pkt_data[9],pkt_data[10],pkt_data[11]);

    if (pkt_data[14] == 0xaa && pkt_data[15] == 0xaa && pkt_data[16] == 0x03 && pkt_data[17] == 0 && pkt_data[18] == 0 && pkt_data[19] == 0) {
	    type = pkt_data[20] * 256 + pkt_data[21];
	    if (type == 2048) ip_protokol(pkt_data, 8);	// if we received IP protocol, analysis continues
    }

    net_protokol(type, net_proto);
}

void net_protokol(int number, char *protokols)
{
    char protokol[9];
    switch (number) {
        case 2048:
	        sprintf(protokol, "IP");
	        break;
        case 2054:
	        sprintf(protokol, "ARP");
	        break;
        case 32821:
	        sprintf(protokol, "RARP");
	        break;
        case 33079:
	        sprintf(protokol, "IPX");
	        break;
        case 34525:
	        sprintf(protokol, "IPv6");
	        break;
        case 34827:
	        sprintf(protokol, "PPP");
	        break;
        case 36864:
	        sprintf(protokol, "Loopback");
	        break;
        default:
	        sprintf(protokol, "%d", number);
	        break;
    }
    strcpy(protokols, protokol);
}

void m_protokoly(ZACIATOK_P * p_zac, char *s)
{
    ZAZNAMY *help_zaznamy;	//pomocna struktura
    PROTOKOLY *help_protokol, *p_protokol;
    int find, find_p;

    if (p_zac->empty) {		//if struct is empty, we create it
	    p_zac->empty = 0;
	    if ((p_zac->p_protokoly = (PROTOKOLY *) malloc(sizeof(PROTOKOLY))) == NULL) {	// create structure PROTOKOLY
	        fprintf(stderr, "Error malloc: %s\n", strerror(errno));
	        return;
	    }
	    p_protokol = p_zac->p_protokoly;
	    strcpy(p_protokol->protokol, s);	// set the name of the protocol
	    if ((p_protokol->zoznam = (ZAZNAMY *) malloc(sizeof(ZAZNAMY))) == NULL) {	// create structure ZAZNAMY
	        fprintf(stderr, "Error malloc: %s\n", strerror(errno));
	        return;
	    }
	    p_protokol->is_ipv6 = is_ipv6ext;	// check s it a ip6 ip adress
	    p_protokol->zoznam->mac_s = MAC_adr_S;	// set the MAC adres and IP adress
	    p_protokol->zoznam->mac_d = MAC_adr_D;
	    p_protokol->zoznam->ip_d = 0;
	    p_protokol->zoznam->ip_s = 0;
	    p_protokol->zoznam->ipv6_d = NULL;
	    p_protokol->zoznam->ipv6_s = NULL;
	    if (is_ipv6ext) {
	        if ((p_protokol->zoznam->ipv6_d = (unsigned int *) malloc(sizeof(int) * IPV6SIZE)) == NULL) {
		        fprintf(stderr, "Error malloc p_protokol->zoznam->ipv6_d: %s\n", strerror(errno));
		        exit(-1);
	        }
	        *p_protokol->zoznam->ipv6_d = *IPV6_adr_D;
	        if ((p_protokol->zoznam->ipv6_s = (unsigned int *) malloc(sizeof(int) * IPV6SIZE)) == NULL) {
		        fprintf(stderr, "Error malloc p_protokol->zoznam->ipv6_s: %s\n", strerror(errno));
		        exit(-1);
	        }
	        *p_protokol->zoznam->ipv6_s = *IPV6_adr_S;
	    } else {
	        p_protokol->zoznam->ip_s = ntohl(IP_adr_S);	// we use ntohl, because of frontend
	        p_protokol->zoznam->ip_d = ntohl(IP_adr_D);
	    }
	    p_protokol->zoznam->pocet_B = pocet_B;	// set the number of bytes
	    p_protokol->zoznam->pocet_ramcov = 1;
	    p_protokol->zoznam->p_next = NULL;
	    p_protokol->zoznam->spracovany[0] = 0;
	    p_protokol->zoznam->spracovany[1] = 0;
	    p_protokol->zoznam->spracovany[2] = 0;
	    p_protokol->zoznam->spracovany[3] = 0;
	    p_protokol->p_next = NULL;
	    p_protokol->empty = 0;
    } else {
	    for (help_protokol = p_zac->p_protokoly; help_protokol != NULL; help_protokol = help_protokol->p_next) { //we looking for the protocol
	        find_p = 0;		//we don't find the protocol
	        if ((!strcmp(help_protokol->protokol, s)) && (help_protokol->is_ipv6 == is_ipv6ext)) {
		        find_p = 1;	//we find the protocol
		        for (help_zaznamy = help_protokol->zoznam; help_zaznamy != NULL; help_zaznamy = help_zaznamy->p_next) {	// prechadza strukturu zoznam
		            find = 0;	//set we don't find the IP adress and MAC adress
		            if ((help_zaznamy->mac_s == MAC_adr_S) && (help_zaznamy->mac_d == MAC_adr_D)) {	// we looking the IP adress and MAC adress in the structure
                        //we find the IP adress and MAC adress
			            if ((!is_ipv6ext && ((help_zaznamy->ip_s == ntohl(IP_adr_S)) && (help_zaznamy->ip_d == ntohl(IP_adr_D)))) || (is_ipv6ext && ((compare_IPv6(help_zaznamy->ipv6_d, IPV6_adr_D)) && (compare_IPv6(help_zaznamy->ipv6_s, IPV6_adr_S))))) {
			                help_zaznamy->pocet_B += pocet_B;
			                help_zaznamy->pocet_ramcov++;
			                find = 1;	//set we find the IP adress and MAC adress
			                break;
			            }
		            }
		        }
		        if (!find) {	// we don't find the IP adress and MAC adress, so we create new structure
		            for (help_zaznamy = help_protokol->zoznam; help_zaznamy->p_next != NULL; help_zaznamy = help_zaznamy->p_next) ;	//dojde na koniec struktury
		            if ((help_zaznamy->p_next = (ZAZNAMY *) malloc(sizeof(ZAZNAMY))) == NULL) {	//we create new structre ZANAMY
			            fprintf(stderr, "Error malloc help_zaznamy->p_next: %s\n",strerror(errno));
			            exit(-1);
		            }
		            help_zaznamy = help_zaznamy->p_next;
		            help_zaznamy->mac_s = MAC_adr_S;
		            help_zaznamy->mac_d = MAC_adr_D;
		            help_zaznamy->ip_d = 0;
		            help_zaznamy->ip_s = 0;
		            help_zaznamy->ipv6_d = NULL;
		            help_zaznamy->ipv6_s = NULL;
		            if (is_ipv6ext) {
			            if ((help_zaznamy->ipv6_d = (unsigned int *) malloc(sizeof(int) * IPV6SIZE)) == NULL) {
			                fprintf(stderr, "Error malloc help_zaznamy->ipv6_d: %s\n",strerror(errno));
			                exit(-1);
			            }
			            *help_zaznamy->ipv6_d = *IPV6_adr_D;
			            if ((help_zaznamy->ipv6_s = (unsigned int *) malloc(sizeof(int) * IPV6SIZE)) == NULL) {
			                fprintf(stderr, "Error malloc help_zaznamy->ipv6_s: %s\n",strerror(errno));
			                exit(-1);
			            }
			            *help_zaznamy->ipv6_s = *IPV6_adr_S;
		            } else {
			            help_zaznamy->ip_s = ntohl(IP_adr_S);	// we use ntohl, because of frontend
			            help_zaznamy->ip_d = ntohl(IP_adr_D);
		            }
		            help_zaznamy->pocet_B = pocet_B;
		            help_zaznamy->pocet_ramcov = 1;
		            help_zaznamy->spracovany[0] = 0;
		            help_zaznamy->spracovany[1] = 0;
		            help_zaznamy->spracovany[2] = 0;
		            help_zaznamy->spracovany[3] = 0;
		            help_zaznamy->p_next = NULL;
		        }
		        break;
	        }
	    }
	    if (!find_p) {		// we don't find the protocol, so we crete new structure for the new protocol
	        for (help_protokol = p_zac->p_protokoly; help_protokol->p_next != NULL; help_protokol = help_protokol->p_next);	//dojde na koniec struktury
	        if ((help_protokol->p_next = (PROTOKOLY *) malloc(sizeof(PROTOKOLY))) == NULL) {	// create structure PROTOKOLY
		        fprintf(stderr, "Error malloc help_protokol->p_next: %s\n", strerror(errno));
		        exit(-1);
	        }
	        help_protokol = help_protokol->p_next;
	        strcpy(help_protokol->protokol, s);	// set the name of the new protocol
	        if ((help_protokol->zoznam = (ZAZNAMY *) malloc(sizeof(ZAZNAMY))) == NULL) {	// create structure ZAZNAMY
		    fprintf(stderr, "Error malloc help_protokol->zoznam: %s\n", strerror(errno));
		        exit(-1);
	        }
	        help_protokol->is_ipv6 = is_ipv6ext;
	        help_protokol->zoznam->mac_s = MAC_adr_S;
	        help_protokol->zoznam->mac_d = MAC_adr_D;
	        help_protokol->zoznam->ip_d = 0;
	        help_protokol->zoznam->ip_s = 0;
	        help_protokol->zoznam->ipv6_d = NULL;
	        help_protokol->zoznam->ipv6_s = NULL;
	        if (is_ipv6ext) {
		        if ((help_protokol->zoznam->ipv6_d = (unsigned int *) malloc(sizeof(int) * IPV6SIZE)) == NULL) {
		            fprintf(stderr, "Error malloc help_protokol->zoznam->ipv6_d: %s\n", strerror(errno));
		            exit(-1);
		        }
		        *help_protokol->zoznam->ipv6_d = *IPV6_adr_D;
		        if ((help_protokol->zoznam->ipv6_s = (unsigned int *) malloc(sizeof(int) * IPV6SIZE)) == NULL) {
		            fprintf(stderr, "Error malloc help_protokol->zoznam->ipv6_s: %s\n", strerror(errno));
		            exit(-1);
		        }
		        *help_protokol->zoznam->ipv6_s = *IPV6_adr_S;
	        }
	        help_protokol->zoznam->ip_s = ntohl(IP_adr_S);	// we use ntohl, because of frontend 
	        help_protokol->zoznam->ip_d = ntohl(IP_adr_D);
	        help_protokol->zoznam->pocet_B = pocet_B;
	        help_protokol->zoznam->pocet_ramcov = 1;
	        help_protokol->zoznam->p_next = NULL;
	        help_protokol->zoznam->spracovany[0] = 0;
	        help_protokol->zoznam->spracovany[1] = 0;
	        help_protokol->zoznam->spracovany[2] = 0;
	        help_protokol->zoznam->spracovany[3] = 0;
	        help_protokol->p_next = NULL;
	        help_protokol->empty = 0;
	    }
    }
}

//free structure
void free_protokoly(ZACIATOK_P * p_zac)
{
    ZAZNAMY *help_zaznamy, *p_pomz;	//help structure
    PROTOKOLY *help_protokol, *p_pomp, *p_protokol;

    if (!p_zac->empty) {
	    p_protokol = p_zac->p_protokoly;
	    if (!p_protokol->empty) {
	        for (help_protokol = p_protokol; help_protokol != NULL;) {
		        for (help_zaznamy = help_protokol->zoznam;help_zaznamy != NULL;) {
		            p_pomz = help_zaznamy->p_next;
		            free(help_zaznamy);
		            help_zaznamy = p_pomz;
		        }
		        p_pomp = help_protokol->p_next;
		        if (!strcmp(help_protokol->protokol, "IPV6")) {
		            if (!help_protokol->zoznam->ipv6_d) {
			            free(help_protokol->zoznam->ipv6_d);
			            help_protokol->zoznam->ipv6_d = NULL;
		            }
		            if (!help_protokol->zoznam->ipv6_s) {
			            free(help_protokol->zoznam->ipv6_s);
			            help_protokol->zoznam->ipv6_d = NULL;
		            }
		        }
		        free(help_protokol);
		        help_protokol = p_pomp;
	        }
	    }
	    p_zac->empty = 1;	// set the structure is empty
	    p_zac->p_protokoly = NULL;
    }
}

//zapis strukturu do DB
void *zapis_do_DB_protokoly(void *arg)
{
    PRETAH *pretah1 = (PRETAH *) arg;
    ZACIATOK_P *p_zac;
    p_zac = pretah1->p;
    PROTOKOLY *p_protokol;
    if (!p_zac->empty) {
        for (p_protokol = p_zac->p_protokoly; p_protokol != NULL; p_protokol = p_protokol->p_next) {
            processingl(p_protokol);
        }
    }
    fprintf(stderr, "Zapis do DB...\t[DONE]\n");
    free_protokoly(p_zac);
    free(p_zac);
    free(pretah1);
    return NULL;
}

char compare_IPv6(unsigned int *IP1, unsigned int *IP2)
{
    int i;
    for (i = 0; i < IPV6SIZE; i++)
        if (IP1[i] != IP2[i]) return 0;
    return 1;
}
