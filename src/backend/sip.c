#include "sip.h"

char *sip_buf;
int sippocet=0;

void SIP_vytvor_db(MYSQL *conn) {
		if(mysql_query(conn,"CREATE TABLE IF NOT EXISTS SIP_1m ( `id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY, `callid` TEXT NOT NULL, `ip_s0` INT UNSIGNED NOT NULL, `ip_s1` INT UNSIGNED NOT NULL, `ip_s2` INT UNSIGNED NOT NULL, `ip_s3` INT UNSIGNED NOT NULL, `ip_d0` INT UNSIGNED NOT NULL,`ip_d1` INT UNSIGNED NOT NULL,`ip_d2` INT UNSIGNED NOT NULL,`ip_d3` INT UNSIGNED NOT NULL, `postup` TEXT NOT NULL);")) {
			fprintf(stderr,"SIP: Failed to create tables in MYSQL database %s: %s\n",db_name, mysql_error(conn));
		}
}

void SIP_zapis_do_db(MYSQL *conn) {
	if(sippocet) {
		//FILE *fw;
		//fw = fopen("fw.txt","a");
		if(debug) printf("SIP: Zapis do DB: spravy pocet: %d\n",sippocet);
		int i=0,j;
		char pom[20000];
		sip_buf = (char *) malloc(90*sizeof(char));			
		sprintf(sip_buf,"INSERT INTO SIP_1m (callid,ip_s0,ip_s1,ip_s2,ip_s3,ip_d0,ip_d1,ip_d2,ip_d3,postup) VALUES");
		SIPMSG *pointer;
		for(pointer=first;pointer!=NULL;pointer=pointer->next) {
			//fprintf(fw,"%d\n",i);
			if(!pointer->pridany) {
				sprintf(pom,"('%s','%d','%d','%d','%d','%d','%d','%d','%d','a %d %d %d",pointer->callid,pointer->ip_s[0],pointer->ip_s[1],pointer->ip_s[2],pointer->ip_s[3],pointer->ip_d[0],pointer->ip_d[1],pointer->ip_d[2],pointer->ip_d[3],pointer->time,pointer->type,pointer->comm);
				SIPMSG *pointer2;
				for(pointer2=pointer->next;pointer2!=NULL;pointer2=pointer2->next) {
					if (!pointer2->pridany) {
						if ((strcmp(pointer->callid,pointer2->callid))==0) {
							pointer2->pridany=1;
							int porovnanie = sipporovnajip(pointer,pointer2);
							if (porovnanie==1)
								sprintf(pom,"%s a %d %d %d",pom,pointer2->time,pointer2->type,pointer2->comm);
							else if (porovnanie==0)
								sprintf(pom,"%s b %d %d %d",pom,pointer2->time,pointer2->type,pointer2->comm);
							else sprintf(pom,"%s c %d %d %d",pom,pointer2->time,pointer2->type,pointer2->comm);
						}
					}
				}
				sprintf(pom,"%s'),",pom);
				int alvel = strlen(pom)+strlen(sip_buf)+2;
				sip_buf = (char *) realloc(sip_buf,alvel*sizeof(char));
				strcat(sip_buf,pom);
        		}
			i++;
		}
		
		sip_buf[strlen(sip_buf)-1]=';';
		//write
		if (mysql_query(conn,sip_buf)) {
			fprintf(stderr,"SIP: Failed to insert data into MYSQL database %s: %s\n",db_name, mysql_error(conn));
		}
		sprintf(sip_buf,"");
		sippocet=0;
		//free
		SIPMSG *pointer3;
		for(pointer3=first;pointer3!=NULL;) {
			first = pointer3;
			pointer3=pointer3->next;
			free((void *) first);		
		}
		first=sipmsgs;
		//fclose(fw);	
	}
}

void SIP_detekcia(const u_char *pkt_data,int h_len) {
	char dats[1500];
	int i=0,g;
	int length = 1499;
	length=pkt_data[16]*256+pkt_data[17];
	for(g=h_len;((pkt_data[g]!=13) && g<length && i<1500);g++) {
		dats[i]=pkt_data[g];
		i++;
	}
	dats[i]='\0';
	if (strstr(dats,"SIP/2.0")) {
		SIP_analyza(pkt_data, h_len);
/*
		if(debug) {
             		sprintf(s_tmp_str,"SIP: Out of sip_protocol\n");
             		log(s_tmp_str,1);
		}
*/
	}
}

