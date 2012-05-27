#ifdef SNMP_P

#include "snmpp.h"

void snmp_protokol(const u_char *pkt_data, SNMP_STRUK *sn){

	const u_char *snmp;
	char *string, tmp[4], snmp_pom;
	unsigned char ll[100];
	int i,len, *ale;
	int snmp_find=0;
	SNMP_STRUK *pom;

	iph=(struct iphdr*) (pkt_data + sizeof(struct ether_header));

	udph=(struct udphdr*) (pkt_data + sizeof(struct ether_header) + iph->ihl*4);
//	fprintf(stderr,"Pok:%d\n", ntohs(udph->len));
	snmp=(pkt_data + sizeof(struct ether_header) + iph->ihl*4 + sizeof(struct udphdr));
//	fprintf(stderr,"%p: %x\n",&snmp[0],snmp[0]);
	string=(char*) malloc((ntohs(udph->len)-sizeof(struct udphdr))*3);
	strcpy(string,"");

//	fprintf(stderr,"%d\n",sizeof(struct ether_header) + iph->ihl*4 + sizeof(struct udphdr));
//	fprintf(stderr,"%p: %x\n",&snmp[0],snmp[0]);


	for(i=0;i<ntohs(udph->len)- sizeof(struct udphdr);i++){
//		fprintf(stderr,"tmp %d:%x \n",i,snmp[i]);
//		fprintf(stderr,"%x\n",snmp[i]);
		if(0xF<snmp_pom)
			sprintf(tmp,"%X",snmp[i]);
		else
			sprintf(tmp,"0%X",snmp[i]);
		strcat(string,tmp);
		fprintf(stderr,"Len: %d\n", strlen(string));
//		fprintf(stderr,"st: %s\n",string);
	}

	if(sn->empty==1){
		sn->empty=0;
		sn->IP_S=IP_adr_S;
		sn->IP_D=IP_adr_D;
		sn->bytes=ntohs(udph->len)-sizeof(struct udphdr);
		sn->number=1;
		sn->p_snmp=(char*) malloc(strlen(string)+1);
		strcpy(sn->p_snmp,string);
	}
	else{
		for(pom=sn;pom->p_next!=NULL;pom=pom->p_next){
			if((sn->IP_S==IP_adr_S) && (sn->IP_D==IP_adr_D)){
				sn->number++;
				sn->bytes+=ntohs(udph->len)-sizeof(struct udphdr);
				len=strlen(sn->p_snmp);
				sn->p_snmp=(char*) realloc(sn->p_snmp,len+(strlen(string)+1));
				strcat(sn->p_snmp,string);
				snmp_find=1;
				break;
			}
			else
				snmp_find=0;
		}
		if(!snmp_find){
			pom->p_next=create_snmp();
			pom=pom->p_next;
			pom->empty=0;
			pom->IP_S=IP_adr_S;
			pom->IP_D=IP_adr_D;
			pom->bytes=ntohs(udph->len)-sizeof(struct udphdr);
			pom->number=1;
			pom->p_snmp=(char*) malloc(strlen(string)+1);
			strcpy(pom->p_snmp,string);
		}
	}

	fprintf(stderr,"SNMP: %s\n", string);
	free(string);

}

SNMP_STRUK *create_snmp(){
	SNMP_STRUK *pom;
	pom=(SNMP_STRUK*) malloc(sizeof(SNMP_STRUK));
	pom->empty=1;
	pom->p_next=NULL;
	return pom;
}

void procesing_snmp(SNMP_STRUK s, MYSQL *conn){
	char prikaz[20000];
	char temp_str[10000];
	char temp[15];  // premenna pre ulozenie konvertovanej IP adresy z formatu int spat do stringu - pre zapis do DB sa bude nadalej pouzivat a.b.c.d format
	int sn=0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	//////////
	// TIME //
	//////////
	if(!s.empty)
	{
		if(!s.empty) {
			sprintf(prikaz,"SELECT MAX(id) FROM SNMP_1m;");
			if(mysql_query(conn,prikaz)) {
				fprintf(stderr,"Failed to insert data into MYSQL database %s in processing SNMP TIME1: %s\n",db_name, mysql_error(conn));
			}
			result = mysql_store_result(conn);
			row=mysql_fetch_row(result);
			if(row[0]!=NULL) sn = atoi(row[0]);
			mysql_free_result(result);
			sn++;
		} else {
			sprintf(prikaz,"SELECT MAX(id) FROM SNMP_time;");
			if(mysql_query(conn,prikaz)) {
				fprintf(stderr,"Failed to insert data into MYSQL database %s in processing SNMP_time TIME1: %s\n",db_name, mysql_error(conn));
			}
			result = mysql_store_result(conn);
			row=mysql_fetch_row(result);
			if(row[0]!=NULL) sn = atoi(row[0]);
			mysql_free_result(result);
		}
		sprintf(prikaz,"INSERT INTO SNMP_time (time, id ) VALUES ('%d','%d');", unix_time, sn);
		fprintf(stderr,"TIME: %s\n",prikaz );
		if(mysql_query(conn, prikaz))
		{
			fprintf(stderr,"Failed to insert data into MYSQL database %s in processing SNMP_time TIME5: %s\n",db_name, mysql_error(conn));
		}
	}

	//////////
	// SNMP //
	//////////

	if(!s.empty){
		sprintf(prikaz,"INSERT INTO SNMP_1m (IP_S,IP_D, bytes, number, SNMP) VALUES ");
		SNMP_STRUK *snm;
			for(snm=&s;snm->p_next!=NULL;snm=snm->p_next) 
		{
			sprintf(temp_str,"('%u','%u','%d','%d','%s'),",snm->IP_S, snm->IP_D, snm->bytes, snm->number, snm->p_snmp); 
			strcat(prikaz,temp_str);
		}
		prikaz[strlen(prikaz)-1]=';';  //nahradenie poslednej ciarky , bodkociarkou ; na ukoncenie prikazu
		if(mysql_query(conn, prikaz)) 
		{
			fprintf(stderr,"Failed to insert data into MYSQL database %s in processing SNMP: %s\n",db_name, mysql_error(conn));
		}
	}

}

#endif

