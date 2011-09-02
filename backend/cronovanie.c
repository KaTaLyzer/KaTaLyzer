#include "cronovanie.h"
#include "errdef.h"
#include <errno.h>

//version 3.4.2010 - 10:56
//#define DEBUG_CRONX
//#define DEBUG_CRON
//#define DEBUG

//struktura na ukladanie nazvov protokolov
typedef struct pro {
    char nazov[9];
    char is_ipv6;
    struct pro *dalsi;
} PRO;

//struktura na nacitanie udajov z db
typedef struct sd_t {
    uint64_t mac_s;
    uint64_t mac_d;
    uint32_t ip_s;
    uint32_t ip_d;
    unsigned int *ipv6_s;
    unsigned int *ipv6_d;
    unsigned long int pocet_B12, pocet_B21;
    unsigned long int pocet_ramcov12, pocet_ramcov21;
    int spracovany;
    char is_ipv6;
    struct sd_t *dalsi;
} SD_T;

inline void *cronovanie(void *cast) {
    int cas = (int *) cast; //becouse of thread policy
#ifdef DEBUG
    fprintf(stderr,"cronovanie\n");
#endif
    PRO *table, *prvy; // table - zoznam tabuliek
    SD_T *sd_ta, *sd_prvy, *sd_ta2;
    char mhd[4],nl[4];
    char mstr[10];
    int konst;
    int i,ii,pocitadlo=1;
    int mhd_time[2],test_time[2]; //mhd_time 0 - first, 1 - last; test_time 0 - nl max, 1 - mhd max
    int IP_id[3], MAC_id[3], IP_SD_id[3], MAC_SD_id[3]; //0 - 1 min, 1 - 5xy min, 3 - MIN(id)+1
    char kreatura[100000]; //toto vynimocne nie je prehnane - kvoli CREATE-om
    char prikaz[100000];
    switch (cas) {
    case 5:
        sprintf(mhd,"5m");
        sprintf(nl,"1m");
        konst=290;
        break;
    case 30:
        sprintf(mhd,"30m");
        sprintf(nl,"5m");
        konst=1790;
        break;
    case 2:
        sprintf(mhd,"2h");
        sprintf(nl,"30m");
        konst=7190;
        break;
    case 1:
        sprintf(mhd,"1d");
        sprintf(nl,"2h");
        konst=86390;
        break;
    case 0:
        sprintf(mhd,"1m");
        break;
    default:
        fprintf(stderr,"Failed to switch cas... ...Exiting...");
        exit(0);
        break;
    }
    sprintf(mstr,"_%s_time",nl); //premmenna k show tables na vyparsovanie spravnych tables
#ifdef DEBUG_CRONX
    fprintf(stderr,"za somarinami, ide sa na connect\n");
#endif

    MYSQL *conn;
    MYSQL_RES *result = NULL, *result2 = NULL;
    MYSQL_ROW row, row2;
    conn = mysql_init(NULL);
    if (mysql_real_connect(conn, db_host, db_user, db_pass, NULL, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL) {
        fprintf(stderr,"Failed to connect to MYSQL database: Error: %s\n", mysql_error(conn));
        sprintf(s_tmp_str,"%s:katalyzer.cpp:dispatcher_handler:Failed to connect to MYSQL database: Error: %ld\n", mysql_error(conn),time(&actual_time));
        fprintf(stderr,"%s",s_tmp_str);
        exit (-1);
    }

#ifdef DEBUG_CRONX
    fprintf(stderr,"Connect [DONE], ide sa na USE a creaty\n");
#endif

    sprintf(prikaz,"USE %s;",db_name);
    if (mysql_query(conn,prikaz)) {
        fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
    }
#ifdef DEBUG_CRONX
    fprintf(stderr,"USE [DONE]\n");
#endif

///////////////////
//GET TABLE NAMES
///////////////////
    table = (PRO *)malloc(sizeof(PRO));
    prvy=table;
    sprintf(prikaz,"SHOW TABLES;");
    if (mysql_query(conn,prikaz)) {
        fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
    }
    while ((result = mysql_store_result(conn))!=NULL) {
        while ((row=mysql_fetch_row(result))!=NULL) {
            if (strstr(row[0],mstr)) {
                if (pocitadlo==1) {
		  if(strstr(row[0],"_v6")){
                    sprintf(table->nazov,"%s_v6",strtok(row[0],"_"));
		    table->is_ipv6=1;
		  }
		  else{
                    sprintf(table->nazov,"%s",strtok(row[0],"_"));
		    table->is_ipv6=0;
		  }
		    
		    
#ifdef DEBUG_CRON
                    fprintf(stderr,"'%s' ",table->nazov);
#endif
                    table->dalsi=NULL;
                } else {
                    PRO *pom;
                    pom = (PRO*)malloc(sizeof(PRO));
                    table->dalsi=pom;
                    table = table->dalsi;
		    if(strstr(row[0],"_v6")){
		      sprintf(table->nazov,"%s_v6",strtok(row[0],"_"));
		      table->is_ipv6=1;
		    }
		    else{
		      sprintf(table->nazov,"%s",strtok(row[0],"_"));
		      table->is_ipv6=0;
		    }
#ifdef DEBUG_CRON
                    fprintf(stderr,"'%s' ",table->nazov);
#endif
                    table->dalsi=NULL;
                }
                pocitadlo++;
            }
        }
#ifdef DEBUG_CRON
        fprintf(stderr,"\n");
#endif
    }

#ifdef DEBUG_CRONX
    fprintf(stderr,"SHOW [DONE], ide sa na CREATY\n");
#endif

///////////////////////////
//CREATE TABLES
///////////////////////////
    sprintf(prikaz,"");
    pocitadlo=0;
    for (table=prvy;table!=NULL;table=table->dalsi) {
        if (strstr(table->nazov,"SIP")!=NULL) ;
        else if (strstr(table->nazov,"CDP")!=NULL) ;
        else {
	  if(!table->is_ipv6)
            sprintf(kreatura,"CREATE TABLE IF NOT EXISTS %s_%s_time (`time` int(10) unsigned NOT NULL default '0',`IP_id` int(10) unsigned NOT NULL default '0',`MAC_id` int(10) unsigned NOT NULL default '0',`IP_SD_id` int(10) unsigned NOT NULL default '0',`MAC_SD_id` int(10) unsigned NOT NULL default '0',PRIMARY KEY (`time`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_%s_IP(`id` int(10) unsigned NOT NULL auto_increment,`IP` int unsigned default '0',`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` bigint(20) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D` bigint(20) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_%s_MAC(`id` int(10) unsigned NOT NULL auto_increment,`MAC` bigint unsigned default '0',`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` bigint(20) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D` bigint(20) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_%s_IP_SD(`id` int(10) unsigned NOT NULL auto_increment,`IP_1` int unsigned default '0',`IP_2` int unsigned default '0',`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` bigint(20) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` bigint(20) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_%s_MAC_SD(`id` int(10) unsigned NOT NULL auto_increment,`MAC_1` bigint unsigned default '0',`MAC_2` bigint unsigned default '0',`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` bigint(20) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` bigint(20) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;",table->nazov,mhd,table->nazov,mhd,table->nazov,mhd,table->nazov,mhd,table->nazov,mhd);
	  else
            sprintf(kreatura,"CREATE TABLE IF NOT EXISTS %s_%s_time (`time` int(10) unsigned NOT NULL default '0',`IP_id` int(10) unsigned NOT NULL default '0',`MAC_id` int(10) unsigned NOT NULL default '0',`IP_SD_id` int(10) unsigned NOT NULL default '0',`MAC_SD_id` int(10) unsigned NOT NULL default '0',PRIMARY KEY (`time`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_%s_IP(`id` int(10) unsigned NOT NULL auto_increment,`IP` char(32),`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` bigint(20) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D` bigint(20) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_%s_MAC(`id` int(10) unsigned NOT NULL auto_increment,`MAC` bigint unsigned default '0',`bytes_S` bigint(20) unsigned NOT NULL default '0',`packets_S` bigint(20) unsigned NOT NULL default '0',`bytes_D` bigint(20) unsigned NOT NULL default '0',`packets_D` bigint(20) unsigned NOT NULL default '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_%s_IP_SD(`id` int(10) unsigned NOT NULL auto_increment,`IP_1` char(32),`IP_2` char(32),`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` bigint(20) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` bigint(20) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;CREATE TABLE IF NOT EXISTS %s_%s_MAC_SD(`id` int(10) unsigned NOT NULL auto_increment,`MAC_1` bigint unsigned default '0',`MAC_2` bigint unsigned default '0',`bytes_12` bigint(20) unsigned NOT NULL default '0',`packets_12` bigint(20) unsigned NOT NULL default '0',`bytes_21` bigint(20) unsigned NOT NULL default '0',`packets_21` bigint(20) unsigned NOT NULL default '0',PRIMARY KEY (`id`))  ENGINE=MyISAM DEFAULT CHARSET=latin1;",table->nazov,mhd,table->nazov,mhd,table->nazov,mhd,table->nazov,mhd,table->nazov,mhd);
            strcat(prikaz,kreatura);
            pocitadlo++;
        }
    }
    if (mysql_query(conn, prikaz)) {
        fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
        exit (-1);
    }
    for (ii=0;ii<pocitadlo*5;ii++) {// pocitadlo*5 is number of commands sent into DB - we have to release results of their execution
        mysql_next_result(conn);
    }

///////////////////
//NAPLNENIE
///////////////////
    for (table=prvy;table!=NULL;table=table->dalsi) {
#ifdef DEBUG_CRONX
        fprintf(stderr,"NAPLNANIE %s\n",table->nazov);
#endif
        int x=0;
        for (x=0;x<4;x++) {
            IP_id[x]=0;
            MAC_id[x]=0;
            IP_SD_id[x]=0;
            MAC_SD_id[x]=0;
        }
//KONTROLA, CI JE VOBEC CO SPRACOVAVAT
        sprintf(prikaz,"SELECT MAX(time) FROM %s_%s_time;",table->nazov,nl);
        if (mysql_query(conn,prikaz)) {
            fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
        }
        if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
            if (row[0]!=NULL) test_time[0] = atoi(row[0]);
            mysql_free_result(result);
	    result = NULL;
        } else {
            fprintf(stderr,"Kontrola MAX(time) z nl zlyhala. Ukoncit?\n");
            test_time[0]=0;
        }

        sprintf(prikaz,"SELECT MAX(time) FROM %s_%s_time;",table->nazov,mhd);
        if (mysql_query(conn,prikaz)) {
            fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
        }
        if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
            if (row[0]!=NULL) test_time[1] = atoi(row[0]);
            mysql_free_result(result);
	    result = NULL;
        } else {
            fprintf(stderr,"Kontrola MAX(time) z mhd zlyhala. Ukoncit?\n");
            test_time[1]=0;
        }

        if (test_time[0]<test_time[1]+290) {
            fprintf(stderr,"Netreba robit cronovanie pre protokol %s.\n",table->nazov);
        }
        else {
//TIME TABULKA - DATA
            //MAC
            sprintf(prikaz,"SELECT MAX(id) FROM %s_%s_MAC;",table->nazov,mhd);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                if (row[0]!=NULL) MAC_id[2] = atoi(row[0]);
                MAC_id[2]++;
                mysql_free_result(result);
		result = NULL;
            } else fprintf(stderr,"a");

            //IP
            sprintf(prikaz,"SELECT MAX(id) FROM %s_%s_IP;",table->nazov,mhd);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                if (row[0]!=NULL) IP_id[2] = atoi(row[0]);
                IP_id[2]++;
                mysql_free_result(result);
		result = NULL;
            } else fprintf(stderr,"b");

            //MAC_SD
            sprintf(prikaz,"SELECT MAX(id) FROM %s_%s_MAC_SD;",table->nazov,mhd);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                if (row[0]!=NULL) MAC_SD_id[2] = atoi(row[0]);
                MAC_SD_id[2]++;
                mysql_free_result(result);
		result = NULL;
            } else fprintf(stderr,"c");

            //IP_SD
            sprintf(prikaz,"SELECT MAX(id) FROM %s_%s_IP_SD;",table->nazov,mhd);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                if (row[0]!=NULL)  IP_SD_id[2] = atoi(row[0]);
                IP_SD_id[2]++;
                mysql_free_result(result);
		result = NULL;
            } else fprintf(stderr,"d");