void SIP_analyza(const u_char *dats,int h_len) {
	int a,i,j;
	char stline[1500];
	char hline[1500];
	char st_dat_1[20];
	char st_dat_2[300];
	char st_dat_3[20];
	char hl_dat_1[1500];
	SIPMSG *new;
	new=(SIPMSG *)malloc(sizeof(SIPMSG));

// TIME
	new->time=unix_time;

////////////////////////////////////////////////////////
//  IPs
////////////////////////////////////////////////////////
	for(j=0;j<4;j++) {
		new->ip_s[j]=dats[j+26];
	}
	for(j=0;j<4;j++)
		new->ip_d[j]=dats[j+30];

////////////////////////////////////////////////////////
//  Start line
////////////////////////////////////////////////////////
	//	Coding
        //        1=ACK, 2=BYE, 3=CANCEL, 4=INFO, 5=INVITE,
        //        6=MESSAGE, 7=NOTIFY, 8=OPTIONS, 9=PRACK, 10=PUBLISH,
        //        11=REFER, 12=REGISTER, 13=SUBSCRIBE, 14=UPDATE,
        //        
	//	  1xx=Informational, 2xx=Successful, 3xx=Redirection, 4xx=Client failure, 5xx=Server failure,
        //        6xx=Global failure, UNKNOWN
        //
		
	//Read Start Line
	j=0;
	for (i=h_len;dats[i]!=13;i++) {
		stline[j]=dats[i]; //read start line;
		j++;
	}
	stline[j]=dats[i]; //save also a ending character
	j++;
	stline[j]='\0';

	//Parse Start Line
	for(j=0;stline[j]!=32;j++) st_dat_1[j]=stline[j]; //read 1st word till blank character
	j++; st_dat_1[j]='\0'; //skip blank character and end st_dat_1 string
	for(i=0;stline[j]!=32;j++) {
		st_dat_2[i]=stline[j]; //read 2nd word till blank character
		i++;
	}
	j++; st_dat_2[i]='\0'; //skip blank character, end st_dat_2 string and clear i for new addressing
	for(i=0;stline[j]!=13;j++) {
		st_dat_3[i]=stline[j];
		i++;
	}
	st_dat_3[i]='\0'; //end st_dat_3 string
	
	if (strstr(st_dat_1,"ACK")) {new->type=1; new->comm=1;}
	else if(strstr(st_dat_1,"BYE")) {new->type=1; new->comm=2;}
	else if(strstr(st_dat_1,"CANCEL")) {new->type=1; new->comm=3;}
	else if(strstr(st_dat_1,"INFO")) {new->type=1; new->comm=4;}
	else if(strstr(st_dat_1,"INVITE")) {new->type=1; new->comm=5;}
	else if(strstr(st_dat_1,"MESSAGE")) {new->type=1; new->comm=6;}
	else if(strstr(st_dat_1,"NOTIFY")) {new->type=1; new->comm=7;}
	else if(strstr(st_dat_1,"OPTIONS")) {new->type=1; new->comm=8;}
	else if(strstr(st_dat_1,"PRACK")) {new->type=1; new->comm=9;}
	else if(strstr(st_dat_1,"PUBLISH")) {new->type=1; new->comm=10;}
	else if(strstr(st_dat_1,"REFER")) {new->type=1; new->comm=11;}
	else if(strstr(st_dat_1,"REGISTER")) {new->type=1; new->comm=12;}
	else if(strstr(st_dat_1,"SUBSCRIBE")) {new->type=1; new->comm=13;}
	else if(strstr(st_dat_1,"UPDATE")) {new->type=1; new->comm=14;}
	else if(strstr(st_dat_1,"SIP/2.0")) {new->type=2;}
	else {new->type=0;new->comm=0;}
		
	if(new->type==1) sprintf(new->name,"%s",st_dat_2);
	else if (new->type==2) {
		new->comm= atoi(st_dat_2);
		sprintf(new->name, "%s %s",st_dat_2,st_dat_3);
	}
	else sprintf(new->name,"");

		
////////////////////////////////////////////////////////
//  Message Header
////////////////////////////////////////////////////////
	int odoa=0;
	for(i=h_len;dats[i]!=13;i++) ; //jump StartLine
	i=i+2; //also CR
	for(j=0;odoa==0;i++) {
		if ((dats[i]==13) && (dats[i+1]==10) && (dats[i+2]==13)) // && (dats[i+3]==10))
			odoa=1;
		else {
			hline[j]=dats[i];
			j++;
		}
	}
	hline[j]='\0';
	
	//Call ID
	char *caid;
	caid = strstr(hline,"Call-ID: ");
	if (caid!=NULL) {
		for(i=0;caid[i]!=13;i++);
		//fprintf(stderr,"i: %d\n",i);
		new->callid = (char *)malloc(i+2*sizeof(char));
		for(i=0;caid[i]!=13;i++) {
			new->callid[i]=caid[i];		
		}
		new->callid[i]='\0';
	}
	else {
		new->callid = (char *) malloc(14*sizeof(char));
		sprintf(new->callid,"Call-ID: None");			
	}
	
	new->next=NULL;
	new->pridany=0;

////////////////////////////////////////////////////////
//  Save
////////////////////////////////////////////////////////
	if(sippocet==0) {
		sipmsgs=(SIPMSG*) malloc(sizeof(SIPMSG));
		sipmsgs=new;
		first=sipmsgs;
	} else {
		sipmsgs->next=new;
		sipmsgs=sipmsgs->next;	
	}
	
	sippocet++;
}

//porovna adresy v SIPPAKETe
int sipporovnajip(SIPMSG *a, SIPMSG *b) {
	if((a->ip_s[0]==b->ip_s[0]) && (a->ip_s[1]==b->ip_s[1]) && (a->ip_s[2]==b->ip_s[2]) && (a->ip_s[3]==b->ip_s[3]) && \
		(a->ip_d[0]==b->ip_d[0]) && (a->ip_d[1]==b->ip_d[1]) && (a->ip_d[2]==b->ip_d[2]) && (a->ip_d[3]==b->ip_d[3])) return 1;
	else if((a->ip_s[0]==b->ip_d[0]) && (a->ip_s[1]==b->ip_d[1]) && (a->ip_s[2]==b->ip_d[2]) && (a->ip_s[3]==b->ip_d[3]) && \
		(a->ip_d[0]==b->ip_s[0]) && (a->ip_d[1]==b->ip_s[1]) && (a->ip_d[2]==b->ip_s[2]) && (a->ip_d[3]==b->ip_s[3])) return 0;
	else return -1;	
}
