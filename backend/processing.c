#include "processing.h"


void processingl(PROTOKOLY *s, MYSQL *conn) {
        char *prikaz; // dynamicka premena
        char *prikaz_pom; // pomocna dynamicka premena
        char temp_str[10000];
        char temp[15];  // premenna pre ulozenie konvertovanej IP adresy z formatu int spat do stringu - pre zapis do DB sa bude nadalej pouzivat a.b.c.d format
        char isipv6[4]; // pouzijeme pri ipv6
	int ip=0,mac=0,ip_sd=0,mac_sd=0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	unsigned long int source_B, destin_B, source_ramcov, destin_ramcov;
	ZAZNAMY *help_zaznamy, *help_zaznamy2, *p_pomz, *p_pomz2;

//TIME	
	if(!s->empty) {
	  
		if(!strcmp(s->protokol,"IPv6"))
		  strcpy(isipv6,"_v6");
		else
		  strcpy(isipv6,"");
		//MAC			
		sprintf(temp_str,"SELECT MAX(id) FROM %s%s_1m_MAC;",s->protokol, isipv6);
               	if(mysql_query(conn,temp_str)) {
                       	fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",temp_str,db_name, mysql_error(conn));
               	}
               	result = mysql_store_result(conn);
               	row=mysql_fetch_row(result);
		if(row[0]!=NULL) mac = atoi(row[0]);
		mysql_free_result(result);
		mac++;

		//IP
		sprintf(temp_str,"SELECT MAX(id) FROM %s%s_1m_IP;",s->protokol, isipv6);
               	if(mysql_query(conn,temp_str)) {
			fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",temp_str,db_name, mysql_error(conn));
		}	
                result = mysql_store_result(conn);
                row=mysql_fetch_row(result);
		if(row[0]!=NULL) ip = atoi(row[0]);
		mysql_free_result(result);
		ip++;

		//MAC_SD
		sprintf(temp_str,"SELECT MAX(id) FROM %s%s_1m_MAC_SD;",s->protokol, isipv6);
                if(mysql_query(conn,temp_str)) {
                        fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",temp_str,db_name, mysql_error(conn));
                }
                result = mysql_store_result(conn);
                row=mysql_fetch_row(result);
		if(row[0]!=NULL) mac_sd = atoi(row[0]);
		mysql_free_result(result);
		mac_sd++;

		//IP_SD
		sprintf(temp_str,"SELECT MAX(id) FROM %s%s_1m_IP_SD;",s->protokol, isipv6);
                if(mysql_query(conn,temp_str)) {
                      	fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",temp_str,db_name, mysql_error(conn));
                }
                result = mysql_store_result(conn);
                row=mysql_fetch_row(result);
		if(row[0]!=NULL) ip_sd = atoi(row[0]);
		mysql_free_result(result);
		ip_sd++;

		sprintf(temp_str,"INSERT INTO %s%s_1m_time (time,IP_id,MAC_id,IP_SD_id,MAC_SD_id) VALUES ('%d','%d','%d','%d','%d');", s->protokol, isipv6, processing_time, ip, mac, ip_sd, mac_sd);
		//if(debug) fprintf(stderr,"%s\n",temp_str);
		if(mysql_query(conn, temp_str)) {
			fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",temp_str,db_name, mysql_error(conn));
		}	
	}



//IP	
//	struct in_addr *ips, *ipd;
	if(!s->empty) {
		prikaz=(char*) malloc(sizeof(char)*100); // alokujeme si pamät pre retazec, pre istotu si alokuje trosku viac pamäti ako je treba
		sprintf(prikaz,"INSERT INTO %s_1m_IP (IP, bytes_S, packets_S, bytes_D, packets_D) VALUES ",s->protokol);
		for(help_zaznamy=s->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next) {
//				ips=(struct in_addr*) &help_zaznamy->ip_s;
//				fprintf(stderr,"IPs: %s   hod=%X\n",inet_ntoa(*ips), help_zaznamy->ip_s);
//				ipd=(struct in_addr*) &help_zaznamy->ip_d;
//				fprintf(stderr,"IPd: %s   hod=%X\n",inet_ntoa(*ipd), help_zaznamy->ip_d);
			if(help_zaznamy->spracovany[2]==0) {
				source_B=help_zaznamy->pocet_B;
				source_ramcov=help_zaznamy->pocet_ramcov;
				destin_B=0;
				destin_ramcov=0;
				for(help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next) {
					if(help_zaznamy->ip_s==help_zaznamy2->ip_s) {
						source_B+=help_zaznamy2->pocet_B;
						source_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[2]=1;
					}
					if(help_zaznamy->ip_s==help_zaznamy2->ip_d) {
						destin_B+=help_zaznamy2->pocet_B;
						destin_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[3]=1;
					}
				}
				sprintf(temp_str,"('%u','%lu','%lu','%lu','%lu'),", help_zaznamy->ip_s, source_B, source_ramcov, destin_B, destin_ramcov);
				if(!(prikaz_pom=(char*) malloc(sizeof(char)*strlen(prikaz)+sizeof(char)*(strlen(temp_str)+1)))){
					fprintf(stderr,"Error realloc: %s\n", strerror(errno));
					exit(1);
				}
				strcpy(prikaz_pom,prikaz);
				free(prikaz);
				prikaz=prikaz_pom;
				strcat(prikaz,temp_str);
				//fprintf(stderr,"%s\n",temp_str);
				help_zaznamy->spracovany[2]=1;
			}

			if(help_zaznamy->spracovany[3]==0) {
				source_B=0;
				source_ramcov=0;
				destin_B=help_zaznamy->pocet_B;
				destin_ramcov=help_zaznamy->pocet_ramcov;
				for(help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next) {
					if(help_zaznamy->ip_d==help_zaznamy2->ip_s) {
						source_B+=help_zaznamy2->pocet_B;
						source_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[2]=1;
					}
					if(help_zaznamy->ip_d==help_zaznamy2->ip_d) {
						destin_B+=help_zaznamy2->pocet_B;
						destin_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[3]=1;
					}
				}
				sprintf(temp_str,"('%u','%lu','%lu','%lu','%lu'),", help_zaznamy->ip_d, source_B, source_ramcov, destin_B, destin_ramcov);
				if(!(prikaz_pom=(char*) malloc(sizeof(char)*strlen(prikaz)+sizeof(char)*(strlen(temp_str)+1)))){ // alokujeme si novú väcsiu cast pamäti podla potreby
					fprintf(stderr,"Error realloc: %s\n", strerror(errno));
					exit(1);
				}
				strcpy(prikaz_pom,prikaz); // skopirujeme si obsah zo starej pamäti do novej
				free(prikaz); // uvolnime staru pamät
				prikaz=prikaz_pom;
				strcat(prikaz,temp_str);
				//fprintf(stderr,"%s\n",temp_str);
				help_zaznamy->spracovany[3]=1;
			}
		}
		prikaz[strlen(prikaz)-1]=';';
//		fprintf(stderr,"Velkost: %d \n", strlen(prikaz));
		if(mysql_query(conn, prikaz)) {
			fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error(conn));
			if(debug) fprintf(stderr,"%s\n",prikaz);
		}
		free(prikaz);

//IP_SD		
		prikaz=(char*) malloc(sizeof(char)*100);
		sprintf(prikaz,"INSERT INTO %s_1m_IP_SD (IP_1, IP_2, bytes_12, packets_12, bytes_21, packets_21) VALUES ",s->protokol);
		for(help_zaznamy=s->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next) {
			if((help_zaznamy->spracovany[2]<2) && (help_zaznamy->spracovany[3]<2)) {
				source_B=help_zaznamy->pocet_B;
				source_ramcov=help_zaznamy->pocet_ramcov;
				destin_B=0;
				destin_ramcov=0;
				for(help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next) {
					if(help_zaznamy->ip_s==help_zaznamy2->ip_s && help_zaznamy->ip_d==help_zaznamy2->ip_d) {
						source_B+=help_zaznamy2->pocet_B;
						source_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[2]=2;
						help_zaznamy2->spracovany[3]=2;
					}
					else if(help_zaznamy->ip_s==help_zaznamy2->ip_d && help_zaznamy->ip_d==help_zaznamy2->ip_s) {
						destin_B+=help_zaznamy2->pocet_B;
						destin_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[2]=2;
						help_zaznamy2->spracovany[3]=2;
					}
				}
				sprintf(temp_str,"('%u','%u','%lu','%lu','%lu','%lu'),", help_zaznamy->ip_s, help_zaznamy->ip_d, source_B, source_ramcov, destin_B, destin_ramcov);
				if(!(prikaz_pom=(char*) malloc(sizeof(char)*strlen(prikaz)+sizeof(char)*(strlen(temp_str)+1)))){
					fprintf(stderr,"Error realloc: %s\n", strerror(errno));
					exit(1);
				}
				strcpy(prikaz_pom,prikaz);
				free(prikaz);
				prikaz=prikaz_pom;
				strcat(prikaz,temp_str);
				//fprintf(stderr,"%s\n",temp_str);
				help_zaznamy->spracovany[2]=2;
				help_zaznamy->spracovany[3]=2;
			}
		}
		prikaz[strlen(prikaz)-1]=';';
//		fprintf(stderr,"Velkost: %d \n", strlen(prikaz));
		if(mysql_query(conn, prikaz)) {
			fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error(conn));
		}
		free(prikaz);
		