//SPRACOVANIE CASOV
            //zistime MAX z mhd_time tabulky
            sprintf(prikaz,"SELECT MAX(time) FROM %s_%s_time;",table->nazov,mhd);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            //ak uz je daka mhd tabulka, zvysi hodnotu casu o konst a vyberie najblizsi 'od' cas
            if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                if (row[0]!=NULL) {
                    mhd_time[0] = atoi(row[0]);
                    mhd_time[0]+=konst;
                    sprintf(prikaz,"SELECT time FROM %s_%s_time WHERE time>%d;",table->nazov,nl,mhd_time[0]);
                    if (mysql_query(conn,prikaz)) {
                        fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
                    }
                    if (((result2 = mysql_store_result(conn))!=NULL)&&((row2=mysql_fetch_row(result2))!=NULL)) {
                        if (row2[0]!=NULL) mhd_time[0] = atoi(row2[0]);
                        mysql_free_result(result2);
			result2 = NULL;
                    } else fprintf(stderr,"e",table->nazov);
                }
                //ak v mhd nic nie je, zoberie prvy cas z nl tabulky
                else {
                    sprintf(prikaz,"SELECT MIN(time) FROM %s_%s_time;",table->nazov,nl);
                    if (mysql_query(conn,prikaz)) {
                        fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
                    }
                    if (((result2 = mysql_store_result(conn))!=NULL)&&((row2=mysql_fetch_row(result2))!=NULL)) {
                        if (row2[0]!=NULL) mhd_time[0] = atoi(row2[0]);
                        mysql_free_result(result2);
			result2 = NULL;
                    } else fprintf(stderr,"f");
                }
                mhd_time[1]=mhd_time[0]+konst;
                sprintf(prikaz,"SELECT time FROM %s_%s_time WHERE time>%d;",table->nazov,nl,mhd_time[1]);
                if (mysql_query(conn,prikaz)) {
                    fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
                }
                if (((result2 = mysql_store_result(conn))!=NULL)&&((row2=mysql_fetch_row(result2))!=NULL)) {
                    if (row2[0]!=NULL) mhd_time[1] = atoi(row2[0]);
                    mysql_free_result(result2);
		    result2 = NULL;
                } else fprintf(stderr,"g %s",table->nazov);
            }

