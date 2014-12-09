#include "processing.h"


void processingl ( PROTOKOLY *s)
{
// dynamicka premena
  char *prikaz;
// pomocna dynamicka premena
  char *prikaz_pom;
  char temp_str[10000];
  char is_ipv6 = 0;
// pouzijeme pri ipv6
  char s_protokol[20];
  int ip=0,mac=0,ip_sd=0,mac_sd=0;
  MYSQL *conn;
  MYSQL_RES *result;
  MYSQL_ROW row;
  unsigned long int source_B, destin_B, source_ramcov, destin_ramcov;
  ZAZNAMY *help_zaznamy, *help_zaznamy2;

#ifdef _DEBUG_WRITE
  FILE *fw;
#endif
  
  conn = mysql_init ( NULL );
  if ( mysql_real_connect ( conn, db_host, db_user, db_pass, NULL, db_port, NULL, CLIENT_MULTI_STATEMENTS ) == NULL ) {
    fprintf ( stderr,"Failed to connect to MYSQL database: Error: %s\n", mysql_error ( conn ) );
    sprintf ( s_tmp_str,"%s:processing.c:processingl:Failed to connect to MYSQL database: Error: %d\n", mysql_error ( conn ),(int) time( &actual_time ) );
    fprintf ( stderr,"%s",s_tmp_str );
    exit ( -1 );
  }
  
  sprintf ( temp_str,"USE %s;",db_name );
  if ( mysql_query ( conn, temp_str ) ) {
    fprintf ( stderr,"Failed to use '%s' from MYSQL database %s: %s\n", temp_str,db_name,mysql_error ( conn ) );
  }
  
//TIME
        if ( !s->empty ) {

                is_ipv6 = s->is_ipv6;

                if ( is_ipv6 ) {
                        sprintf ( s_protokol,"%s_v6", s->protokol );
                } else {
                        sprintf ( s_protokol,"%s",s->protokol );
                }
                //MAC
                sprintf ( temp_str,"SELECT MAX(id) FROM %s_1m_MAC;",s_protokol );
                if ( mysql_query ( conn,temp_str ) ) {
                        fprintf ( stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",temp_str,db_name, mysql_error ( conn ) );
                }
                result = mysql_store_result ( conn );
                row=mysql_fetch_row ( result );
                if ( row[0]!=NULL ) mac = atoi ( row[0] );
                mysql_free_result ( result );
                mac++;

                //IP
                sprintf ( temp_str,"SELECT MAX(id) FROM %s_1m_IP;", s_protokol );
                if ( mysql_query ( conn,temp_str ) ) {
                        fprintf ( stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",temp_str,db_name, mysql_error ( conn ) );
                }
                result = mysql_store_result ( conn );
                row=mysql_fetch_row ( result );
                if ( row[0]!=NULL ) ip = atoi ( row[0] );
                mysql_free_result ( result );
                ip++;

                //MAC_SD
                sprintf ( temp_str,"SELECT MAX(id) FROM %s_1m_MAC_SD;",s_protokol );
                if ( mysql_query ( conn,temp_str ) ) {
                        fprintf ( stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",temp_str,db_name, mysql_error ( conn ) );
                }
                result = mysql_store_result ( conn );
                row=mysql_fetch_row ( result );
                if ( row[0]!=NULL ) mac_sd = atoi ( row[0] );
                mysql_free_result ( result );
                mac_sd++;

                //IP_SD
                sprintf ( temp_str,"SELECT MAX(id) FROM %s_1m_IP_SD;",s_protokol );
                if ( mysql_query ( conn,temp_str ) ) {
                        fprintf ( stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",temp_str,db_name, mysql_error ( conn ) );
                }
                result = mysql_store_result ( conn );
                row=mysql_fetch_row ( result );
                if ( row[0]!=NULL ) ip_sd = atoi ( row[0] );
                mysql_free_result ( result );
                ip_sd++;

                sprintf ( temp_str,"INSERT INTO %s_1m_time (time,IP_id,MAC_id,IP_SD_id,MAC_SD_id) VALUES ('%d','%d','%d','%d','%d');", s_protokol, processing_time, ip, mac, ip_sd, mac_sd );
                //if(debug) fprintf(stderr,"%s\n",temp_str);
#ifdef _DEBUG_WRITE
                fw=fopen ( NAME_FILE,"a+" );
                fprintf ( fw,"%s\n",temp_str );
                fclose ( fw );
#endif
                if ( mysql_query ( conn, temp_str ) ) {
                        fprintf ( stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",temp_str,db_name, mysql_error ( conn ) );
                }
        }



//IP
//	struct in_addr *ips, *ipd;
        if ( !s->empty ) {

                is_ipv6 = s->is_ipv6;
                if ( is_ipv6 ) {
                        sprintf ( s_protokol,"%s_v6", s->protokol );
                } else {
                        sprintf ( s_protokol,"%s",s->protokol );
                }
                prikaz= ( char* ) malloc ( sizeof ( char ) *100 ); // alokujeme si pam
                sprintf ( prikaz,"INSERT INTO %s_1m_IP (IP, bytes_S, packets_S, bytes_D, packets_D) VALUES ",s_protokol );
                for ( help_zaznamy=s->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next ) {
//				ips=(struct in_addr*) &help_zaznamy->ip_s;
//				fprintf(stderr,"IPs: %s   hod=%X\n",inet_ntoa(*ips), help_zaznamy->ip_s);
//				ipd=(struct in_addr*) &help_zaznamy->ip_d;
//				fprintf(stderr,"IPd: %s   hod=%X\n",inet_ntoa(*ipd), help_zaznamy->ip_d);
                        if ( help_zaznamy->spracovany[2]==0 ) {
                                source_B=help_zaznamy->pocet_B;
                                source_ramcov=help_zaznamy->pocet_ramcov;
                                destin_B=0;
                                destin_ramcov=0;
                                for ( help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next ) {
                                        if ( is_ipv6 ) {
                                                if ( compare_IPv6 ( help_zaznamy->ipv6_s, help_zaznamy2->ipv6_s ) ) {
                                                        source_B+=help_zaznamy2->pocet_B;
                                                        source_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[2]=1;
                                                }
                                                if ( compare_IPv6 ( help_zaznamy->ipv6_s, help_zaznamy2->ipv6_d ) ) {
                                                        destin_B+=help_zaznamy2->pocet_B;
                                                        destin_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[3]=1;
                                                }
                                        } else {
                                                if ( help_zaznamy->ip_s==help_zaznamy2->ip_s ) {
                                                        source_B+=help_zaznamy2->pocet_B;
                                                        source_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[2]=1;
                                                }
                                                if ( help_zaznamy->ip_s==help_zaznamy2->ip_d ) {
                                                        destin_B+=help_zaznamy2->pocet_B;
                                                        destin_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[3]=1;
                                                }
                                        }
                                }
                                if ( is_ipv6 )
                                        sprintf ( temp_str,"('%08x%08x%08x%08x','%lu','%lu','%lu','%lu'),", help_zaznamy->ipv6_s[0], help_zaznamy->ipv6_s[1], help_zaznamy->ipv6_s[2], help_zaznamy->ipv6_s[3], source_B, source_ramcov, destin_B, destin_ramcov );
                                else {
                                        sprintf ( temp_str,"('%u','%lu','%lu','%lu','%lu'),", help_zaznamy->ip_s, source_B, source_ramcov, destin_B, destin_ramcov );
				}
                                if ( ! ( prikaz_pom= ( char* ) malloc ( sizeof ( char ) *strlen ( prikaz ) +sizeof ( char ) * ( strlen ( temp_str ) +1 ) ) ) ) {
                                        fprintf ( stderr,"Error realloc: %s\n", strerror ( errno ) );
                                        exit ( 1 );
                                }
                                strcpy ( prikaz_pom,prikaz );
                                if ( prikaz )
                                        free ( prikaz );
                                prikaz=NULL;
                                prikaz=prikaz_pom;
                                strcat ( prikaz,temp_str );
                                //fprintf(stderr,"%s\n",temp_str);
                                help_zaznamy->spracovany[2]=1;
                        }

                        if ( help_zaznamy->spracovany[3]==0 ) {
                                source_B=0;
                                source_ramcov=0;
                                destin_B=help_zaznamy->pocet_B;
                                destin_ramcov=help_zaznamy->pocet_ramcov;
                                for ( help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next ) {
                                        if ( is_ipv6 ) {
                                                if ( compare_IPv6 ( help_zaznamy->ipv6_d, help_zaznamy2->ipv6_s ) ) {
                                                        source_B+=help_zaznamy2->pocet_B;
                                                        source_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[2]=1;
                                                }
                                                if ( compare_IPv6 ( help_zaznamy->ipv6_d, help_zaznamy2->ipv6_d ) ) {
                                                        destin_B+=help_zaznamy2->pocet_B;
                                                        destin_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[3]=1;

                                                }
                                        } else {
                                                if ( help_zaznamy->ip_d==help_zaznamy2->ip_s ) {
                                                        source_B+=help_zaznamy2->pocet_B;
                                                        source_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[2]=1;
                                                }
                                                if ( help_zaznamy->ip_d==help_zaznamy2->ip_d ) {
                                                        destin_B+=help_zaznamy2->pocet_B;
                                                        destin_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[3]=1;

                                                }
                                        }
                                }
                                if ( is_ipv6 )
                                        sprintf ( temp_str,"('%08x%08x%08x%08x','%lu','%lu','%lu','%lu'),", help_zaznamy->ipv6_d[0], help_zaznamy->ipv6_d[1], help_zaznamy->ipv6_d[2], help_zaznamy->ipv6_d[3], source_B, source_ramcov, destin_B, destin_ramcov );
                                else
                                        sprintf ( temp_str,"('%u','%lu','%lu','%lu','%lu'),", help_zaznamy->ip_d, source_B, source_ramcov, destin_B, destin_ramcov );
                                if ( ! ( prikaz_pom= ( char* ) malloc ( sizeof ( char ) *strlen ( prikaz ) +sizeof ( char ) * ( strlen ( temp_str ) +1 ) ) ) ) { // alokujeme si nov
                                        fprintf ( stderr,"Error realloc: %s\n", strerror ( errno ) );
                                        exit ( 1 );
                                }
                                strcpy ( prikaz_pom,prikaz ); // skopirujeme si obsah zo starej pam
                                if ( prikaz )
                                        free ( prikaz ); // uvolnime staru pam
                                prikaz=NULL;
                                prikaz=prikaz_pom;
                                strcat ( prikaz,temp_str );
                                //fprintf(stderr,"%s\n",temp_str);
                                help_zaznamy->spracovany[3]=1;
                        }
                }
                prikaz[strlen ( prikaz )-1]=';';
//		fprintf(stderr,"Velkost: %d \n", strlen(prikaz));
#ifdef _DEBUG_WRITE
                fw=fopen ( NAME_FILE,"a+" );
                fprintf ( fw,"%s\n",prikaz );
                fclose ( fw );
#endif
                if ( mysql_query ( conn, prikaz ) ) {
                        fprintf ( stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error ( conn ) );
                        if ( debug ) fprintf ( stderr,"%s\n",prikaz );
                }
                if ( prikaz )
                        free ( prikaz );

//IP_SD
                prikaz= ( char* ) malloc ( sizeof ( char ) *100 );
                sprintf ( prikaz,"INSERT INTO %s_1m_IP_SD (IP_1, IP_2, bytes_12, packets_12, bytes_21, packets_21) VALUES ",s_protokol );
                for ( help_zaznamy=s->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next ) {
                        if ( ( help_zaznamy->spracovany[2]<2 ) && ( help_zaznamy->spracovany[3]<2 ) ) {
                                source_B=help_zaznamy->pocet_B;
                                source_ramcov=help_zaznamy->pocet_ramcov;
                                destin_B=0;
                                destin_ramcov=0;
                                for ( help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next ) {
                                        if ( is_ipv6 ) {
                                                if ( ( compare_IPv6 ( help_zaznamy->ipv6_s, help_zaznamy2->ipv6_s ) ) && ( compare_IPv6 ( help_zaznamy->ipv6_d, help_zaznamy2->ipv6_d ) ) ) {
                                                        source_B+=help_zaznamy2->pocet_B;
                                                        source_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[2]=2;
                                                        help_zaznamy2->spracovany[3]=2;
                                                } else if ( ( compare_IPv6 ( help_zaznamy->ipv6_s, help_zaznamy2->ipv6_d ) ) && ( compare_IPv6 ( help_zaznamy->ipv6_d, help_zaznamy2->ipv6_s ) ) ) {
                                                        destin_B+=help_zaznamy2->pocet_B;
                                                        destin_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[2]=2;
                                                        help_zaznamy2->spracovany[3]=2;
                                                }
                                        } else {
                                                if ( help_zaznamy->ip_s==help_zaznamy2->ip_s && help_zaznamy->ip_d==help_zaznamy2->ip_d ) {
                                                        source_B+=help_zaznamy2->pocet_B;
                                                        source_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[2]=2;
                                                        help_zaznamy2->spracovany[3]=2;
                                                } else if ( help_zaznamy->ip_s==help_zaznamy2->ip_d && help_zaznamy->ip_d==help_zaznamy2->ip_s ) {
                                                        destin_B+=help_zaznamy2->pocet_B;
                                                        destin_ramcov+=help_zaznamy2->pocet_ramcov;
                                                        help_zaznamy2->spracovany[2]=2;
                                                        help_zaznamy2->spracovany[3]=2;
                                                }
                                        }
                                }
                                if ( is_ipv6 )
                                        sprintf ( temp_str,"('%08x%08x%08x%08x','%08x%08x%08x08%x','%lu','%lu','%lu','%lu'),", help_zaznamy->ipv6_s[0], help_zaznamy->ipv6_s[1], help_zaznamy->ipv6_s[2], help_zaznamy->ipv6_s[3], help_zaznamy->ipv6_d[0], help_zaznamy->ipv6_d[1], help_zaznamy->ipv6_d[2], help_zaznamy->ipv6_d[3], source_B, source_ramcov, destin_B, destin_ramcov );
                                else
                                        sprintf ( temp_str,"('%u','%u','%lu','%lu','%lu','%lu'),", help_zaznamy->ip_s, help_zaznamy->ip_d, source_B, source_ramcov, destin_B, destin_ramcov );
                                if ( ! ( prikaz_pom= ( char* ) malloc ( sizeof ( char ) *strlen ( prikaz ) +sizeof ( char ) * ( strlen ( temp_str ) +1 ) ) ) ) {
                                        fprintf ( stderr,"Error realloc: %s\n", strerror ( errno ) );
                                        exit ( 1 );
                                }
                                strcpy ( prikaz_pom,prikaz );
                                if ( prikaz )
                                        free ( prikaz );
                                prikaz=NULL;
                                prikaz=prikaz_pom;
                                strcat ( prikaz,temp_str );
                                //fprintf(stderr,"%s\n",temp_str);
                                help_zaznamy->spracovany[2]=2;
                                help_zaznamy->spracovany[3]=2;
                        }
                }
                prikaz[strlen ( prikaz )-1]=';';
//		fprintf(stderr,"Velkost: %d \n", strlen(prikaz));
                if ( mysql_query ( conn, prikaz ) ) {
                        fprintf ( stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error ( conn ) );
                }
                if ( prikaz )
                        free ( prikaz );
                prikaz=NULL;

//MAC
                prikaz= ( char* ) malloc ( sizeof ( char ) *100 );
                sprintf ( prikaz,"INSERT INTO %s_1m_MAC (MAC, bytes_S, packets_S, bytes_D, packets_D) VALUES ",s_protokol );
                for ( help_zaznamy=s->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next ) {
                        if ( help_zaznamy->spracovany[0]==0 ) {
                                source_B=help_zaznamy->pocet_B;
                                source_ramcov=help_zaznamy->pocet_ramcov;
                                destin_B=0;
                                destin_ramcov=0;
                                for ( help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next ) {
                                        if ( help_zaznamy->mac_s==help_zaznamy2->mac_s ) {
                                                source_B+=help_zaznamy2->pocet_B;
                                                source_ramcov+=help_zaznamy2->pocet_ramcov;
                                                help_zaznamy2->spracovany[0]=1;
                                        }
                                        if ( help_zaznamy->mac_s==help_zaznamy2->mac_d ) {
                                                destin_B+=help_zaznamy2->pocet_B;
                                                destin_ramcov+=help_zaznamy2->pocet_ramcov;
                                                help_zaznamy2->spracovany[1]=1;
                                        }
                                }
                                sprintf ( temp_str,"('%llu','%lu','%lu','%lu','%lu'),", help_zaznamy->mac_s, source_B, source_ramcov, destin_B, destin_ramcov );
                                if ( ! ( prikaz_pom= ( char* ) malloc ( sizeof ( char ) *strlen ( prikaz ) +sizeof ( char ) * ( strlen ( temp_str ) +1 ) ) ) ) {
                                        fprintf ( stderr,"Error realloc: %s\n", strerror ( errno ) );
                                        exit ( 1 );
                                }
                                strcpy ( prikaz_pom,prikaz );
                                if ( prikaz )
                                        free ( prikaz );
                                prikaz=NULL;
                                prikaz=prikaz_pom;
                                strcat ( prikaz,temp_str );
                                //fprintf(stderr,"%s\n",temp_str);
                                help_zaznamy->spracovany[0]=1;
                        }

                        if ( help_zaznamy->spracovany[1]==0 ) {
                                source_B=0;
                                source_ramcov=0;
                                destin_B=help_zaznamy->pocet_B;
                                destin_ramcov=help_zaznamy->pocet_ramcov;
                                for ( help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next ) {
                                        if ( help_zaznamy->mac_d==help_zaznamy2->mac_s ) {
                                                source_B+=help_zaznamy2->pocet_B;
                                                source_ramcov+=help_zaznamy2->pocet_ramcov;
                                                help_zaznamy2->spracovany[0]=1;
                                        }
                                        if ( help_zaznamy->mac_d==help_zaznamy2->mac_d ) {
                                                destin_B+=help_zaznamy2->pocet_B;
                                                destin_ramcov+=help_zaznamy2->pocet_ramcov;
                                                help_zaznamy2->spracovany[1]=1;
                                        }
                                }
                                sprintf ( temp_str,"('%llu','%lu','%lu','%lu','%lu'),", help_zaznamy->mac_d, source_B, source_ramcov, destin_B, destin_ramcov );
                                if ( ! ( prikaz_pom= ( char* ) malloc ( sizeof ( char ) *strlen ( prikaz ) +sizeof ( char ) * ( strlen ( temp_str ) +1 ) ) ) ) {
                                        fprintf ( stderr,"Error realloc: %s\n", strerror ( errno ) );
                                        exit ( 1 );
                                }
                                strcpy ( prikaz_pom,prikaz );
                                if ( prikaz )
                                        free ( prikaz );
                                prikaz=NULL;
                                prikaz=prikaz_pom;
                                strcat ( prikaz,temp_str );
                                //fprintf(stderr,"%s\n",temp_str);
                                help_zaznamy->spracovany[1]=1;
                        }
                }
                prikaz[strlen ( prikaz )-1]=';';
//		fprintf(stderr,"Velkost: %d \n", strlen(prikaz));
#ifdef _DEBUG_WRITE
                fw=fopen ( NAME_FILE,"a+" );
                fprintf ( fw,"%s\n",prikaz );
                fclose ( fw );
#endif
                if ( mysql_query ( conn, prikaz ) ) {
                        fprintf ( stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error ( conn ) );
                }
                if ( prikaz )
                        free ( prikaz );
                prikaz=NULL;

//MAC_SD
                prikaz= ( char* ) malloc ( sizeof ( char ) *100 );
                sprintf ( prikaz,"INSERT INTO %s_1m_MAC_SD (MAC_1, MAC_2, bytes_12, packets_12, bytes_21, packets_21) VALUES ",s_protokol );
                for ( help_zaznamy=s->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next ) {
                        if ( ( help_zaznamy->spracovany[0]<2 ) && ( help_zaznamy->spracovany[1]<2 ) ) {
                                source_B=help_zaznamy->pocet_B;
                                source_ramcov=help_zaznamy->pocet_ramcov;
                                destin_B=0;
                                destin_ramcov=0;
                                for ( help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next ) {
                                        if ( help_zaznamy->mac_s==help_zaznamy2->mac_s && help_zaznamy->mac_d==help_zaznamy2->mac_d ) {
                                                source_B+=help_zaznamy2->pocet_B;
                                                source_ramcov+=help_zaznamy2->pocet_ramcov;
                                                help_zaznamy2->spracovany[0]=2;
                                                help_zaznamy2->spracovany[1]=2;
                                        } else if ( help_zaznamy->mac_s==help_zaznamy2->mac_d && help_zaznamy->mac_d==help_zaznamy2->mac_s ) {
                                                destin_B+=help_zaznamy2->pocet_B;
                                                destin_ramcov+=help_zaznamy2->pocet_ramcov;
                                                help_zaznamy2->spracovany[0]=2;
                                                help_zaznamy2->spracovany[1]=2;
                                        }
                                }
                                sprintf ( temp_str,"('%llu','%llu','%lu','%lu','%lu','%lu'),", help_zaznamy->mac_s, help_zaznamy->mac_d, source_B, source_ramcov, destin_B, destin_ramcov );
                                if ( ! ( prikaz_pom= ( char* ) malloc ( sizeof ( char ) *strlen ( prikaz ) +sizeof ( char ) * ( strlen ( temp_str ) +1 ) ) ) ) {
                                        fprintf ( stderr,"Error realloc: %s\n", strerror ( errno ) );
                                        exit ( 1 );
                                }
                                strcpy ( prikaz_pom,prikaz );
                                if ( prikaz )
                                        free ( prikaz );
                                prikaz=NULL;
                                prikaz=prikaz_pom;
                                strcat ( prikaz,temp_str );
                                //fprintf(stderr,"%s\n",temp_str);
                                help_zaznamy->spracovany[0]=2;
                                help_zaznamy->spracovany[1]=2;
                        }
                }
                prikaz[strlen ( prikaz )-1]=';';
//		fprintf(stderr,"Velkost: %d \n", strlen(prikaz));
#ifdef _DEBUG_WRITE
                fw=fopen ( NAME_FILE,"a+" );
                fprintf ( fw,"%s\n",prikaz );
                fclose ( fw );
#endif
                if ( mysql_query ( conn, prikaz ) ) {
                        fprintf ( stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error ( conn ) );
                }
                if ( prikaz )
                        free ( prikaz );
                prikaz=NULL;

        }

// OK, BUT WT_?
        for ( help_zaznamy=s->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next ) {
                help_zaznamy->spracovany[0]=0;
                help_zaznamy->spracovany[1]=0;
                help_zaznamy->spracovany[2]=0;
                help_zaznamy->spracovany[3]=0;
        }
        mysql_close(conn);
}


