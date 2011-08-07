#include "cdp.h"

#ifdef CDP_P
void cdp_protokol(const u_char *pkt_data, struct cdp_struct *cdp_st) {

    struct ether_header *eth;
    struct cdph *cdp;
    struct cdp_info *cdp_i;
    struct cdp_struct *cdp_pom;
    struct address *p_adr, *pp;
    u_int16_t *type;
    u_int16_t *length;
    u_int16_t eth_length;
    u_int32_t *pom;
    uint8_t *pom_protocol;
    uint8_t *pom_length;
    uint8_t *p_pomprotoloc;
    uint32_t *p_pomaddres, *number;
    uint16_t *pom_adrlen;
    int typ;
    int i,j, addres_len;

    cdp_i=(struct cdp_info*) malloc(sizeof(struct cdp_info));
    cdp_i->device_id=NULL;
    cdp_i->number_IP=0;
    cdp_i->p_addres=NULL;
    cdp_i->port_id=NULL;
    cdp_i->version=NULL;
    cdp_i->platform=NULL;

    eth=(struct ether_header*) pkt_data;
    eth_length=ntohs(eth->ether_type);
    cdp=(struct cdph*) (pkt_data + sizeof(struct ether_header) + 8);

    for (i=(sizeof(struct ether_header) + 8 + sizeof(struct cdph));i<eth_length+sizeof(struct ether_header);i+=ntohs(*length)) {
        type=(u_int16_t*) (pkt_data + i);
        length=(u_int16_t*) (pkt_data + i + sizeof(u_int16_t)); // set the length of the file
        typ=ntohs(*type); // set the type of the file
        switch (typ) {
        case 1:		// Device ID
            if ((cdp_i->device_id=(char*) malloc(ntohs(*length)+1-4)) == NULL) {	//allocate memory for device ID
                fprintf(stderr,"Error malloc for device_id: %s\n", strerror(errno));
                return;
            }
            for (j=0;j<(ntohs(*length)-4);j++) {	//copy Device ID
                cdp_i->device_id[j]=pkt_data[i+4+j];
            }
            cdp_i->device_id[ntohs(*length)-4]='\0'; // end string
            break;
        case 2:		// Address
            number=(uint32_t*) (pkt_data + i + 4);
            cdp_i->number_IP=*number; // set number of the IP address on the file
            addres_len=0;
            if (0<ntohl(cdp_i->number_IP)) {
                if ((p_adr=(struct address*) malloc(sizeof(struct address))) == NULL) { //allocate memory for IP address
                    fprintf(stderr,"Error malloc for adress: %s\n", strerror(errno));
                    return;
                }
                p_adr->p_next=NULL;
                cdp_i->p_addres=p_adr;
            }
            for (j=0;j<ntohl(cdp_i->number_IP);j++) {
                // we will set the structure with the value from the CDP protokol
                pom_protocol=(uint8_t*) (pkt_data+i+8+addres_len);
                addres_len++;
                pom_length=(uint8_t*) (pkt_data+i+8+addres_len);
                addres_len++;
                if (1==*pom_length)
                    p_pomprotoloc=(uint8_t*) (pkt_data+i+8+addres_len);
                addres_len+=*pom_length;
                pom_adrlen=(uint16_t*) (pkt_data+i+8+addres_len);
                addres_len+=2;
                if (4==ntohs(*pom_adrlen))
                    p_pomaddres=(uint32_t*) (pkt_data+i+8+addres_len);
                addres_len+=ntohs(*pom_adrlen);
                p_adr->protocol=*pom_protocol;
                p_adr->length=*pom_length;
                p_adr->adr_len=*pom_adrlen;
                if (0xCC== *p_pomprotoloc) {
                    p_adr->pprotocol=*p_pomprotoloc;
                    p_adr->adress=ntohl(*p_pomaddres);
                }
                // if exist any IP address we create the structure
                if ((j+1)<ntohs(cdp_i->number_IP)) {
                    pp=p_adr;
                    pp->p_next=NULL;
                    if ((p_adr=(struct address*) malloc(sizeof(struct address))) == NULL) {
                        fprintf(stderr,"Error malloc for address: %s\n", strerror(errno));
                        return;
                    }
                    p_adr->p_next=NULL;
                    pp->p_next=p_adr;
                }
            }
            break;
        case 3:		// Port ID
            if ((cdp_i->port_id=(char*) malloc(ntohs(*length)+1-4)) == NULL) { // allocate memory for Port ID
                fprintf(stderr,"Error malloc for port ID: %s\n", strerror(errno));
                return;
            }
            for (j=0;j<(ntohs(*length)-4);j++) {	//copy Port ID
                cdp_i->port_id[j]=pkt_data[i+4+j];
            }
            cdp_i->port_id[ntohs(*length)-4]='\0';
            break;
        case 4:		// Capabilities
            pom=(u_int32_t*) (pkt_data+i+4);
            cdp_i->capabil=*pom;	//set the Capabilities
            break;
        case 5:		// Version
            if ((cdp_i->version=(char*) malloc(ntohs(*length)+1-4)) == NULL) { // allocate memory for Version
                fprintf(stderr,"Error malloc for version: %s\n", strerror(errno));
                return;
            }
            for (j=0;j<(ntohs(*length)-4);j++) {	//copy Version
                cdp_i->version[j]=pkt_data[i+4+j];
            }
            cdp_i->version[ntohs(*length)-4]='\0';
            break;
        case 6:		// Platform
            if ((cdp_i->platform=(char*) malloc(ntohs(*length)+1-4)) == NULL) { // allocate memory for Platform
                fprintf(stderr,"Error malloc for platform: %s\n", strerror(errno));
                return;
            }
            for (j=0;j<(ntohs(*length)-4);j++) {	// copy Platform
                cdp_i->platform[j]=pkt_data[i+4+j];
            }
            cdp_i->platform[ntohs(*length)-4]='\0';
            break;
        case 7:		// IP Prefix
            break;
        }
    }

//fprintf(stderr,"ID: %s, Version: %s Platform: %s, Addres: %u\n", cdp_i->device_id, cdp_i->version, cdp_i->platform, cdp_i->p_addres->adress);

//here we copy value which is store in structure cdp_info to structure cdp_struct
    if (cdp_st->empty) {
        cdp_st->empty=0;
        cdp_st->ttl=cdp->ttl;
        if (cdp_i->device_id!=NULL) {
            if ((cdp_st->device_id=(char*) malloc(strlen(cdp_i->device_id)+1)) == NULL) {
                fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                return;
            }
            strcpy(cdp_st->device_id,cdp_i->device_id);
        }
        else
            cdp_st->device_id=NULL;
        if (cdp_i->port_id!=NULL) {
            if ((cdp_st->port_id=(char*) malloc(strlen(cdp_i->port_id)+1)) == NULL) {
                fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                return;
            }
            strcpy(cdp_st->port_id, cdp_i->port_id);
        }
        else
            cdp_st->port_id=NULL;
        cdp_st->capabil=cdp_i->capabil;
        if (cdp_i->version!=NULL) {
            if ((cdp_st->version=(char*) malloc(strlen(cdp_i->version)+1)) == NULL) {
                fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                return;
            }
            strcpy(cdp_st->version, cdp_i->version);
        }
        else
            cdp_st->version=NULL;
        if (cdp_i->platform!=NULL) {
            if ((cdp_st->platform=(char*) malloc(strlen(cdp_i->platform)+1)) == NULL ) {
                fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                return;
            }
            strcpy(cdp_st->platform, cdp_i->platform);
        }
        else
            cdp_st->platform=NULL;
        if (cdp_i->p_addres!=NULL) {
            if ((cdp_st->p_addres=(struct address*) malloc(sizeof(struct address))) == NULL) {
                fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                return;
            }
            for (p_adr=cdp_i->p_addres,pp=cdp_st->p_addres;p_adr!=NULL;p_adr=p_adr->p_next) {
                pp->protocol=p_adr->protocol;
                pp->length=p_adr->length;
                pp->pprotocol=p_adr->pprotocol;
                pp->adr_len=p_adr->adr_len;
                pp->adress=p_adr->adress;
                if (p_adr->p_next!=NULL) {
                    if ((pp->p_next=(struct address*) malloc(sizeof(struct address))) == NULL) {
                        fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                        return;
                    }
                    pp=pp->p_next;
                }
                else
                    pp->p_next=NULL;
            }
        }
        else
            cdp_st->p_addres=NULL;
    }
    else {
        int find;
        for (cdp_pom=cdp_st;cdp_pom!=NULL;cdp_pom=cdp_pom->p_next) {
            find=0;
            if ((!(strcmp(cdp_pom->device_id, cdp_i->device_id))) && (!(strcmp(cdp_pom->version, cdp_i->version)))) {
                find=1;
                break;
            }
        }
        if (!(find)) {
            for (cdp_pom=cdp_st;cdp_pom->p_next!=NULL;cdp_pom=cdp_pom->p_next) {
            }
            cdp_pom->empty=0;
            cdp_pom->ttl=cdp->ttl;
            if (cdp_i->device_id!=NULL) {
                if ((cdp_pom->device_id=(char*) malloc(strlen(cdp_i->device_id)+1)) == NULL) {
                    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                    return;
                }
                strcpy(cdp_pom->device_id,cdp_i->device_id);
            }
            else
                cdp_pom->device_id=NULL;
            if (cdp_i->port_id!=NULL) {
                if ((cdp_pom->port_id=(char*) malloc(strlen(cdp_i->port_id)+1)) == NULL) {
                    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                    return;
                }
                strcpy(cdp_pom->port_id, cdp_i->port_id);
            }
            else
                cdp_pom->port_id=NULL;
            cdp_pom->capabil=cdp_i->capabil;
            if (cdp_i->version!=NULL) {
                if ((cdp_pom->version=(char*) malloc(strlen(cdp_i->version)+1)) == NULL) {
                    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                    return;
                }
                strcpy(cdp_pom->version, cdp_i->version);
            }
            else
                cdp_pom->version=NULL;
            if (cdp_i->platform!=NULL) {
                if ((cdp_pom->platform=(char*) malloc(strlen(cdp_i->platform)+1)) == NULL) {
                    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                    return;
                }
                strcpy(cdp_pom->platform, cdp_i->platform);
            }
            else
                cdp_pom->platform=NULL;
            if (cdp_i->p_addres!=NULL) {
                if ((cdp_pom->p_addres=(struct address*) malloc(sizeof(struct address))) == NULL) {
                    fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                    return;
                }
                for (p_adr=cdp_i->p_addres,pp=cdp_pom->p_addres;p_adr!=NULL;p_adr=p_adr->p_next) {
                    pp->protocol=p_adr->protocol;
                    pp->length=p_adr->length;
                    pp->pprotocol=p_adr->pprotocol;
                    pp->adr_len=p_adr->adr_len;
                    pp->adress=p_adr->adress;
                    if (p_adr->p_next!=NULL) {
                        if ((pp->p_next=(struct address*) malloc(sizeof(struct address))) == NULL) {
                            fprintf(stderr,"Error malloc: %s\n", strerror(errno));
                            return;
                        }
                        pp=pp->p_next;
                    }
                    else {
                        fprintf(stderr,"Null\n");
                        pp->p_next=NULL;
                    }
                }
            }
            else
                cdp_pom->p_addres=NULL;
        }
    }

// free the structure cdp_info
    if (cdp_i->device_id!=NULL)
        free(cdp_i->device_id);
    if (cdp_i->port_id!=NULL)
        free(cdp_i->port_id);
    if (cdp_i->version!=NULL)
        free(cdp_i->version);
    if (cdp_i->platform!=NULL)
        free(cdp_i->platform);
    struct address *pom_adr1;
    struct address *pom_adr2;
    pom_adr1=cdp_i->p_addres;
    while (pom_adr1!=NULL) {
        pom_adr2=pom_adr1->p_next;
        free(pom_adr1);
        pom_adr1=pom_adr2;
    }
    free(cdp_i);
}