///TIME TABULKA - INSERT - ulozime zaznam do time tabulky, kedze uz vieme aky cas ma mat
            sprintf(prikaz,"INSERT INTO %s_%s_time (time,IP_id,MAC_id,IP_SD_id,MAC_SD_id) VALUES ('%d','%d','%d','%d','%d');",table->nazov,mhd, mhd_time[0], IP_id[2], MAC_id[2], IP_SD_id[2], MAC_SD_id[2]);
            if (mysql_query(conn, prikaz)) {
                fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error(conn));
            }

//ZISTENIE ROZSAHOV SPRACOVAVANYCH IDciek
            /*OD*/			sprintf(prikaz,"SELECT IP_id,MAC_id,IP_SD_id,MAC_SD_id FROM %s_%s_time WHERE time>='%d';",table->nazov,nl,mhd_time[0]);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                if (row[0]!=NULL) IP_id[0] = atoi(row[0]);
                if (row[1]!=NULL) MAC_id[0] = atoi(row[1]);
                if (row[2]!=NULL) IP_SD_id[0] = atoi(row[2]);
                if (row[3]!=NULL) MAC_SD_id[0] = atoi(row[3]);
                IP_id[0]--;
                MAC_id[0]--;
                IP_SD_id[0]--;
                MAC_SD_id[0]--;
                mysql_free_result(result);
		result = NULL;
            } else fprintf(stderr,"h");

            /*DO*/
            sprintf(prikaz,"SELECT IP_id,MAC_id,IP_SD_id,MAC_SD_id FROM %s_%s_time WHERE time>'%d';",table->nazov,nl,mhd_time[0]+konst);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                if (row[0]!=NULL) IP_id[1] = atoi(row[0]);
                if (row[1]!=NULL) MAC_id[1] = atoi(row[1]);
                if (row[2]!=NULL) IP_SD_id[1] = atoi(row[2]);
                if (row[3]!=NULL) MAC_SD_id[1] = atoi(row[3]);
                IP_id[1]--;
                MAC_id[1]--;
                IP_SD_id[1]--;
                MAC_SD_id[1]--;
                mysql_free_result(result);
		result = NULL;
            } else {
                //MAC
                sprintf(prikaz,"SELECT MAX(id) FROM %s_%s_MAC;",table->nazov,nl);
                if (mysql_query(conn,prikaz)) {
                    fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
                }
                if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                    if (row[0]!=NULL) MAC_id[1] = atoi(row[0]);
                    mysql_free_result(result);
		    result = NULL;
                } else fprintf(stderr,"i");

                //IP
                sprintf(prikaz,"SELECT MAX(id) FROM %s_%s_IP;",table->nazov,nl);
                if (mysql_query(conn,prikaz)) {
                    fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
                }
                if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                    if (row[0]!=NULL) IP_id[1] = atoi(row[0]);
                    mysql_free_result(result);
		    result = NULL;
                } else fprintf(stderr,"j");

                //MAC_SD
                sprintf(prikaz,"SELECT MAX(id) FROM %s_%s_MAC_SD;",table->nazov,nl);
                if (mysql_query(conn,prikaz)) {
                    fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
                }
                if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                    if (row[0]!=NULL) MAC_SD_id[1] = atoi(row[0]);
                    mysql_free_result(result);
		    result = NULL;
                } else fprintf(stderr,"k");

                //IP_SD
                sprintf(prikaz,"SELECT MAX(id) FROM %s_%s_IP_SD;",table->nazov,nl);
                if (mysql_query(conn,prikaz)) {
                    fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
                }
                if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                    if (row[0]!=NULL)  IP_SD_id[1] = atoi(row[0]);
                    mysql_free_result(result);
		    result = NULL;
                } else fprintf(stderr,"l");
            }
