#include "read_conf.h"
#include "katalyzer.h"

void read_conf()
{
	char db_name_pom[DLZKA+1];
        f_config=fopen ( config_name, "r" );
        if ( !f_config ) {
                fprintf ( stderr,"Cannot open config file %s, EXITING!\n", config_name );
                exit ( ERR_OPEN_CONFIG );
        }
        //fgets(str, sizeof(str), f_config);
        //while (!feof(f_config))
        while ( fgets ( str, sizeof str, f_config ) ) {
                char name[100],value[128];
                if ( !strncmp ( str, "#", 1 ) ) ;// printf ("Komentar\n");

                else if ( sscanf ( str, " %[^=]=%[^\n]", name, value ) == 2 ) {
                        if ( !strcmp ( name, "INTERFACE" ) ) {
                                snprintf ( interface,sizeof interface,"%s", value );
                                b_is_iface = 1;
                                i_is_configured++;
                        } else if ( !strcmp ( name, "DB_HOST" ) ) {
                                snprintf ( db_host,sizeof db_host,"%s", value );
                                i_is_configured++;

                        } else if ( !strcmp ( name, "DB_NAME" ) ) {
                                snprintf ( db_name,sizeof db_name,"%s", value );
                                i_is_configured++;
                        } else if ( !strcmp ( name, "DB_USER" ) ) {
                                snprintf ( db_user,sizeof db_user,"%s", value );
                                i_is_configured++;
                        } else if ( !strcmp ( name, "DB_PASS" ) ) {
                                snprintf ( db_pass,sizeof db_pass,"%s", value );
                                i_is_configured++;
                        } else if ( !strcmp ( name, "LOG" ) ) snprintf ( log_name,sizeof log_name,"%s", value );
                       // nacitanie protokolov preddefinovanych - ETH, 802.3, ARP, RARP, IP, IGMP, ICMP, IPX, TCP, UDP
                        else if ( !strcmp ( name, "PROTOCOL_ETH" ) ) protocol_eth = atoi ( value );
                        else if ( !strcmp ( name, "PROTOCOL_8023" ) ) protocol_8023 = atoi ( value );
                        else if ( !strcmp ( name, "PROTOCOL_ARP" ) ) protocol_arp = atoi ( value );
                        else if ( !strcmp ( name, "PROTOCOL_RARP" ) ) protocol_rarp = atoi ( value );
                        else if ( !strcmp ( name, "PROTOCOL_IP" ) )	protocol_ip = atoi ( value );
                        else if ( !strcmp ( name, "PROTOCOL_IPv6" ) ) {
                                protocol_ipv6 = atoi ( value );
                        } else if ( !strcmp ( name, "PROTOCOL_IGMP" ) ) protocol_igmp = atoi ( value );
                        else if ( !strcmp ( name, "PROTOCOL_ICMP" ) ) protocol_icmp = atoi ( value );
                        else if ( !strcmp ( name, "PROTOCOL_IPX" ) ) protocol_ipx = atoi ( value );
                        else if ( !strcmp ( name, "PROTOCOL_TCP" ) ) protocol_tcp = atoi ( value );
                        else if ( !strcmp ( name, "PROTOCOL_UDP" ) ) protocol_udp = atoi ( value );
                        else if ( !strcmp ( name, "TCP_PORT" ) ) {
                                pole_TCP_portov[i_TCP] = atoi ( value );
                                i_TCP++; // after reading all TCP/UDP port numbers in 'i_TCP' is number of all read ports, not index of last port read
                        } else if ( !strcmp ( name, "UDP_PORT" ) ) {
                                pole_UDP_portov[i_UDP] = atoi ( value );
                                i_UDP++;
                        }
                        // protokol SIP nastavenie
                        else if ( !strcmp ( name, "PROTOCOL_SIP" ) ) {
                                protocol_sip = atoi ( value );
                                //fprintf(stderr,"protocol_sip = %d\n",protocol_sip);
                        }
                }
        }

        fclose ( f_config );

        if ( i_is_configured != 5 ) {
                printf ( "*** Incorrect configuration, check %s configuration file!\n",config_name );
                exit ( ERR_INC_CONF );
        }
        strcpy(db_name_pom,db_name);
	sprintf(db_name,"%s_%s",db_name_pom, interface);
}