void cdp_processing(struct cdp_struct *cdp_st, MYSQL *conn) {
    char prikaz[20000], prikaz1[20000];
    char temp_str[10000], temp_str1[10000];
    char cap[10000];
    char temp[15];
    unsigned int id=0, i, j, cap_pom;
    MYSQL_RES *result;
    MYSQL_ROW row;
    struct cdp_struct *pom;
    struct address *ad;

//TIME
    if (!(cdp_st->empty)) {
        sprintf(prikaz,"SELECT MAX(id) FROM CDP_1mm;");
        if (mysql_query(conn,prikaz)) {
            fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error(conn));
            sprintf(s_tmp_str,"%s:processing.c:processing:Failed to insert data into MYSQL database %s in processing TIME-1a: %ld\n", db_name, mysql_error(conn),time(&actual_time));
        }
        result = mysql_store_result(conn);
        row=mysql_fetch_row(result);
        if (row[0]!=NULL) id= atoi(row[0]);
        mysql_free_result(result);
        id++;

        sprintf(prikaz,"INSERT INTO CDP_1mm_time (time, id) VALUES (%d, %d);", unix_time, id);
        if (mysql_query(conn, prikaz)) {
            fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error(conn));
            sprintf(s_tmp_str,"%s:processing.c:processing:Failed to insert data into MYSQL database %s in processing TIME5: %ld\n", db_name, mysql_error(conn),time(&actual_time));
        }

        sprintf(prikaz,"INSERT INTO CDP_1mm (ttl, device_ID, Capabilities, version, platform ) VALUES ");
        for (pom=cdp_st;pom!=NULL;pom=pom->p_next) {
            sprintf(cap," ");
            for (i=0,j=1;i<7;i++) {
                cap_pom=pom->capabil&j;
                j<<=1;
                switch (cap_pom) {
                case 0x01:
                    strcat(cap,"IS a Router, ");
                    break;
                case 0x02:
                    strcat(cap,"IS a Transparent Bridge, ");
                    break;
                case 0x04:
                    strcat(cap,"IS a Source Route Bridge, ");
                    break;
                case 0x08:
                    strcat(cap,"IS a Switch, ");
                    break;
                case 0x10:
                    strcat(cap,"IS a Host, ");
                    break;
                case 0x20:
                    strcat(cap,"IS a IGMP capable, ");
                    break;
                case 0x40:
                    strcat(cap,"IS a Repeater, ");
                    break;
                }
            }
            cap[strlen(cap)-2]=' ';
            sprintf(temp_str,"('%d', '%s', '%s', '%s', '%s'),",pom->ttl, pom->device_id, cap, pom->version, pom->platform);
            strcat(prikaz, temp_str);
// Addres
            if (pom->p_addres!=NULL) {
                sprintf(prikaz1,"INSERT INTO CDP_ADDRS_1m (id, IP) VALUES ");
                for (ad=pom->p_addres;ad!=NULL;ad=ad->p_next) {
                    sprintf(temp_str1,"('%u', '%u'),", id, ad->adress);
                    strcat(prikaz1,temp_str1);
                    id++;
                }
                prikaz1[strlen(prikaz1)-1]=';';
                if (mysql_query(conn, prikaz1)) {
                    fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error(conn));
                }
            }
        }
        prikaz[strlen(prikaz)-1]=';';
        if (mysql_query(conn, prikaz)) {
            fprintf(stderr,"Failed to insert //%s// into MYSQL database %s: %s\n",prikaz,db_name, mysql_error(conn));
        }
    }
}

#endif