// ???
            sprintf(prikaz,"SELECT MAX(IP_id),MAX(MAC_id),MAX(IP_SD_id),MAX(MAC_SD_id) FROM %s_%s_time;",table->nazov,nl);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            if (((result = mysql_store_result(conn))!=NULL)&&((row=mysql_fetch_row(result))!=NULL)) {
                if (row[0]!=NULL) IP_id[2] = atoi(row[0]);
                if (row[1]!=NULL) MAC_id[2] = atoi(row[1]);
                if (row[2]!=NULL) IP_SD_id[2] = atoi(row[2]);
                if (row[3]!=NULL) MAC_SD_id[2] = atoi(row[3]);
                IP_id[2]++;
                MAC_id[2]++;
                IP_SD_id[2]++;
                MAC_SD_id[2]++;
                mysql_free_result(result);
		result = NULL;
            } else fprintf(stderr,"m");


//IP TABULKA - INSERT (najprv SELECT)
            sprintf(prikaz,"SELECT IP,SUM(bytes_S),SUM(packets_S),SUM(bytes_D),SUM(packets_D) FROM %s_%s_IP WHERE id BETWEEN %d AND %d GROUP BY IP;",table->nazov,nl,IP_id[0],IP_id[1]);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            sprintf(prikaz,"INSERT INTO %s_%s_IP (IP,bytes_S,packets_S,bytes_D,packets_D) VALUES ",table->nazov,mhd);
            while ((result = mysql_store_result(conn))!=NULL) {
                while ((row=mysql_fetch_row(result))!=NULL) {
                    sprintf(kreatura, "('%s','%s','%s','%s','%s'),",row[0],row[1],row[2],row[3],row[4]);
                    strcat(prikaz,kreatura);
                }
            }
            prikaz[strlen(prikaz)-1]=';';
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }

