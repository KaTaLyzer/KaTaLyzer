#include "variables.h"
#include "mysql/mysql.h"
#include "katalyzer.h"
#include <stdio.h>
#include <string.h>

extern char *sip_name;
extern char *sip_buf;
extern int sippocet;

void SIP_vytvor_db(MYSQL *conn);
void SIP_zapis_do_db(MYSQL *conn);
void SIP_detekcia(const u_char *pkt_data,int h_len);
void SIP_analyza(const u_char *dats,int h_len);
int sipporovnajip(SIPMSG *a, SIPMSG *b);