//MAC		
		prikaz=(char*) malloc(sizeof(char)*100);
		sprintf(prikaz,"INSERT INTO %s_1m_MAC (MAC, bytes_S, packets_S, bytes_D, packets_D) VALUES ",s->protokol);
		for(help_zaznamy=s->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next) {
			if(help_zaznamy->spracovany[0]==0) {
				source_B=help_zaznamy->pocet_B;
				source_ramcov=help_zaznamy->pocet_ramcov;
				destin_B=0;
				destin_ramcov=0;
				for(help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next) {
					if(help_zaznamy->mac_s==help_zaznamy2->mac_s) {
						source_B+=help_zaznamy2->pocet_B;
						source_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[0]=1;
					}
					if(help_zaznamy->mac_s==help_zaznamy2->mac_d) {
						destin_B+=help_zaznamy2->pocet_B;
						destin_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[1]=1;
					}
				}
				sprintf(temp_str,"('%llu','%lu','%lu','%lu','%lu'),", help_zaznamy->mac_s, source_B, source_ramcov, destin_B, destin_ramcov);
				if(!(prikaz_pom=(char*) malloc(sizeof(char)*strlen(prikaz)+sizeof(char)*(strlen(temp_str)+1)))){
					fprintf(stderr,"Error realloc: %s\n", strerror(errno));
					exit(1);
				}
				strcpy(prikaz_pom,prikaz);
				free(prikaz);
				prikaz=prikaz_pom;
				strcat(prikaz,temp_str);
				//fprintf(stderr,"%s\n",temp_str);
				help_zaznamy->spracovany[0]=1;
			}

			if(help_zaznamy->spracovany[1]==0) {
				source_B=0;
				source_ramcov=0;
				destin_B=help_zaznamy->pocet_B;
				destin_ramcov=help_zaznamy->pocet_ramcov;
				for(help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next) {
					if(help_zaznamy->mac_d==help_zaznamy2->mac_s) {
						source_B+=help_zaznamy2->pocet_B;
						source_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[0]=1;
					}
					if(help_zaznamy->mac_d==help_zaznamy2->mac_d) {
						destin_B+=help_zaznamy2->pocet_B;
						destin_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[1]=1;
					}
				}
				sprintf(temp_str,"('%llu','%lu','%lu','%lu','%lu'),", help_zaznamy->mac_d, source_B, source_ramcov, destin_B, destin_ramcov);
				if(!(prikaz_pom=(char*) malloc(sizeof(char)*strlen(prikaz)+sizeof(char)*(strlen(temp_str)+1)))){
					fprintf(stderr,"Error realloc: %s\n", strerror(errno));
					exit(1);
				}
				strcpy(prikaz_pom,prikaz);
				free(prikaz);
				prikaz=prikaz_pom;
				strcat(prikaz,temp_str);
				//fprintf(stderr,"%s\n",temp_str);
				help_zaznamy->spracovany[1]=1;
			}
		}
		prikaz[strlen(prikaz)-1]=';';