//MAC TABULKA - INSERT (najprv SELECT)
            sprintf(prikaz,"SELECT MAC,SUM(bytes_S),SUM(packets_S),SUM(bytes_D),SUM(packets_D) FROM %s_%s_MAC WHERE id BETWEEN %d AND %d GROUP BY MAC;",table->nazov,nl,MAC_id[0],MAC_id[1]);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            sprintf(prikaz,"INSERT INTO %s_%s_MAC (MAC,bytes_S,packets_S,bytes_D,packets_D) VALUES ",table->nazov,mhd);
            while ((result = mysql_store_result(conn))!=NULL) {
                while ((row=mysql_fetch_row(result))!=NULL) {
                    sprintf(kreatura, "('%s','%s','%s','%s','%s'),",row[0],row[1],row[2],row[3],row[4]);
                    strcat(prikaz,kreatura);
                }
            }
            prikaz[strlen(prikaz)-1]=';';
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }


//IP_SD TABULKA - SELECT
            sd_ta=(SD_T*)malloc(sizeof(SD_T));
            sd_prvy=sd_ta;
            sprintf(prikaz,"SELECT IP_1,IP_2,bytes_12,packets_12,bytes_21,packets_21 FROM %s_%s_IP_SD WHERE id BETWEEN %d AND %d;",table->nazov,nl,IP_SD_id[0],IP_SD_id[1]);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            pocitadlo=1;
            while ((result = mysql_store_result(conn))!=NULL) {
                while ((row=mysql_fetch_row(result))!=NULL) {
                    if (pocitadlo==1) {
		      if(table->is_ipv6){
			unsigned int *p_help;
			int i;
			
			sd_ta->is_ipv6=table->is_ipv6;
			
			if((sd_ta->ipv6_d=(int*) malloc(sizeof(int)*IPV6SIZE)) == NULL){
			  fprintf(stderr,"Error malloc in cronovanie: %s\n", strerror(errno));
			  return;
			}
			if((sd_ta->ipv6_s=(int*) malloc(sizeof(int)*IPV6SIZE)) == NULL){
			  fprintf(stderr,"Error malloc in cronovanie: %s\n", strerror(errno));
			  return;
			}
			
			p_help=(int*) &row[0];
			for(i = 0 ;i < IPV6SIZE; i++){
			  sd_ta->ipv6_s[i] = p_help[i];
			}
			
			p_help=(int*) &row[1];
			for(i = 0 ;i < IPV6SIZE; i++){
			  sd_ta->ipv6_d[i] = p_help[i];
			}
		      }
		      else{
			sd_ta->is_ipv6=table->is_ipv6;
			sd_ta->ip_s=atoi(row[0]);
			sd_ta->ip_d=atoi(row[1]);
		      }
		      sd_ta->pocet_B12=atoi(row[2]);
		      sd_ta->pocet_ramcov12=atoi(row[3]);
		      sd_ta->pocet_B21=atoi(row[4]);
		      sd_ta->pocet_ramcov21=atoi(row[5]);
		      sd_ta->spracovany=0;
		      sd_ta->dalsi=NULL;
		    } else {
		      SD_T *sd_pom;
		      sd_pom = (SD_T*)malloc(sizeof(SD_T));
		      sd_ta->dalsi=sd_pom;
		      sd_ta = sd_ta->dalsi;
		      if(table->is_ipv6){
			unsigned int *p_help;
			int i;
			
			sd_ta->is_ipv6=table->is_ipv6;
			
			if((sd_ta->ipv6_d=(int*) malloc(sizeof(int)*IPV6SIZE)) == NULL){
			  fprintf(stderr,"Error malloc in cronovanie: %s\n", strerror(errno));
			  return;
			}
			if((sd_ta->ipv6_s=(int*) malloc(sizeof(int)*IPV6SIZE)) == NULL){
			  fprintf(stderr,"Error malloc in cronovanie: %s\n", strerror(errno));
			  return;
			}
			
			p_help=(int*) &row[0];
			for(i = 0 ;i < IPV6SIZE; i++){
			  sd_ta->ipv6_s[i] = p_help[i];
			}
			
			p_help=(int*) &row[1];
			for(i = 0 ;i < IPV6SIZE; i++){
			  sd_ta->ipv6_d[i] = p_help[i];
			}
		      }		
		      else{
			sd_ta->is_ipv6=table->is_ipv6;
			sd_ta->ip_s=atoi(row[0]);
			sd_ta->ip_d=atoi(row[1]);
		      }
		      sd_ta->pocet_B12=atoi(row[2]);
		      sd_ta->pocet_ramcov12=atoi(row[3]);
		      sd_ta->pocet_B21=atoi(row[4]);
		      sd_ta->pocet_ramcov21=atoi(row[5]);
		      sd_ta->spracovany=0;
		      sd_ta->dalsi=NULL;
		    }
                    pocitadlo++;
                }
            }

