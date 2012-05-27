#include "variables.h"
#include "katalyzer.h"
#include <stdio.h>
#include <math.h>

#ifdef NETFLOW
uint64_t hodnota(const u_char *pkt_data, int poz, int len);
NFLOW9_TEMPLATE *search_tmpl(int source_ID, int tmpl_ID);
void N9_analyza(const u_char *pkt_data, int len);
void S5_analyza(const u_char *pkt_data, int len);
void N5_analyza(const u_char *pkt_data, int len);
void template_update(time_t act_time);
void flow_buffer_update(int cas);
void save_record();
#endif
