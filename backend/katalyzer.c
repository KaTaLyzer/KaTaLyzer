/* Copyright (c) 2006 Roman Benkovic
 * modified by Tomas Kovacik  & Stanislav Bocinec (2008) 
 * modified by Roman Bronis & Andrej Kozemcak (2009)
 * benkovic_roman@post.sk *
 * tokosk16@yahoo.com *
 * svacko@gmail.com *
 * roman.bronis@gmail.com *
 * akozemcak@gmail.com *
*/

#include "katalyzer.h"
#include "processing.h"
#include "variables.h"
#include "wait_function.h"
#include "read_conf.h"
#include "errdef.h"
#include <signal.h>
#include "cronovanie.h"
#include "cdp.h"
#include "snmpp.h"
#include "sip.h"
#include <arpa/inet.h>

struct cdp_struct *cdp_st;

//struktura na pretiahnutie MySQL a dat cez thread - nechutne, ja viem, ale inak sa to neda (aspon co ja viem)
typedef struct {
	MYSQL *d;
	ZACIATOK_P *p;
	KTHREAD *t;
} PRETAH;

int main(int argc, char **argv) {
//         char errbuf[PCAP_ERRBUF_SIZE];
	int o;
//         int snaplen=65535;
	int i_is_config = 0;
	IPV6_adr_D = NULL;
	IPV6_adr_S = NULL;
	char offilename[255];
	KTHREAD *kt1, *kt2;

	while((o=getopt(argc,argv,":hwc:df:")) != -1 ) {  // usage of getopt() is that if you expect argument than you specify colon ':' after the option (i.e. here we expect interface name after -i)
		switch(o)
		{ 
			case 'h':
				help();
				exit(OK);
                        case 'w':
                                wait = 0;  // if we do not want to wait for new minute, we start analyzator with parameter 'w'
				break;
			case 'c':
				i_is_config = 1;
				config_name = optarg;
				break;
			case 'd':
				debug = 1;
				break;
			case 'f':
			  fprintf(stderr,"WARNING: DANGEROUS FUNCTION. PRESS CTRL+C TO EXIT OR ANY KEY TO CONTINUE.\n");
			//  getchar();
			  isoffline = 1;
			  strcpy(offilename,optarg);
			  break;
			default:
				help();
				exit(OK);
		}
	}

	if (!i_is_config)
	{
		printf ("*** Error: please specify config file!\n");
		help();
		exit (ERR_NO_CONF);
	}
	s_tmp_log = (char*) malloc (5000*sizeof(char));
	s_tmp_str = (char*) malloc (200*sizeof(char));

	printf ("*** KaTaLyzer backend started, time: %ld\n",time(&actual_time));
	
	if (debug) printf ("*** Debug mode is enabled!!!\n");

        read_conf(); // function for reading configuration from config file

//SIP
	if(debug) {
		printf("*** protocol_sip = %d\n",protocol_sip);
#ifdef CDP_P
		printf("*** protocol CDP\n");
#endif
	}
	
#ifdef _CAPTURE

  struct k_capture c;
  
  raw_init(&c, "wlan0");
  
#else
	if(isoffline){
	  FILE *offile;
	  offile = fopen(offilename,"r");
	  if(offile == NULL){
	   fprintf(stderr,"Error: opening offline file\n");
	   return -1;
	  }
	  
	  if((fp=pcap_fopen_offline(offile,errbuf))==NULL){ //open and capture traffic from offline file
	    fprintf(stderr,"Error: pcap offline failure\n");
	    return -1;
	  }
	}
	else{
	  if((fp=pcap_open_live(interface,snaplen,1,0,errbuf))==NULL) //open and capture traffic from specified interface; interface is used in promiscuous mode, so you need to be 'root'  to open it
       	// snaplen is maximum length of the frame which is catched, the rest is dumped 
	  {
	    fprintf(stderr,"Error capturing traffic on %s network interface: %s",interface, errbuf);
	    exit(ERR_OPEN_IF);
	  }
	}
#endif

	// we wait after opening network adapter - we do not need to wait for error message if we are not able to open the adapter
        
	if( wait == 1 ) waiting(); // here we wait until new minute beggins

	if(!isoffline)
	  time( &beggining_time );  // beggining_time is time of start of the program. it is incremented by casovac_zapisu timer after each "casovac_zapisu" seconds

#ifdef CDP_P
	cdp_st=(struct cdp_struct*) malloc(sizeof(struct cdp_struct));
	cdp_st->empty=1;
#endif

#ifdef SNMP_P
	sn.p_snmp=NULL;
	sn.p_next=NULL;
	sn.empty=1;
#endif

	z_protokoly.empty=1;				//array is free
	z_protokoly.p_protokoly=NULL;
	z_pair_array=NULL;
#ifdef _CAPTURE
  k_loop(&c, dispatcher_handler);
#else
	pcap_loop(fp,0,dispatcher_handler,NULL);
	
	pcap_close(fp); 	//closing the interface
#endif
	
//cakame na beziace vlakna a potom uvolnime strukturu	
	for(kt1=p_thread;kt1!=NULL;){
	  pthread_join(kt1->zapdb, NULL);
	  kt2=kt1->p_next;
	  if(kt1)
	    free(kt1);
	  kt1=kt2;
	}

	if (debug) printf("*** End\n");
		
	return (OK);
}

void help()
{
	printf("\nKaTaLyzer\n");
	printf("Usage: ./katalyzer [-h] [-w] [-c config_file] [-d] [-f file]\n");
	printf("-h print this help -h\n");
        printf("-w disable waiting for new minute after start of analyzator(e.g. in case we debug this program)\n");
	printf("-c sets path to configuration file (e.g. /tmp/my_config.conf); do not use space " " in the path\n");
        printf("-d sets debug mode on\n");
	printf("-f sets offline mode and path to offline capture file. WARNING: DANGEROUS");
}