//IP_SD TABULKA - INSERT
            sprintf(prikaz,"INSERT INTO %s_%s_IP_SD (IP_1, IP_2, bytes_12, packets_12, bytes_21, packets_21) VALUES ",table->nazov,mhd);
            for (sd_ta=sd_prvy;sd_ta!=NULL;sd_ta=sd_ta->dalsi) {
                if (sd_ta->spracovany==0) {
                    for (sd_ta2=sd_ta->dalsi;sd_ta2!=NULL;sd_ta2=sd_ta2->dalsi) {
                        if (sd_ta->ip_s==sd_ta2->ip_s && sd_ta->ip_d==sd_ta2->ip_d) {
                            sd_ta->pocet_B12+=sd_ta2->pocet_B12;
                            sd_ta->pocet_B21+=sd_ta2->pocet_B21;
                            sd_ta->pocet_ramcov12+=sd_ta2->pocet_ramcov12;
                            sd_ta->pocet_ramcov21+=sd_ta2->pocet_ramcov21;
                            sd_ta2->spracovany=1;
                        }
                        else if (sd_ta->ip_s==sd_ta2->ip_d && sd_ta->ip_d==sd_ta2->ip_s) {
                            sd_ta->pocet_B12+=sd_ta2->pocet_B21;
                            sd_ta->pocet_B21+=sd_ta2->pocet_B12;
                            sd_ta->pocet_ramcov12+=sd_ta2->pocet_ramcov21;
                            sd_ta->pocet_ramcov21+=sd_ta2->pocet_ramcov12;
                            sd_ta2->spracovany=1;
                        }
                    }
                    if(sd_ta->is_ipv6)
		      sprintf(kreatura,"('%08x%08x%08x%08u','%08x%08x%08x%08u','%lu','%lu','%lu','%lu'),", sd_ta->ipv6_s[0], sd_ta->ipv6_s[1], sd_ta->ipv6_s[2], sd_ta->ipv6_s[3], sd_ta->ipv6_d[0], sd_ta->ipv6_d[1], sd_ta->ipv6_d[2], sd_ta->ipv6_d[3], sd_ta->pocet_B12, sd_ta->pocet_ramcov12, sd_ta->pocet_B21, sd_ta->pocet_ramcov21);
		    else
		      sprintf(kreatura,"('%u','%u','%lu','%lu','%lu','%lu'),", sd_ta->ip_s, sd_ta->ip_d, sd_ta->pocet_B12, sd_ta->pocet_ramcov12, sd_ta->pocet_B21, sd_ta->pocet_ramcov21);
                    strcat(prikaz,kreatura);
                    sd_ta->spracovany=1;
                }
            }
            prikaz[strlen(prikaz)-1]=';';
            if (mysql_query(conn, prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }

            for (sd_ta=sd_prvy;sd_ta!=NULL;) {
                sd_prvy=sd_ta;
                sd_ta=sd_ta->dalsi;
		if(sd_prvy->is_ipv6){
		  free(sd_prvy->ipv6_d);
		  free(sd_prvy->ipv6_s);
		}
                free(sd_prvy);
            }

//MAC_SD TABULKA - SELECT
            sd_ta=(SD_T*)malloc(sizeof(SD_T));
            sd_prvy=sd_ta;
            sprintf(prikaz,"SELECT MAC_1,MAC_2,bytes_12,packets_12,bytes_21,packets_21 FROM %s_%s_MAC_SD WHERE id BETWEEN %d AND %d;",table->nazov,nl,MAC_SD_id[0],MAC_SD_id[1]);
            if (mysql_query(conn,prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }
            pocitadlo=1;
            while ((result = mysql_store_result(conn))!=NULL) {
                while ((row=mysql_fetch_row(result))!=NULL) {
                    if (pocitadlo==1) {
                        sd_ta->mac_s=atol(row[0]);
                        sd_ta->mac_d=atol(row[1]);
                        sd_ta->pocet_B12=atoi(row[2]);
                        sd_ta->pocet_ramcov12=atoi(row[3]);
                        sd_ta->pocet_B21=atoi(row[4]);
                        sd_ta->pocet_ramcov21=atoi(row[5]);
                        sd_ta->spracovany=0;
                        sd_ta->dalsi=NULL;
                    } else {
                        SD_T *sd_pom;
                        sd_pom = (SD_T*)malloc(sizeof(SD_T));
                        sd_ta->dalsi=sd_pom;
                        sd_ta = sd_ta->dalsi;
                        sd_ta->mac_s=atol(row[0]);
                        sd_ta->mac_d=atol(row[1]);
                        sd_ta->pocet_B12=atoi(row[2]);
                        sd_ta->pocet_ramcov12=atoi(row[3]);
                        sd_ta->pocet_B21=atoi(row[4]);
                        sd_ta->pocet_ramcov21=atoi(row[5]);
                        sd_ta->spracovany=0;
                        sd_ta->dalsi=NULL;
                    }
                    pocitadlo++;
                }
            }

//MAC_SD TABULKA - INSERT
            sprintf(prikaz,"INSERT INTO %s_%s_MAC_SD (MAC_1, MAC_2, bytes_12, packets_12, bytes_21, packets_21) VALUES ",table->nazov,mhd);
            for (sd_ta=sd_prvy;sd_ta!=NULL;sd_ta=sd_ta->dalsi) {
                if (sd_ta->spracovany==0) {
                    for (sd_ta2=sd_ta->dalsi;sd_ta2!=NULL;sd_ta2=sd_ta2->dalsi) {
                        if (sd_ta->mac_s==sd_ta2->mac_s && sd_ta->mac_d==sd_ta2->mac_d) {
                            sd_ta->pocet_B12+=sd_ta2->pocet_B12;
                            sd_ta->pocet_B21+=sd_ta2->pocet_B21;
                            sd_ta->pocet_ramcov12+=sd_ta2->pocet_ramcov12;
                            sd_ta->pocet_ramcov21+=sd_ta2->pocet_ramcov21;
                            sd_ta2->spracovany=1;
                        }
                        else if (sd_ta->mac_s==sd_ta2->mac_d && sd_ta->mac_d==sd_ta2->mac_s) {
                            sd_ta->pocet_B12+=sd_ta2->pocet_B21;
                            sd_ta->pocet_B21+=sd_ta2->pocet_B12;
                            sd_ta->pocet_ramcov12+=sd_ta2->pocet_ramcov21;
                            sd_ta->pocet_ramcov21+=sd_ta2->pocet_ramcov12;
                            sd_ta2->spracovany=1;
                        }
                    }
                    sprintf(kreatura,"('%llu','%llu','%lu','%lu','%lu','%lu'),", sd_ta->mac_s, sd_ta->mac_d, sd_ta->pocet_B12, 	sd_ta->pocet_ramcov12, sd_ta->pocet_B21, sd_ta->pocet_ramcov21);
                    strcat(prikaz,kreatura);
                    sd_ta->spracovany=1;
                }
            }
            prikaz[strlen(prikaz)-1]=';';
            if (mysql_query(conn, prikaz)) {
                fprintf(stderr,"Failed to insert '%s' into MYSQL database %s: %s\n",prikaz,db_name,mysql_error(conn));
            }

            for (sd_ta=sd_prvy;sd_ta!=NULL;) {
                sd_prvy=sd_ta;
                sd_ta=sd_ta->dalsi;
                free(sd_prvy);
            }
        }//ukoncenie elsu, ze naozaj treba robit cronovanie
    }//ukoncenie forcyklu na tabulky

//uvolnenie pamate (hoc sa program ukonci...)
    for (table=prvy;table!=NULL;) {
        prvy=table;
        table=table->dalsi;
        free(prvy);
    }

    if(result)
      mysql_free_result(result);
    result=NULL;
    mysql_close(conn);
}