//		fprintf(stderr,"Velkost: %d \n", strlen(prikaz));
		if(mysql_query(conn, prikaz)) {
			fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error(conn));
		}
		free(prikaz);

//MAC_SD
		prikaz=(char*) malloc(sizeof(char)*100);
		sprintf(prikaz,"INSERT INTO %s_1m_MAC_SD (MAC_1, MAC_2, bytes_12, packets_12, bytes_21, packets_21) VALUES ",s->protokol);
		for(help_zaznamy=s->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next) {
			if((help_zaznamy->spracovany[0]<2) && (help_zaznamy->spracovany[1]<2)) {
				source_B=help_zaznamy->pocet_B;
				source_ramcov=help_zaznamy->pocet_ramcov;
				destin_B=0;
				destin_ramcov=0;
				for(help_zaznamy2=help_zaznamy->p_next;help_zaznamy2!=NULL;help_zaznamy2=help_zaznamy2->p_next) {
					if(help_zaznamy->mac_s==help_zaznamy2->mac_s && help_zaznamy->mac_d==help_zaznamy2->mac_d) {
						source_B+=help_zaznamy2->pocet_B;
						source_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[0]=2;
						help_zaznamy2->spracovany[1]=2;
					}
					else if(help_zaznamy->mac_s==help_zaznamy2->mac_d && help_zaznamy->mac_d==help_zaznamy2->mac_s) {
						destin_B+=help_zaznamy2->pocet_B;
						destin_ramcov+=help_zaznamy2->pocet_ramcov;
						help_zaznamy2->spracovany[0]=2;
						help_zaznamy2->spracovany[1]=2;
					}
				}
				sprintf(temp_str,"('%llu','%llu','%lu','%lu','%lu','%lu'),", help_zaznamy->mac_s, help_zaznamy->mac_d, source_B, source_ramcov, destin_B, destin_ramcov);
				if(!(prikaz_pom=(char*) malloc(sizeof(char)*strlen(prikaz)+sizeof(char)*(strlen(temp_str)+1)))){
					fprintf(stderr,"Error realloc: %s\n", strerror(errno));
					exit(1);
				}
				strcpy(prikaz_pom,prikaz);
				free(prikaz);
				prikaz=prikaz_pom;
				strcat(prikaz,temp_str);
				//fprintf(stderr,"%s\n",temp_str);
				help_zaznamy->spracovany[0]=2;
				help_zaznamy->spracovany[1]=2;
			}
		}
		prikaz[strlen(prikaz)-1]=';';
//		fprintf(stderr,"Velkost: %d \n", strlen(prikaz));
		if(mysql_query(conn, prikaz)) {
			fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error(conn));
		}
		free(prikaz);

	}

// OK, BUT WT_?
	for(help_zaznamy=s->zoznam;help_zaznamy!=NULL;help_zaznamy=help_zaznamy->p_next) {
		help_zaznamy->spracovany[0]=0;
		help_zaznamy->spracovany[1]=0;
		help_zaznamy->spracovany[2]=0;
		help_zaznamy->spracovany[3]=0;
	}
}