//toto spusta funkcia pcap_loop - tu sa robi analyza prevadzky
#ifdef _CAPTURE
void dispatcher_handler(const struct k_header *header, const u_char *pkt_data) {
#else
void dispatcher_handler(u_char *dump, const struct pcap_pkthdr *header, const u_char *pkt_data) {
#endif
	int lng_type;		// here is stored value from frame - considering this value we have to decide whether it is length of the frame or type 
        //time_t actual_time;	// actual time is stored here
//         char temp[15];          // temp - variable for converting IP address from INTEGER into STRING - we put IP address into DB in a.b.c.d format
//         int prepinac = 0;	// switching variable for searching in arrays and inserting new IP/MAC addresses into the array if it does not exist there yet
	char protokol[9];	// pomocna premena

#ifdef CHCEM_POCTY
	//increment number of packets
	ppaketov++;
#endif

	MAC_adr_S=0;
	MAC_adr_D=0;
	IP_adr_S=0;
	IP_adr_D=0;
	is_ipv6ext=0;
	if(IPV6_adr_D)
	  free(IPV6_adr_D);
	IPV6_adr_D = NULL;
	if(IPV6_adr_S)
	  free(IPV6_adr_S);
	IPV6_adr_S = NULL;
	
	if(isoffline){
	  if(!isfirsttime){
	    beggining_time=header->ts.tv_sec;
	    beggining_time/=10;
	    beggining_time*=10;
	    isfirsttime = 1;
	  }
	  unix_time = header->ts.tv_sec;
	  actual_time = header->ts.tv_sec;
	}
	else
	  unix_time = time(&actual_time); // unix_time - variable for inserting time info into DB
		
	pocet_B = header->len; 	// pocet_B - number of Bytes captured in frame - needed for counting amount of traffic 

				// tuto moze byt problem/chyba v poctoch bajtov, pretoze neviem, kt. premennu pouzivat - header->len alebo header->caplen

	ethh=(struct ether_header*) pkt_data; // we store Ether_header
	lng_type=ntohs(ethh->ether_type); // lng_type - LENGHT/TYPE value
	
	//is this frame ETHERNET II or IEEE 802.3
	if (lng_type>1500) eth2_frame(pkt_data,lng_type);		//here we go to inspect captured ETHERNET II frame closely
	else ieee802(pkt_data,lng_type);		//here we go to inspect captured IEEE 802.3 frame closely

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// processing data to be written into database
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////
///// data-link layer
/////////////////////

/// ETHERNET
	if(protocol_eth == 1 && strcmp(Eor8, "E") == 0) {// if we specified to watch ETHERNET II frames in config file and we catched 'E'(it means ETHERNET II frame), we adjust particular statistics
		strcpy(protokol,"E");
		m_protokoly(&z_protokoly,protokol);
	}

/// 802.3
	if(protocol_8023 == 1 && strcmp(Eor8, "8") == 0) { 
		strcpy(protokol,"8");
		m_protokoly(&z_protokoly,protokol);
        }

///////////////////
///// network layer
///////////////////

/// IP protocol
        if(protocol_ip == 1 && strcmp(net_proto, "IP") == 0) {
		strcpy(protokol,"IP");
		m_protokoly(&z_protokoly,protokol);
        }

	if(protocol_ipv6 == 1 && strcmp(net_proto, "IPv6") == 0){
		strcpy(protokol,"IPv6");
		m_protokoly(&z_protokoly,protokol);
	}

/// ARP
	if(protocol_arp == 1 && strcmp(net_proto, "ARP") == 0) {
		strcpy(protokol,"ARP");
		m_protokoly(&z_protokoly,protokol);
        }

/// RARP
	if(protocol_rarp == 1 && strcmp(net_proto, "RARP") == 0) {
		strcpy(protokol,"RARP");
		m_protokoly(&z_protokoly,protokol);
 	}

/// IPX protocol
	if(protocol_ipx == 1 && strcmp(net_proto, "IPX") == 0) {
		strcpy(protokol,"IPX");
		m_protokoly(&z_protokoly,protokol);
        }

/////////////////////
// transport layer
/////////////////////

/// TCP protocol
        if(protocol_tcp == 1 && strcmp(trans_layer, "TCP") == 0) {
		strcpy(protokol,"TCP");
		m_protokoly(&z_protokoly,protokol);
        }

/// UDP protocol
	if(protocol_udp == 1 && strcmp(trans_layer, "UDP") == 0) {
		strcpy(protokol,"UDP");
		m_protokoly(&z_protokoly,protokol);
        }

///  ICMP PROTOKOL
	if(protocol_icmp == 1 && strcmp(trans_layer, "ICMP") == 0) {
		strcpy(protokol,"ICMP");
		m_protokoly(&z_protokoly,protokol);
        }
        
///  ICMPv6 PROTOKOL
        if(protocol_icmp == 1 && strcmp(trans_layer, "ICMPv6") == 0){
	  strcpy(protokol,"ICMPv6");
	  m_protokoly(&z_protokoly,protokol);
        }
		
/// IGMP protokol
	if(protocol_igmp == 1 && strcmp(trans_layer, "IGMP") == 0) {
		strcpy(protokol,"IGMP");
		m_protokoly(&z_protokoly,protokol);
	}

/////////////////////
// APPLICATION LAYER
////////////////////

////////////
// TCP ports
////////////

	int j;
	for(j=0;j<i_TCP; j++) {
		// variable 'protocol' contains source port number information, druhy_protocol contains destination port number information
		// if either one of them is the one we are watching, we adjust statistics
		if(s_protocol == pole_TCP_portov[j] || d_protocol == pole_TCP_portov[j]) {			
			if(s_protocol == pole_TCP_portov[j]) sprintf(protokol,"TCP%d",s_protocol);
			if(d_protocol == pole_TCP_portov[j]) sprintf(protokol,"TCP%d",d_protocol);
			m_protokoly(&z_protokoly,protokol);
		}
	}

	////////////
	// UDP ports
	////////////
	for(j=0;j<i_UDP; j++){
		if(s_protocol == pole_UDP_portov[j] || d_protocol == pole_UDP_portov[j]) {
			if(s_protocol == pole_UDP_portov[j]) sprintf(protokol,"UDP%d",s_protocol);
			if(d_protocol == pole_UDP_portov[j]) sprintf(protokol,"UDP%d",d_protocol);
			m_protokoly(&z_protokoly,protokol);
		}

	}

//////////////////////////////////////////////////////////////////////
//// pairing IP with MAC addresses
////////////////////////////////////////////////////////////////////////

	if(z_pair_array==NULL) {
		z_pair_array=(PAIR_ARRAY*) malloc(sizeof(PAIR_ARRAY));
		z_pair_array->mac_d=MAC_adr_D;
		z_pair_array->mac_s=MAC_adr_S;
		if(is_ipv6ext){
		  z_pair_array->is_ipv6=is_ipv6ext;
		  z_pair_array->ipv6_d=IPV6_adr_D;
		  z_pair_array->ipv6_s=IPV6_adr_S;
		}
		else{
		  z_pair_array->is_ipv6=is_ipv6ext;
		  z_pair_array->ip_d=ntohl(IP_adr_D);
		  z_pair_array->ip_s=ntohl(IP_adr_S);
		}
		z_pair_array->p_next=NULL;
	}
	else {
		PAIR_ARRAY *pom_array;
		int find=0;
		for(pom_array=z_pair_array;pom_array!=NULL;pom_array=pom_array->p_next) {
			find=0;
			if((pom_array->mac_s==MAC_adr_S) && (pom_array->mac_d==MAC_adr_D)) {
				if(( !is_ipv6ext && (pom_array->is_ipv6 == is_ipv6ext) && (pom_array->ip_s==ntohl(IP_adr_S)) && (pom_array->ip_d==ntohl(IP_adr_D))) || ( is_ipv6ext && (pom_array->is_ipv6 == is_ipv6ext) && (compare_IPv6(pom_array->ipv6_s, IPV6_adr_S) && compare_IPv6(pom_array->ipv6_d, IPV6_adr_D)))) {
					find=1;
					break;
				}
			}
		}
		if(!find) {
			for(pom_array=z_pair_array;pom_array->p_next!=NULL;pom_array=pom_array->p_next) ;
			pom_array->p_next=(PAIR_ARRAY*) malloc(sizeof(PAIR_ARRAY));
			pom_array=pom_array->p_next;
			pom_array->mac_d=MAC_adr_D;
			pom_array->mac_s=MAC_adr_S;
			if(is_ipv6ext){
			  pom_array->is_ipv6=is_ipv6ext;
			  pom_array->ipv6_d=IPV6_adr_D;
			  pom_array->ipv6_s=IPV6_adr_S;
			}
			else{
			  pom_array->is_ipv6=is_ipv6ext;
			  pom_array->ip_d=ntohl(IP_adr_D);
			  pom_array->ip_s=ntohl(IP_adr_S);
			}
			pom_array->p_next=NULL;
		}
	}

	if((flag == 0 && (actual_time - beggining_time) >= casovac_zapisu) || ((actual_time - beggining_time) >= casovac_zapisu*2)) { // flag - were data written into DB in this time interval? 
	        MYSQL *conn;
                conn = mysql_init(NULL);
		char prikaz[20000];
// 		char temp_str[10000];

		if((actual_time - beggining_time) >= casovac_zapisu*2) processing_time+=60;
		else processing_time=unix_time;

		//ak je prilis velky rozdiel medzi aktualnym casom a poslednim ulozenim do databazy tak nastavi aktualny cas
		if((actual_time - beggining_time) >= casovac_zapisu*2) beggining_time=actual_time - casovac_zapisu;

#ifdef CHCEM_POCTY
		FILE *fw;
		if((fw=fopen("ppaketov.txt","a"))!=NULL) {
			fprintf(fw,"%d\n",ppaketov);
			ppaketov=0;
			if(fclose(fw)==EOF) fprintf(stderr,"Neviem zavriet subor ppaketov.txt!\n");
		}
		else fprintf(stderr,"Neviem otvorit subor ppaketov.txt pre dopis!\n");
#endif

//////////////////////////////////////////////////////////////////////
// connecting to DB
//////////////////////////////////////////////////////////////////////
		if (mysql_real_connect(conn, db_host, db_user, db_pass, NULL, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL) {
			fprintf(stderr,"Failed to connect to MYSQL database: Error: %s\n", mysql_error(conn));
			exit (ERR_MYSQL_CON);
		}

//creating DB if it does not exist and connecting to it
                sprintf(prikaz,"CREATE DATABASE IF NOT EXISTS %s; USE %s;",db_name, db_name);
                if (mysql_query(conn, prikaz)) {
			fprintf(stderr,"Failed to create MYSQL database %s: %s\n",db_name, mysql_error(conn));
			exit (ERR_MYSQL_DB_CREATE);
		}
		mysql_next_result(conn);
		
		// creating DB tables 

		if(!(z_protokoly.empty)) {
			PROTOKOLY *p_prot;
			for(p_prot=z_protokoly.p_protokoly;p_prot!=NULL;p_prot=p_prot->p_next){
				sprintf(prikaz,""); // clearing 'prikaz' because we use 'strcat' to put commands into 'prikaz'
				if(!p_prot->is_ipv6)
				  sprintf(prikaz,"CREATE TABLE IF NOT EXISTS %s_1m_time (`time` int(10) unsigned NOT NULL default '0',`IP_id` int(10) unsigned NOT NULL default '0',`MAC_id` int(10) unsigned NOT NULL default '0',`IP_SD_id` int(10) unsigned NOT NULL default '0',`MAC_SD_id` int(10) unsigned NOT NULL default '0',PRIMARY KEY (`time`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_1m_IP(`id` int(10) unsigned NOT NULL auto_increment,`IP` int unsigned default '0',`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` mediumint(8) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_1m_MAC(`id` int(10) unsigned NOT NULL auto_increment,`MAC` bigint unsigned default '0',`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` mediumint(8) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_1m_IP_SD(`id` int(10) unsigned NOT NULL auto_increment,`IP_1` int unsigned default '0',`IP_2` int unsigned default '0',`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` mediumint(8) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_1m_MAC_SD(`id` int(10) unsigned NOT NULL auto_increment,`MAC_1` bigint unsigned default '0',`MAC_2` bigint unsigned default '0',`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` mediumint(8) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;",p_prot->protokol, p_prot->protokol, p_prot->protokol, p_prot->protokol,p_prot->protokol);
				else
				  sprintf(prikaz,"CREATE TABLE IF NOT EXISTS %s_v6_1m_time (`time` int(10) unsigned NOT NULL default '0',`IP_id` int(10) unsigned NOT NULL default '0',`MAC_id` int(10) unsigned NOT NULL default '0',`IP_SD_id` int(10) unsigned NOT NULL default '0',`MAC_SD_id` int(10) unsigned NOT NULL default '0',PRIMARY KEY (`time`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_v6_1m_IP(`id` int(10) unsigned NOT NULL auto_increment,`IP` char(32),`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` mediumint(8) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_v6_1m_MAC(`id` int(10) unsigned NOT NULL auto_increment,`MAC` bigint unsigned default '0',`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` mediumint(8) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_v6_1m_IP_SD(`id` int(10) unsigned NOT NULL auto_increment,`IP_1` char(32), `IP_2` char(32),`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` mediumint(8) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_v6_1m_MAC_SD(`id` int(10) unsigned NOT NULL auto_increment,`MAC_1` bigint unsigned default '0',`MAC_2` bigint unsigned default '0',`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` mediumint(8) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` mediumint(8) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;",p_prot->protokol, p_prot->protokol, p_prot->protokol, p_prot->protokol,p_prot->protokol);

				if(mysql_query(conn, prikaz)){
					fprintf(stderr,"Failed to create %s tables in MYSQL database %s: %s\nPrikaz: %s\n",p_prot->protokol ,db_name, mysql_error(conn), prikaz);
					exit (ERR_MYSQL_TBL_CREATE);
				}
				int ii;
				// 5 is number of commands sent into DB - we have to release results of their execution
				for (ii=0;ii<5;ii++) mysql_next_result(conn);
			}
		}

		if(z_pair_array!=NULL) {
			sprintf(prikaz,""); // clearing 'prikaz' because we use 'strcat' to put commands into 'prikaz'
			sprintf(prikaz,"CREATE TABLE IF NOT EXISTS IPlist (`IP` int unsigned default '0', `MAC` bigint unsigned default '0',  PRIMARY KEY(`IP`)) ENGINE=MyISAM;");
			if(mysql_query(conn, prikaz)){
				fprintf(stderr,"Failed to create IPlist tables in MYSQL database %s: %s\n",db_name, mysql_error(conn));
				exit (ERR_MYSQL_TBL_CREATE);
			}
			mysql_next_result(conn);
			
			sprintf(prikaz,""); // clearing 'prikaz' because we use 'strcat' to put commands into 'prikaz'
			sprintf(prikaz,"CREATE TABLE IF NOT EXISTS IPv6list (`IP` char(32), `MAC` bigint unsigned default '0',  PRIMARY KEY(`IP`)) ENGINE=MyISAM;");

			if(mysql_query(conn, prikaz)){
				fprintf(stderr,"Failed to create IPlist tables in MYSQL database %s: %s\n",db_name, mysql_error(conn));
				exit (ERR_MYSQL_TBL_CREATE);
			}
			mysql_next_result(conn);

		}

		// inserting data into table IPlist
		//printf("\n\nNEW WAY:\n");

		if(z_pair_array!=NULL) {
			PAIR_ARRAY *pom_array;
			for(pom_array=z_pair_array;pom_array!=NULL;pom_array=pom_array->p_next) {
			  if(!pom_array->is_ipv6){
			    sprintf(prikaz,"INSERT INTO IPlist (IP, MAC) VALUES ('%u','%llu')ON DUPLICATE KEY UPDATE MAC='%llu';",pom_array->ip_s, pom_array->mac_s, pom_array->mac_s);
			    if (mysql_query(conn, prikaz)) {
			      fprintf(stderr,"Failed to insert data into MYSQL database %s: %s\n",db_name, mysql_error(conn));
			    }
			    sprintf(prikaz,"INSERT INTO IPlist (IP, MAC) VALUES ('%u','%llu')ON DUPLICATE KEY UPDATE MAC='%llu';",pom_array->ip_d, pom_array->mac_d, pom_array->mac_d);
			    if (mysql_query(conn, prikaz)) {
			      fprintf(stderr,"Failed to insert data into MYSQL database %s: %s\n",db_name, mysql_error(conn));
			    }
			  }
			  else{
			    sprintf(prikaz,"INSERT INTO IPv6list (IP, MAC) VALUES ('%08x%08x%08x%08x','%llu')ON DUPLICATE KEY UPDATE MAC='%llu';",pom_array->ipv6_s[0], pom_array->ipv6_s[1], pom_array->ipv6_s[2], pom_array->ipv6_s[3], pom_array->mac_s, pom_array->mac_s);
			    if (mysql_query(conn, prikaz)) {
			      fprintf(stderr,"Failed to insert data into MYSQL database %s: %s\n",db_name, mysql_error(conn));
			    }
			    sprintf(prikaz,"INSERT INTO IPv6list (IP, MAC) VALUES ('%08x%08x%08x%08x','%llu')ON DUPLICATE KEY UPDATE MAC='%llu';",pom_array->ipv6_d[0], pom_array->ipv6_d[1], pom_array->ipv6_d[2], pom_array->ipv6_d[3], pom_array->mac_d, pom_array->mac_d);
			    if (mysql_query(conn, prikaz)) {
			      fprintf(stderr,"Failed to insert data into MYSQL database %s: %s\n",db_name, mysql_error(conn));
			    }
			  }
			}
		}

#ifdef _SIP
		// SIP table creating
		if(protocol_sip==1) SIP_vytvor_db(conn);
		// SIP data into DB
		if(protocol_sip==1) SIP_zapis_do_db(conn); 
#endif


#ifdef CDP_P
	sprintf(prikaz,""); // clearing 'prikaz' because we use 'strcat' to put commands into 'prikaz'
	sprintf(prikaz,"CREATE TABLE IF NOT EXISTS `CDP_1mm_time`(`time` int(10) unsigned NOT NULL default '0',`id` int(10) unsigned NOT NULL default '0',PRIMARY KEY (`time`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS `CDP_1mm`(`id` int(10) unsigned NOT NULL auto_increment,`ttl` int(10) unsigned NOT NULL default '0',`device_ID` varchar(20) NOT NULL default '',`Capabilities` varchar(200) NOT NULL default '',`version` varchar(255) NOT NULL default '',`platform` varchar(100) NOT NULL default '',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS `CDP_ADDRS_1m`(`id` int(10) unsigned NOT NULL,`IP` int unsigned default '0') ENGINE=MyISAM DEFAULT CHARSET=latin1;");

	if(mysql_query(conn, prikaz)){
		fprintf(stderr,"Failed to create CDP tables in MYSQL database %s: %s\n",db_name, mysql_error(conn));
		sprintf(s_tmp_str,"%s:katalyzer.cpp:dispatcher_handler:Failed to create CDP tables in MYSQL database %s: %ld\n", db_name, mysql_error(conn),time(&actual_time));
	}
	int ii;
	for (ii=0;ii<3;ii++) {// 3 is number of commands sent into DB - we have to release results of their execution
		mysql_next_result(conn);
	}

	cdp_processing(cdp_st ,conn);

	if(!cdp_st->empty){
		struct cdp_struct *pom_cdp;
		struct address *pom_adr1;
		struct address *pom_adr2;
		while(cdp_st!=NULL){
			pom_cdp=cdp_st->p_next;
			cdp_st->empty=1;
			pom_adr1=cdp_st->p_addres;
			while(pom_adr1!=NULL){
				pom_adr2=pom_adr1->p_next;
				free(pom_adr1);
				pom_adr1=pom_adr2;
			}
			if(cdp_st->device_id!=NULL)
				free(cdp_st->device_id);
			if(cdp_st->port_id!=NULL)
				free(cdp_st->port_id);
			if(cdp_st->version!=NULL)
				free(cdp_st->version);
			if(cdp_st->platform!=NULL)
				free(cdp_st->platform);
			free(cdp_st);
			cdp_st=pom_cdp;
		}
		cdp_st=(struct cdp_struct*) malloc(sizeof(struct cdp_struct));
		cdp_st->empty=1;
	}

#endif

#ifdef SNMP_P
	if(protocol_snmp == 1){
		sprintf(prikaz,"");
		sprintf(prikaz,"CREATE TABLE IF NOT EXISTS `SNMP_time` (`time` int(10) unsigned NOT NULL default '0',`id` int(10) unsigned NOT NULL default '0', PRIMARY KEY (`time`)) ENGINE=MyISAM DEFAULT CHARSET=latin1; CREATE TABLE IF NOT EXISTS `SNMP_1m` (`id` int(10) unsigned NOT NULL auto_increment, `IP_S` int unsigned NOT NULL default '0',`IP_D` int unsigned NOT NULL default '0',`bytes` bigint(20) unsigned NOT NULL default '0', `number` mediumint(8) unsigned NOT NULL default '0', `SNMP` mediumtext NOT NULL default '',PRIMARY KEY (`id`))ENGINE=MyISAM DEFAULT CHARSET=latin1;");
		if(mysql_query(conn, prikaz)) 
		{
			fprintf(stderr,"Failed to create SNMP tables in MYSQL database %s: %s\n",db_name, mysql_error(conn));
		}
		int ii;
		for(ii=0;ii<2;ii++) // see i_TCP clearing notice
		{
			mysql_next_result(conn);
		}

		procesing_snmp(sn, conn);

		if(sn.p_next!=NULL){
			SNMP_STRUK *pom1, *pom;
			for(pom=sn.p_next;pom!=NULL;pom=pom1){
				pom->empty=1;
				pom->number=0;
				pom->bytes=0;
				free(pom->p_snmp);
				pom1=pom->p_next;
				free(pom);
			}
		}
		sn.empty=1;
		sn.number=0;
		sn.bytes=0;
		sn.p_next=NULL;


	}
#endif

		// FLUSH DATA INTO DB - 26.7.2010
		//vytvorenie "kopie" z_protokoly do z_protokoly2
		ZACIATOK_P z_protokoly2;
		z_protokoly2.empty=0;
		z_protokoly2.p_protokoly=z_protokoly.p_protokoly;
		z_protokoly.empty=1;
		z_protokoly.p_protokoly=NULL;
		
		PRETAH pretah1;
		KTHREAD *kt;
		kt = create_thread(&p_thread);
		kt->run=1;
		pretah1.p=&z_protokoly2;
		pretah1.d=conn;
		pretah1.t=kt;
		pthread_t zapdb = kt->zapdb;
		pthread_create(&zapdb,NULL,zapis_do_DB_protokoly,(void *)&pretah1);

		flag = 1; // after processing the frame we just processed we set 'flag' to true which indicates that data in this time interval were already processed and in this second we are not going to insert any more data into DB - we will do it after passing 'casovac_zapisu' seconds
		
// clear pairing array
		if(z_pair_array!=NULL) {
			PAIR_ARRAY *pom_array, *pom_array1;
			for(pom_array=z_pair_array;pom_array!=NULL;){
				pom_array1=pom_array->p_next;
				pom_array->p_next=NULL;
				free(pom_array);
				pom_array=pom_array1;
			}
		}
		z_pair_array=NULL;


		beggining_time += casovac_zapisu;  // add 'casovac_zapisu' to 'begginig_time' means that we already processed the data for last time interval(which started at 'beggining_time'), we are going to calculate statistics for ner time interval

		// we predict that time interval between incoming two frames will not be bigger than 'casovac_zapisu' - it is very unlikely to happen(minimum ARP packets arrive very often)

//////////////
//SUM TABLES
//////////////
		interval++;
		pthread_t cron;
		int casy[4] = {5,30,2,1}, i=0;
		int modu[4] = {5, 30, 120, 1440};
		int rozd[4] = {1, 5, 30, 120};
		for (i=0;i<4;i++) {
			if(i==0) fprintf(stderr,"interval:%d\n",interval);
			if ((interval%modu[i]==rozd[i]) && (interval!=rozd[i])) {
				switch(pthread_create(&cron,NULL,cronovanie,(void *)casy[i])) {
					case -1: fprintf(stderr,"zlyhal thread\n");
						 break;
					case 0: fprintf(stderr,"\ncronovanie(%d) [DONE]\n",casy[i]);			
						break;				 
					default: break;
				}
			}			
		}
		
	}
		
	if(flag == 1 && actual_time - beggining_time >= 10) { // 10 seconds is considered enough time interval as we measure with minimum resolution of 60seconds and it is very probable that there will be another frame comming in that 10 seconds - it will set 'flag' again so we can insert data into DB again after'casovac_zapisu' 
		flag = 0;
		//printf("nastavenie flagu na nulu: flag = %d", flag);
	}

// clearing variables - it is necesary only for strings, unix_time and pocet_B might not be cleared as they are integers which are set not incremented
	unix_time=0;
	pocet_B=0;
	//pocet_ramcov=0;
	Eor8[0]=0;
	MAC_adr_S=0;
	MAC_adr_D=0;
	net_proto[0]=0;
	trans_layer[0]=0;
	protocol=0;	
	s_protocol=0;
	d_protocol=0;
}


void eth2_frame(const u_char *pkt_data,int type)
{
	// we received ETHERNET II frame - we need to set variable Eor8
	sprintf(Eor8,"E");

	int i;
	MAC_adr_S=0;
	MAC_adr_D=0;
	for(i=0;i<ETH_ALEN;i++){
		MAC_adr_S<<=8;
		MAC_adr_S+=ethh->ether_shost[i];
	}

	for(i=0;i<ETH_ALEN;i++){
		MAC_adr_D<<=8;
		MAC_adr_D+=ethh->ether_dhost[i];
	}

	net_protokol(type,net_proto);
	
	if(type==2048) ip_protokol(pkt_data, 0);	// if we received IP or ARP protocol, analysis continues
	if(type==34525) ipv6_protokol(pkt_data, 0); //IPv6 to be implemented
	if(type==2054) arp_protokol(pkt_data);
	
}

void arp_protokol(const u_char *pkt_data)
{
// 	int l_prot,op_code;
	int i, j;
	arph=(struct arphdr*) (pkt_data + sizeof(struct ether_header));

// 	l_prot=ntohs(arph->ar_hrd);
// 	op_code=ntohs(arph->ar_op);

	MAC_adr_S=0;
	MAC_adr_D=0;
	j=(sizeof(struct ether_header) + sizeof(struct arphdr));
	for(i=0;i<ETH_ALEN;i++){
		MAC_adr_S<<=8;
		MAC_adr_S+=pkt_data[j+i];
	}
	j=(sizeof(struct ether_header) + sizeof(struct arphdr) + arph->ar_hln + arph->ar_pln);
	for(i=0;i<ETH_ALEN;i++){
		MAC_adr_D<<=8;
		MAC_adr_D+=pkt_data[j+i];
	}

	//sprintf(MAC_adr,"%.2x%.2x%.2x%.2x%.2x%.2x",pkt_data[22],pkt_data[23],pkt_data[24],pkt_data[25],pkt_data[26],pkt_data[27]);
	
	IP_adr_D=0;
	IP_adr_S=0;

	j=(sizeof(struct ether_header) + sizeof(struct arphdr) + arph->ar_hln + arph->ar_pln);
	j--;
	for(i=0;i<4;i++){
		IP_adr_S<<=8;
		IP_adr_S+=pkt_data[j-i];
	}
	j=(sizeof(struct ether_header) + sizeof(struct arphdr) + arph->ar_hln*2 + arph->ar_pln*2);
	j--;
	for(i=0;i<4;i++){
		IP_adr_D<<=8;
		IP_adr_D+=pkt_data[j-i];
	}
}

void ip_protokol(const u_char *pkt_data, int len)
{
	int protocol;
	
	is_ipv6ext = 0;

	iph=(struct iphdr*) (pkt_data + (sizeof(struct ether_header)) + len);	//store IP header structure
	protocol=iph->protocol;			//protocol inside IP packet

	IP_adr_S=iph->saddr;
	IP_adr_D=iph->daddr;


	trans_protokol(protocol,trans_layer);
	
	len = iph->ihl*4;

	if(protocol==6) tcp_protokol(pkt_data,len);		//we continue analysis in case there is TCP, UDP or ICMP protocol inside IP packet
//	if(protocol==1) icmp_protokol(pkt_data,len);
	if(protocol==17) udp_protokol(pkt_data,len);
}

void ipv6_protokol(const u_char *pkt_data, int len)
{
  int protocol, i;
  char end=0;
  unsigned int *ipv6_pom;
  struct ip6_hbh *ip6hop;
  struct ip6_rthdr *ip6r;
  struct ip6_frag *ip6f;
  struct ip6_dest *ip6d;
  
  is_ipv6ext = 1;
  
  iphv6=(struct ip6_hdr*) (pkt_data + (sizeof(struct ether_header)) + len);
  protocol=iphv6->ip6_ctlun.ip6_un1.ip6_un1_nxt;
  
  len=sizeof(struct ip6_hdr);
  
  //prejdeme celu hlavicku ipv6
  for(;;){
    switch(protocol){
      case IPPROTO_HOPOPTS:
	ip6hop=(struct ip6_hbh*) (pkt_data + (sizeof(struct ether_header)) + len);
	protocol=ip6hop->ip6h_nxt;
	len+=sizeof(struct ip6_hbh);
	break;
      case IPPROTO_ROUTING:
	ip6r=(struct ip6_rthdr*) (pkt_data + (sizeof(struct ether_header)) + len);
	protocol=ip6r->ip6r_nxt;
	len+=sizeof(struct ip6_rthdr);
	break;
      case IPPROTO_FRAGMENT:
	ip6f=(struct ip6_frag*) (pkt_data + (sizeof(struct ether_header)) + len);
	protocol=ip6f->ip6f_nxt;
	len+=sizeof(struct ip6_frag);
	break;
      case IPPROTO_ESP:
	break;
      case IPPROTO_AH:
	break;
      case IPPROTO_DSTOPTS:
	ip6d=(struct ip6_dest*) (pkt_data + (sizeof(struct ether_header)) + len);
	protocol=ip6d->ip6d_nxt;
	len+=sizeof(struct ip6_dest);
	break;
      case IPPROTO_TCP:
	trans_protokol(protocol,trans_layer);
	tcp_protokol(pkt_data, len);		//we continue analysis in case there is TCP, UDP or ICMP protocol inside IP packet
	end=1;
	break;
      case IPPROTO_UDP:
	trans_protokol(protocol,trans_layer);
	udp_protokol(pkt_data, len);
	end=1;
	break;
      case IPPROTO_ICMPV6:
	strcpy(trans_layer,"ICMPv6");
	end=1;
	break;
      default:
	end=1;
	break;
    }
    //end ipv6 header
    if(end)
      break;
  }
  
  if((IPV6_adr_D =(unsigned int*) malloc(sizeof(unsigned int)*IPV6SIZE)) == NULL){
    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
    return;
  }
  if((IPV6_adr_S =(unsigned int*) malloc(sizeof(unsigned int)*IPV6SIZE)) == NULL){
    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
    return;
  }
  
  ipv6_pom =(unsigned int*) &iphv6->ip6_src; 
  
  for(i = 0;i<4;i++){
    IPV6_adr_S[i] = ntohl(ipv6_pom[i]);
  }
  
  ipv6_pom =(unsigned int*) &iphv6->ip6_dst;
  
  for(i = 0;i<4; i++){
    IPV6_adr_D[i] = ntohl(ipv6_pom[i]);
  }
  
  
}

void trans_protokol(int number, char *protokols) { //according to number from IP header we assign name of the protocol to the number; adding new protocol takes just a few seconds (ICMP RFC 792)
	char protokol[5];
	switch(number) {
		case 1:
			sprintf(protokol,"ICMP");
			break;
		case 2:
			sprintf(protokol,"IGMP");
			break;
		case 6:
			sprintf(protokol,"TCP");
			break;
		case 8:
			sprintf(protokol,"IGP");
			break;
		case 9:
			sprintf(protokol,"EGP");
			break;
		case 17:
			sprintf(protokol,"UDP");
			break;
		default:
			sprintf(protokol,"%d",number);
			break;
	}
	strcpy(protokols, protokol);
}


void tcp_protokol(const u_char *pkt_data,int len) {
	unsigned int s_port,d_port;
// 	unsigned int port_id,sn_port,dn_port;

	tcph=(struct tcphdr*) (pkt_data + sizeof(struct ether_header) + len);

	s_port=ntohs(tcph->source);
// 	sn_port=tcph->source;
	d_port=ntohs(tcph->dest);
// 	dn_port=tcph->dest;

/*
	a=14+h_len;					//a->begining of TCP header
	s_port=pkt_data[a]*256+pkt_data[a+1];		//source port
	sn_port=pkt_data[a+1]*256+pkt_data[a];		//network order
	d_port=pkt_data[a+2]*256+pkt_data[a+3];		//destination port
	dn_port=pkt_data[a+3]*256+pkt_data[a+2];	//network order
*/

	s_protocol = s_port;
	d_protocol = d_port;

//SIP
	int a = sizeof(struct ether_header) + iph->ihl*4 + len;
	if(protocol_sip==1) {	
		int s_hlen = (4*(pkt_data[a+12]/16))+a; //do premennej s_hlen ulozi adresu zaciatku TCP udajovej casti podla Offset pola
		SIP_detekcia(pkt_data,s_hlen);
	}

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

void udp_protokol(const u_char *pkt_data,int len) {
	int s_port,d_port, port_id;
// 	int sn_port,dn_port;

	udph=(struct udphdr*) (pkt_data + sizeof(struct ether_header) + len);

	s_port=ntohs(udph->source);
// 	sn_port=udph->source;		//network order
	d_port=ntohs(udph->dest);
// 	dn_port=udph->dest;		//network order
/*
	a=14+h_len;					//a->begining of UDP header
	s_port=pkt_data[a]*256+pkt_data[a+1];
	sn_port=pkt_data[a+1]*256+pkt_data[a];		//network order
	d_port=pkt_data[a+2]*256+pkt_data[a+3];
	dn_port=pkt_data[a+3]*256+pkt_data[a+2];	//network order
*/
	if(s_port<1024 || s_port==8080) port_id=s_port;
	else port_id=d_port;

#ifdef SNMP_P	
	if(s_port==SNMP_PORT || s_port==SNMP_TRAP_PORT || d_port==SNMP_PORT || d_port==SNMP_TRAP_PORT) {
		protocol_snmp=1;
		snmp_protokol(pkt_data, &sn);
	}
#endif

//SIP
	int b = sizeof(struct ether_header) + len;
	if(protocol_sip==1) SIP_detekcia(pkt_data,b+22); //a+22 je adresa zaciatku udajovej casti UDP datagramu

	s_protocol = s_port;
	d_protocol = d_port;

}

/*analysis of IEEE 802.3 frame*/
void ieee802(const u_char *pkt_data,int type)
{
	sprintf(Eor8,"8");	//it is 802.3 frame ->  we need to set variable Eor8

	MAC_adr_S=0;
	MAC_adr_D=0;
	int i;
	for(i=0;i<ETH_ALEN;i++){
		MAC_adr_S*=256;
		MAC_adr_S+=ethh->ether_shost[i];
	}

	for(i=0;i<ETH_ALEN;i++){
		MAC_adr_D*=256;
		MAC_adr_D+=ethh->ether_dhost[i];
	}

	//sprintf(MAC_adr,"%.2x%.2x%.2x%.2x%.2x%.2x",pkt_data[6],pkt_data[7],pkt_data[8],pkt_data[9],pkt_data[10],pkt_data[11]);

	if(pkt_data[14]==0xaa && pkt_data[15]==0xaa && pkt_data[16]==0x03 && pkt_data[17]==0 && pkt_data[18]==0 && pkt_data[19]==0){
		type=pkt_data[20]*256+pkt_data[21];
		if(type==2048) ip_protokol(pkt_data, 8);	// if we received IP protocol, analysis continues

	}

	net_protokol(type,net_proto);

#ifdef CDP_P
	uint64_t mac_pom;
	mac_pom=0x0100;
	mac_pom<<=32;
	mac_pom+=0x0CCCCCCC;
	if(pkt_data[14]==0xaa && pkt_data[15]==0xaa){
		if(MAC_adr_D==mac_pom){
			if(pkt_data[17]==0x0 && pkt_data[18]==0x0 && pkt_data[19]==0x0C && pkt_data[20]==0x20 && pkt_data[21]==0x0){
				cdp_protokol(pkt_data, cdp_st);
			}
		}
	}
#endif

}

void net_protokol(int number, char *protokols) {
	char protokol[9];
	switch(number) {
		case 2048:
			sprintf(protokol,"IP");
			break;
		case 2054:
			sprintf(protokol,"ARP");
			break;
		case 32821:
			sprintf(protokol,"RARP");
			break;
		case 33079:
			sprintf(protokol,"IPX");
			break;
		case 34525:
			sprintf(protokol,"IPv6");
			break;
		case 34827:
			sprintf(protokol,"PPP");
			break;
		case 36864:
			sprintf(protokol,"Loopback");
			break;
		default:
			sprintf(protokol,"%d",number);
			break;
	}
	strcpy(protokols, protokol);
}

void m_protokoly(ZACIATOK_P *p_zac, char *s) {
	ZAZNAMY *help_zaznamy;  //pomocna struktura
	PROTOKOLY *help_protokol, *p_protokol;
	int find, find_p;
// 	char pokus;
// 	struct in_addr *addr_d, *add_s;
// 	char ip_s[20], ip_d[20];
	
#ifdef _DEBUG_WRITE
	FILE *fw;
	fw=fopen(NAME_FILE2,"a+");
	fprintf(fw,"%s \n",s);
	fclose(fw);
#endif
	/*
	if(debug){
	  if(is_ipv6ext){
	    printf("Protocol: %s:IPv6 Bytes: %d\n", s, pocet_B);
	  }
	  else{
	    add_s = (struct in_addr*) &IP_adr_S;
	    sprintf(ip_s,"%s", inet_ntoa(*add_s));
	    addr_d = (struct in_addr*) &IP_adr_D;
	    sprintf(ip_d,"%s", inet_ntoa(*addr_d));
	    printf("Protocol: %s: %s -> %s, Bytes: %d\n", s,ip_s, ip_d, pocet_B);
	    
	  }
	}*/

	if(p_zac->empty) { //if struct is empty, we create it
		p_zac->empty=0;
		if((p_zac->p_protokoly=(PROTOKOLY*) malloc(sizeof(PROTOKOLY))) == NULL){	// create structure PROTOKOLY
			fprintf(stderr,"Error malloc: %s\n", strerror(errno));
			return;
		}
		p_protokol=p_zac->p_protokoly;
		strcpy(p_protokol->protokol, s);				// set the name of the protocol
		if((p_protokol->zoznam=(ZAZNAMY*) malloc(sizeof(ZAZNAMY))) == NULL){		// create structure ZAZNAMY
			fprintf(stderr,"Error malloc: %s\n", strerror(errno));
			return;
		}
		p_protokol->is_ipv6=is_ipv6ext;  // check s it a ip6 ip adress
		p_protokol->zoznam->mac_s=MAC_adr_S;				// set the MAC adres and IP adress
		p_protokol->zoznam->mac_d=MAC_adr_D;
		p_protokol->zoznam->ip_d=0;
		p_protokol->zoznam->ip_s=0;
		p_protokol->zoznam->ipv6_d=NULL;
		p_protokol->zoznam->ipv6_s=NULL;
		if(is_ipv6ext){
		  if((p_protokol->zoznam->ipv6_d = (unsigned int*) malloc(sizeof(int)*IPV6SIZE)) == NULL){
		    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
		    return;
		  }
		  *p_protokol->zoznam->ipv6_d = *IPV6_adr_D;
		  if((p_protokol->zoznam->ipv6_s = (unsigned int*) malloc(sizeof(int)*IPV6SIZE)) == NULL){
		    fprintf(stderr, "Error malloc: %s\n", strerror(errno));
		    return;
		  }
		  *p_protokol->zoznam->ipv6_s = *IPV6_adr_S;
		}
		else{
		  p_protokol->zoznam->ip_s=ntohl(IP_adr_S);			// we use ntohl, because of frontend
		  p_protokol->zoznam->ip_d=ntohl(IP_adr_D);
		}
		p_protokol->zoznam->pocet_B = pocet_B;				// set the number of bytes
		p_protokol->zoznam->pocet_ramcov=1;
		p_protokol->zoznam->p_next=NULL;
		p_protokol->zoznam->spracovany[0]=0;
		p_protokol->zoznam->spracovany[1]=0;
		p_protokol->zoznam->spracovany[2]=0;
		p_protokol->zoznam->spracovany[3]=0;
		p_protokol->p_next=NULL;
		p_protokol->empty=0;
	}
	else {
		for(help_protokol=p_zac->p_protokoly;help_protokol!=NULL;help_protokol=help_protokol->p_next) { //we looking for the protocol
			find_p=0;	//we don't find the protocol
			if((!strcmp(help_protokol->protokol,s)) && (help_protokol->is_ipv6 == is_ipv6ext)) {
				find_p=1;	//we find the protocol
				for(help_zaznamy=help_protokol->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next) { // prechadza strukturu zoznam
					find=0; //set we don't find the IP adress and MAC adress
					if((help_zaznamy->mac_s==MAC_adr_S) && (help_zaznamy->mac_d==MAC_adr_D)) { // we looking the IP adress and MAC adress in the structure
						if((!is_ipv6ext && ((help_zaznamy->ip_s==ntohl(IP_adr_S)) && (help_zaznamy->ip_d==ntohl(IP_adr_D))))
						  || (is_ipv6ext && ((compare_IPv6(help_zaznamy->ipv6_d, IPV6_adr_D)) && (compare_IPv6(help_zaznamy->ipv6_s, IPV6_adr_S))))
						) {	//we find the IP adress and MAC adress
							help_zaznamy->pocet_B += pocet_B;
							help_zaznamy->pocet_ramcov++;
							find=1;	//set we find the IP adress and MAC adress
							break;
						}
					}
				}
				if(!find) { // we don't find the IP adress and MAC adress, so we create new structure
					for(help_zaznamy=help_protokol->zoznam;help_zaznamy->p_next!=NULL;help_zaznamy=help_zaznamy->p_next) ; //dojde na koniec struktury
					if((help_zaznamy->p_next=(ZAZNAMY*) malloc(sizeof(ZAZNAMY))) == NULL) { //we create new structre ZANAMY
						fprintf(stderr,"Error malloc: %s\n", strerror(errno));
						return;
					}
					help_zaznamy=help_zaznamy->p_next;
					help_zaznamy->mac_s=MAC_adr_S;
					help_zaznamy->mac_d=MAC_adr_D;
					help_zaznamy->ip_d=0;
					help_zaznamy->ip_s=0;
					help_zaznamy->ipv6_d=NULL;
					help_zaznamy->ipv6_s=NULL;
					if(is_ipv6ext){
					  if((help_zaznamy->ipv6_d = (unsigned int*) malloc(sizeof(int)*IPV6SIZE)) == NULL){
					    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
					    return;
					  }
					  *help_zaznamy->ipv6_d=*IPV6_adr_D;
					  if((help_zaznamy->ipv6_s = (unsigned int*) malloc(sizeof(int)*IPV6SIZE)) == NULL){
					    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
					    return;
					  }
					  *help_zaznamy->ipv6_s=*IPV6_adr_S;
					}
					 else{
					  help_zaznamy->ip_s=ntohl(IP_adr_S);  // we use ntohl, because of frontend
					  help_zaznamy->ip_d=ntohl(IP_adr_D);
					}
					help_zaznamy->pocet_B = pocet_B;
					help_zaznamy->pocet_ramcov=1;
					help_zaznamy->spracovany[0]=0;
					help_zaznamy->spracovany[1]=0;
					help_zaznamy->spracovany[2]=0;
					help_zaznamy->spracovany[3]=0;
					help_zaznamy->p_next=NULL;
				}
				break;
			}
		}
		if(!find_p) { // we don't find the protocol, so we crete new structure for the new protocol
			for(help_protokol=p_zac->p_protokoly;help_protokol->p_next!=NULL;help_protokol=help_protokol->p_next) ; //dojde na koniec struktury
			if((help_protokol->p_next=(PROTOKOLY *) malloc(sizeof(PROTOKOLY))) == NULL) { 		// create structure PROTOKOLY
				fprintf(stderr,"Error malloc: %s\n", strerror(errno));
				return;
			}
			help_protokol=help_protokol->p_next;
			strcpy(help_protokol->protokol, s);  					// set the name of the new protocol
			if((help_protokol->zoznam=(ZAZNAMY*) malloc(sizeof(ZAZNAMY))) == NULL) {		// create structure ZAZNAMY
				fprintf(stderr,"Error malloc: %s\n", strerror(errno));
				return;
			}
			help_protokol->is_ipv6 = is_ipv6ext;
			help_protokol->zoznam->mac_s=MAC_adr_S;
			help_protokol->zoznam->mac_d=MAC_adr_D;
			help_protokol->zoznam->ip_d=0;
			help_protokol->zoznam->ip_s=0;
			help_protokol->zoznam->ipv6_d=NULL;
			help_protokol->zoznam->ipv6_s=NULL;
			if(is_ipv6ext){
			  if((help_protokol->zoznam->ipv6_d = (unsigned int*) malloc(sizeof(int)*IPV6SIZE)) == NULL){
			    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
			    return;
			  }
			  *help_protokol->zoznam->ipv6_d=*IPV6_adr_D;
			  if((help_protokol->zoznam->ipv6_s = (unsigned int*) malloc(sizeof(int)*IPV6SIZE)) == NULL){
			    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
			    return;
			  }
			  *help_protokol->zoznam->ipv6_s=*IPV6_adr_S;
			}
			help_protokol->zoznam->ip_s=ntohl(IP_adr_S);   // we use ntohl, because of frontend 
			help_protokol->zoznam->ip_d=ntohl(IP_adr_D);
			help_protokol->zoznam->pocet_B = pocet_B;
			help_protokol->zoznam->pocet_ramcov=1;
			help_protokol->zoznam->p_next=NULL;
			help_protokol->zoznam->spracovany[0]=0;
			help_protokol->zoznam->spracovany[1]=0;
			help_protokol->zoznam->spracovany[2]=0;
			help_protokol->zoznam->spracovany[3]=0;
			help_protokol->p_next=NULL;
			help_protokol->empty=0;
		}
	}
}

//free structure
void free_protokoly(ZACIATOK_P *p_zac) {
	ZAZNAMY *help_zaznamy, *p_pomz;  //help structure
	PROTOKOLY *help_protokol, *p_pomp, *p_protokol;

	if(!p_zac->empty) {
		p_protokol=p_zac->p_protokoly;
		if(!p_protokol->empty){
			for(help_protokol=p_protokol;help_protokol!=NULL;) {
				for(help_zaznamy=help_protokol->zoznam;help_zaznamy!=NULL;){
					p_pomz=help_zaznamy->p_next;
					free(help_zaznamy);
					help_zaznamy=p_pomz;
				}
				p_pomp=help_protokol->p_next;
				if(!strcmp(help_protokol->protokol,"IPV6")){
				  if(!help_protokol->zoznam->ipv6_d){
				    free(help_protokol->zoznam->ipv6_d);
				    help_protokol->zoznam->ipv6_d=NULL;
				  }
				  if(!help_protokol->zoznam->ipv6_s){
				    free(help_protokol->zoznam->ipv6_s);
				    help_protokol->zoznam->ipv6_d=NULL;
				  }
				}
				free(help_protokol);
				help_protokol=p_pomp;
			}
		}
		p_zac->empty=1; // set the structure is empty
		p_zac->p_protokoly=NULL;
	}
}

//zapis strukturu do DB
void *zapis_do_DB_protokoly(void *pretah2) {
	PRETAH *pretah1 = (PRETAH *) pretah2;
	ZACIATOK_P *p_zac;
	MYSQL *conn;
	KTHREAD *kt;
	p_zac=pretah1->p;
	conn=pretah1->d;
	kt=pretah1->t;
	PROTOKOLY *p_protokol;

	if(!p_zac->empty){
		p_protokol=p_zac->p_protokoly;
		for(;p_protokol!=NULL;p_protokol=p_protokol->p_next){
				processingl(p_protokol,conn);
		}
	}
	fprintf(stderr,"Zapis do DB...\t[DONE]\n");
	kt->run=0;
	mysql_close(conn);
	free_protokoly(p_zac);
}

char compare_IPv6(unsigned int* IP1, unsigned int* IP2)
{

  int i;
  for(i=0;i<IPV6SIZE;i++){
    if(IP1[i] != IP2[i])
      return 0;
  }
  return 1;
  
}

KTHREAD *create_thread(KTHREAD **thread)
{
  KTHREAD *pom_thread1, *start_thread, *pom_thread2, *pom_thread3;
  
  start_thread=*thread;
  // ak struktura neexistuje tak ju vytvorime
  if(!start_thread){
    start_thread = (KTHREAD*) malloc(sizeof(KTHREAD));
    start_thread->p_next=NULL;
    start_thread->p_previous=NULL;
    start_thread->run=0;
    *thread=start_thread;
    return start_thread;
  }
  else{
    //prechadzame celu strukturu
    for(pom_thread1=start_thread;pom_thread1->p_next!=NULL;){
      // kontorujeme ci dane vlakno stale bezi, ak nie tak ho vyhodime z danej struktury 
      if(pom_thread1->run){
	pom_thread1 = pom_thread1->p_next;
      }
      else{
	if(pom_thread1->p_previous==NULL){
	  start_thread=pom_thread1->p_next;
	  if(pom_thread1)
	    free(pom_thread1);
	  pom_thread1=NULL;
	}
	else{
	  pom_thread2 = pom_thread1->p_next;
	  pom_thread2->p_previous = pom_thread1->p_previous;
	  if(pom_thread1)
	    free(pom_thread1);
	  pom_thread1=NULL;
	  pom_thread1=pom_thread2;
	}
      }
    }
    
//vytvorime nove vlakno    
    pom_thread3 = (KTHREAD*) malloc(sizeof(KTHREAD));
    pom_thread3->p_next=NULL;
    pom_thread3->p_previous=pom_thread1;
    pom_thread3->run=0;
    pom_thread1->p_next=pom_thread3;
//skonrolujeme ci bezi este predposledne vlakno strukture, ktore sme este neskontrolovali    
    if(!pom_thread1->run){
      if(pom_thread1->p_previous==NULL){
	start_thread=pom_thread1->p_next;
	start_thread->p_previous=NULL;
	*thread=start_thread;
	if(pom_thread1)
	  free(pom_thread1);
	pom_thread1=NULL;
      }
      else{
	pom_thread2 = pom_thread1->p_next;
	pom_thread2->p_previous = pom_thread1->p_previous;
	if(pom_thread1)
	  free(pom_thread1);
	pom_thread1=NULL;
	pom_thread1=pom_thread2;
      }
    }
  }
  
  return pom_thread3;
}






