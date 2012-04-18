#include "nflow_sflow.h"

#ifdef NETFLOW

uint64_t hodnota(const u_char *pkt_data, int poz, int len) { //this function returns valeu from pkt_data where position and length is known
    int i;
    uint64_t sum = 0;

    for (i = 0; i < len; i++) {
        sum = sum + (pkt_data[poz + i] * pow(256, (len - i - 1)));
    }
    return sum;
}

NFLOW9_TEMPLATE *search_tmpl(int source_ID, int tmpl_ID) { //search template according source ID and template ID values

    if (n9_tmpl_first == NULL) {//there are no stored templates
        return NULL;
    }
    n9_tmpl_akt = n9_tmpl_first;
    while (n9_tmpl_akt != NULL) { //for all templates
        if ((n9_tmpl_akt->source_ID == source_ID) && (n9_tmpl_akt->template_ID == tmpl_ID)) { //this is correct template, we are looking for
            return n9_tmpl_akt;
        }
        n9_tmpl_akt = n9_tmpl_akt->next;
    }
    return NULL; //if we didn't find any correct template, NULL is returned
}

void N9_analyza(const u_char *pkt_data, int len) { //NetFlow version 9 analysis
    int cntv6, set_cnt, source_id, j, next_set_pos, tmpl_id, tmpl_cnt, rel_len, field_type, field_len, flow_type, dd, krk;

    //len is the first position in pkt_data, what is payload of UDP/TCP datagram
    if (hodnota(pkt_data, len, 2) == 9) { //is it netflow v.9?
        set_cnt = hodnota(pkt_data, len - 4, 2) - 8 + len; //size of UDP/TCP payload
        source_id = hodnota(pkt_data, len + 16, 4);
        len += 20;
        while (set_cnt > len) { //repeat until reached end of datagram
            flow_type = hodnota(pkt_data, len, 2);
            if (flow_type == 0) { //template
                next_set_pos = len + hodnota(pkt_data, len + 2, 2); //position of next set
                len += 4;
                while (len < next_set_pos) {//for all records in current set
                    tmpl_id = hodnota(pkt_data, len, 2);
                    tmpl_cnt = hodnota(pkt_data, len + 2, 2);
                    n9_tmpl_akt = search_tmpl(source_id, tmpl_id); //we're looking if we have thit template stored yet
                    if (n9_tmpl_akt != NULL) {//we found template - update time
                        len = len + 4 + (tmpl_cnt * 4);
                        n9_tmpl_akt->add_time = unix_time;
                    } else {//template not found, we will store this new template
                        len += 4;
                        if (n9_tmpl_first == NULL) { //empty structure of templates
                            if ((n9_tmpl_first = (NFLOW9_TEMPLATE *) malloc(sizeof (NFLOW9_TEMPLATE))) == NULL) {
                                fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                                return;
                            }
                            n9_tmpl = n9_tmpl_first;
                        } else { //not empty structure - we must go at the end of list
                            n9_tmpl = n9_tmpl_first;
                            while (n9_tmpl->next != NULL) {
                                n9_tmpl = n9_tmpl->next;
                            }
                            if ((n9_tmpl->next = (NFLOW9_TEMPLATE *) malloc(sizeof (NFLOW9_TEMPLATE))) == NULL) {
                                fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                                return;
                            }
                            n9_tmpl = n9_tmpl->next;
                        }
                        //now, empty record is ready in memory, we can store values
                        rel_len = 0;
                        n9_tmpl->source_ID = source_id;
                        n9_tmpl->template_ID = tmpl_id;
                        n9_tmpl->ip_s_pos = -1;
                        n9_tmpl->ip_s_len = -1;
                        n9_tmpl->ip_d_pos = -1;
                        n9_tmpl->ip_d_len = -1;
                        n9_tmpl->ipv6_s_pos = -1;
                        n9_tmpl->ipv6_s_len = -1;
                        n9_tmpl->ipv6_d_pos = -1;
                        n9_tmpl->ipv6_d_len = -1;
                        n9_tmpl->mac_s_pos = -1;
                        n9_tmpl->mac_s_len = -1;
                        n9_tmpl->mac_d_pos = -1;
                        n9_tmpl->mac_d_len = -1;
                        n9_tmpl->pocet_B_pos = -1;
                        n9_tmpl->pocet_B_len = -1;
                        n9_tmpl->pocet_ramcov_pos = -1;
                        n9_tmpl->pocet_ramcov_len = -1;
                        n9_tmpl->protocol_pos = -1;
                        n9_tmpl->protocol_len = -1;
                        n9_tmpl->src_port_pos = -1;
                        n9_tmpl->src_port_len = -1;
                        n9_tmpl->dst_port_pos = -1;
                        n9_tmpl->dst_port_len = -1;
                        n9_tmpl->next = NULL;
                        for (j = 0; j < tmpl_cnt; j++) {//for all fields in captured template, save positions and lengths
                            field_type = hodnota(pkt_data, len, 2);
                            field_len = hodnota(pkt_data, len + 2, 2);

                            switch (field_type) { //only supported field types will be stored, _pos is a position of field type in data flowset, _len is length of field type
                                case 1: //bytes count
                                    n9_tmpl->pocet_B_pos = rel_len;
                                    n9_tmpl->pocet_B_len = field_len;
                                    break;
                                case 2: //packet count
                                    n9_tmpl->pocet_ramcov_pos = rel_len;
                                    n9_tmpl->pocet_ramcov_len = field_len;
                                    break;
                                case 8: //source IP address
                                    n9_tmpl->ip_s_pos = rel_len;
                                    n9_tmpl->ip_s_len = field_len;
                                    break;
                                case 12: //destination IP address
                                    n9_tmpl->ip_d_pos = rel_len;
                                    n9_tmpl->ip_d_len = field_len;
                                    break;
                                case 27: //source IPv6 address
                                    n9_tmpl->ipv6_s_pos = rel_len;
                                    n9_tmpl->ipv6_s_len = field_len;
                                    break;
                                case 28: //destination IPv6 address
                                    n9_tmpl->ipv6_d_pos = rel_len;
                                    n9_tmpl->ipv6_d_len = field_len;
                                    break;
                                case 56: //source MAC address
                                    n9_tmpl->mac_s_pos = rel_len;
                                    n9_tmpl->mac_s_len = field_len;
                                    break;
                                case 57: //destination MAC address
                                    n9_tmpl->mac_d_pos = rel_len;
                                    n9_tmpl->mac_d_len = field_len;
                                    break;
                                case 4: //protocol
                                    n9_tmpl->protocol_pos = rel_len;
                                    n9_tmpl->protocol_len = field_len;
                                    break;
                                case 7: //source port
                                    n9_tmpl->src_port_pos = rel_len;
                                    n9_tmpl->src_port_len = field_len;
                                    break;
                                case 11: //destination port
                                    n9_tmpl->dst_port_pos = rel_len;
                                    n9_tmpl->dst_port_len = field_len;
                                    break;
                            }
                            rel_len += field_len;
                            len += 4;
                        }
                        n9_tmpl->offset = rel_len; //data record length
                        n9_tmpl->add_time = /*time(NULL)*/unix_time; //we store actual time, because every template has limited life
                    }
                }
            } else if (flow_type >= 256) { // data flowset
                tmpl_id = hodnota(pkt_data, len, 2); //each data flowset has template ID, so we need to find correct template to save data from data flowset
                next_set_pos = len + hodnota(pkt_data, len + 2, 2); //position of next set
                selected_tmpl = search_tmpl(source_id, tmpl_id); //find correct template

                if (selected_tmpl != NULL) {// template was found
                    len += 4;
                    while (len < next_set_pos) { //for all records in current set
                        if ((len + 3) >= next_set_pos) { //padding
                            len = next_set_pos;
                            break;
                        }
                        flow_MAC_adr_S = 0;
                        flow_MAC_adr_D = 0;
                        flow_IP_adr_S = 0;
                        flow_IP_adr_D = 0;
                        flow_pocet_paketov = 0;
                        flow_pocet_B = 0;
                        flow_src_port = 0;
                        flow_dst_port = 0;
                        flow_protocol = 0;
                        flow_capttime = 0;
                        if ((selected_tmpl->ipv6_s_pos != -1) && (selected_tmpl->ipv6_d_pos != -1)) {
                            is_ipv6ext = 1;
                        } else {
                            is_ipv6ext = 0;
                        }

                        //if flow type is saved in template (its position and length), we can save value from data record
                        if (selected_tmpl->ip_s_pos != -1) {
                            flow_IP_adr_S = hodnota(pkt_data, len + selected_tmpl->ip_s_pos, selected_tmpl->ip_s_len);
                        }
                        if (selected_tmpl->ip_d_pos != -1) {
                            flow_IP_adr_D = hodnota(pkt_data, len + selected_tmpl->ip_d_pos, selected_tmpl->ip_d_len);
                        }
                        if ((selected_tmpl->ipv6_s_pos != -1) && (selected_tmpl->ipv6_d_pos != -1)) {
                            if ((flow_IPV6_adr_D = (int*) malloc(sizeof (int) *IPV6SIZE)) == NULL) {
                                fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                                return;
                            }
                            if ((flow_IPV6_adr_S = (int*) malloc(sizeof (int) *IPV6SIZE)) == NULL) {
                                fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                                return;

                            }
                            if ((IPV6SIZE == selected_tmpl->ipv6_s_len / 4) && (IPV6SIZE == selected_tmpl->ipv6_d_len / 4)) {
                                for (cntv6 = 0; cntv6 < IPV6SIZE; cntv6++) {
                                    *(flow_IPV6_adr_S + cntv6) = hodnota(pkt_data, len + selected_tmpl->ipv6_s_pos + (cntv6 * 4), 4);
                                    *(flow_IPV6_adr_D + cntv6) = hodnota(pkt_data, len + selected_tmpl->ipv6_d_pos + (cntv6 * 4), 4);
                                }
                            }
                        }
                        if (selected_tmpl->mac_s_pos != -1) {
                            flow_MAC_adr_S = hodnota(pkt_data, len + selected_tmpl->mac_s_pos, selected_tmpl->mac_s_len);
                        }
                        if (selected_tmpl->mac_d_pos != -1) {
                            flow_MAC_adr_D = hodnota(pkt_data, len + selected_tmpl->mac_d_pos, selected_tmpl->mac_d_len);
                        }
                        if (selected_tmpl->pocet_B_pos != -1) {
                            flow_pocet_B = hodnota(pkt_data, len + selected_tmpl->pocet_B_pos, selected_tmpl->pocet_B_len);
                        }
                        if (selected_tmpl->pocet_ramcov_pos != -1) {
                            flow_pocet_paketov = hodnota(pkt_data, len + selected_tmpl->pocet_ramcov_pos, selected_tmpl->pocet_ramcov_len);
                        }
                        if (selected_tmpl->protocol_pos != -1) {
                            flow_protocol = hodnota(pkt_data, len + selected_tmpl->protocol_pos, selected_tmpl->protocol_len);
                        }
                        if (selected_tmpl->src_port_pos != -1) {
                            flow_src_port = hodnota(pkt_data, len + selected_tmpl->src_port_pos, selected_tmpl->src_port_len);
                        }
                        if (selected_tmpl->dst_port_pos != -1) {
                            flow_dst_port = hodnota(pkt_data, len + selected_tmpl->dst_port_pos, selected_tmpl->dst_port_len);
                        }

                        save_record(); //store results in memory for usually 1 minute interval
                        len += selected_tmpl->offset; //shift to next data record
                    }
                } else { //template wasn't found, we need to save data into buffer and wait until template comes

                    if (n9_buf_first == NULL) { //if buffer isn't created, create it
                        if ((n9_buf_first = (NFLOW9_BUFFER *) malloc(sizeof (NFLOW9_BUFFER))) == NULL) {
                            fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                            return;
                        }
                        n9_buf_akt = n9_buf_first;
                    } else { //if buffer is created, go at the end
                        n9_buf_akt = n9_buf_first;
                        while (n9_buf_akt->next != NULL) {
                            n9_buf_akt = n9_buf_akt->next;
                        }
                        if ((n9_buf_akt->next = (NFLOW9_BUFFER *) malloc(sizeof (NFLOW9_BUFFER))) == NULL) { //create new record in buffer
                            fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                            return;
                        }
                        n9_buf_akt = n9_buf_akt->next;
                    }
                    //now we can store measured data into buffer
                    n9_buf_akt->capture_time = unix_time; //this is actual time, when data arrives
                    n9_buf_akt->database_time = 0;
                    n9_buf_akt->next = NULL;

                    if ((n9_buf_akt->datagram = (u_char *) malloc((next_set_pos - len) * sizeof (u_char))) == NULL) { //we create memory space for data, which we want save into buffer
                        fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                        return;
                    }
                    n9_buf_akt->ID = tmpl_id;
                    n9_buf_akt->flowset_len = next_set_pos - len;
                    for (dd = 0; dd < n9_buf_akt->flowset_len; dd++) { //store data flowset
                        n9_buf_akt->datagram[dd] = (u_char) pkt_data[len + dd];
                    }
                    len = next_set_pos;
                }

            } else if ((flow_type > 0) && (flow_type < 256)) { //other flowsets - not supported
                len = len + hodnota(pkt_data, len + 2, 2);
            }
        }
        //datagram is checked succesfully

        //now we will check saved data from buffer (if we find missing template)
        n9_buf_akt = n9_buf_first;
        n9_buf_pom = n9_buf_first;
        while (n9_buf_akt != NULL) { //for all records in buffer
            selected_tmpl = search_tmpl(source_id, n9_buf_akt->ID); //find template
            if (selected_tmpl != NULL) { //template found
                len = 4;
                while (len < n9_buf_akt->flowset_len) {
                    if ((len + 3) >= n9_buf_akt->flowset_len) { //padding
                        len += 4;
                        break;
                    }
                    flow_MAC_adr_S = 0;
                    flow_MAC_adr_D = 0;
                    flow_IP_adr_S = 0;
                    flow_IP_adr_D = 0;
                    flow_pocet_paketov = 0;
                    flow_pocet_B = 0;
                    flow_src_port = 0;
                    flow_dst_port = 0;
                    flow_protocol = 0;
                    //flow_IPV6_adr_S = NULL;
                    //flow_IPV6_adr_D = NULL;
                    if ((selected_tmpl->ipv6_s_pos != -1) && (selected_tmpl->ipv6_d_pos != -1)) {
                        is_ipv6ext = 1;
                    } else {
                        is_ipv6ext = 0;
                    }

                    if (selected_tmpl->ip_s_pos != -1) {
                        flow_IP_adr_S = hodnota(n9_buf_akt->datagram, len + selected_tmpl->ip_s_pos, selected_tmpl->ip_s_len);
                    }
                    if (selected_tmpl->ip_d_pos != -1) {
                        flow_IP_adr_D = hodnota(n9_buf_akt->datagram, len + selected_tmpl->ip_d_pos, selected_tmpl->ip_d_len);
                    }
                    if ((selected_tmpl->ipv6_s_pos != -1) && (selected_tmpl->ipv6_d_pos != -1)) {
                        if ((flow_IPV6_adr_D = (int*) malloc(sizeof (int) *IPV6SIZE)) == NULL) {
                            fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                            return;
                        }
                        if ((flow_IPV6_adr_S = (int*) malloc(sizeof (int) *IPV6SIZE)) == NULL) {
                            fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                            return;

                        }
                        if ((IPV6SIZE == selected_tmpl->ipv6_s_len / 4) && (IPV6SIZE == selected_tmpl->ipv6_d_len / 4)) {
                            for (cntv6 = 0; cntv6 < IPV6SIZE; cntv6++) {
                                *(flow_IPV6_adr_S + cntv6) = hodnota(n9_buf_akt->datagram, len + selected_tmpl->ipv6_s_pos + (cntv6 * 4), 4);
                                *(flow_IPV6_adr_D + cntv6) = hodnota(n9_buf_akt->datagram, len + selected_tmpl->ipv6_d_pos + (cntv6 * 4), 4);
                            }
                        }
                    }
                    if (selected_tmpl->mac_s_pos != -1) {
                        flow_MAC_adr_S = hodnota(n9_buf_akt->datagram, len + selected_tmpl->mac_s_pos, selected_tmpl->mac_s_len);
                    }
                    if (selected_tmpl->mac_d_pos != -1) {
                        flow_MAC_adr_D = hodnota(n9_buf_akt->datagram, len + selected_tmpl->mac_d_pos, selected_tmpl->mac_d_len);
                    }
                    if (selected_tmpl->pocet_B_pos != -1) {
                        flow_pocet_B = hodnota(n9_buf_akt->datagram, len + selected_tmpl->pocet_B_pos, selected_tmpl->pocet_B_len);
                    }
                    if (selected_tmpl->pocet_ramcov_pos != -1) {
                        flow_pocet_paketov = hodnota(n9_buf_akt->datagram, len + selected_tmpl->pocet_ramcov_pos, selected_tmpl->pocet_ramcov_len);
                    }
                    if (selected_tmpl->protocol_pos != -1) {
                        flow_protocol = hodnota(n9_buf_akt->datagram, len + selected_tmpl->protocol_pos, selected_tmpl->protocol_len);
                    }
                    if (selected_tmpl->src_port_pos != -1) {
                        flow_src_port = hodnota(n9_buf_akt->datagram, len + selected_tmpl->src_port_pos, selected_tmpl->src_port_len);
                    }
                    if (selected_tmpl->dst_port_pos != -1) {
                        flow_dst_port = hodnota(n9_buf_akt->datagram, len + selected_tmpl->dst_port_pos, selected_tmpl->dst_port_len);
                    }
                    if (n9_buf_akt->database_time != 0) { //database time is time, when writing into database occured
                        flow_capttime = n9_buf_akt->database_time;
                    } else {
                        flow_capttime = n9_buf_akt->capture_time;
                    }

                    save_record(); //store results
                    len += selected_tmpl->offset; //shift to next data record
                }
                //now, we can delete record from buffer
                if (n9_buf_akt == n9_buf_first) {
                    n9_buf_first = n9_buf_first->next;
                    free(n9_buf_akt);
                } else {
                    n9_buf_pom->next = n9_buf_akt->next;
                    free(n9_buf_akt);
                }
            }
            if (n9_buf_akt != NULL) { //go to the next record
                n9_buf_pom = n9_buf_akt;
                n9_buf_akt = n9_buf_akt->next;
            } else {
                n9_buf_akt = n9_buf_pom->next;
            }
        }
    }
}

void N5_analyza(const u_char *pkt_data, int len) { //netflow version 5 analysis
    int sample_cnt, i;

    if (hodnota(pkt_data, len, 2) == 5) { //NetFlow v.5 was found
        sample_cnt = hodnota(pkt_data, len + 2, 2); //number of samples
        len += 24;
        for (i = 0; i < sample_cnt; i++) {//for all samples
            flow_MAC_adr_S = 0;
            flow_MAC_adr_D = 0;
            flow_IP_adr_S = 0;
            flow_IP_adr_D = 0;
            flow_pocet_paketov = 0;
            flow_pocet_B = 0;
            flow_src_port = 0;
            flow_dst_port = 0;
            flow_protocol = 0;
            flow_capttime = 0;

            //now we save all values
            flow_IP_adr_S = hodnota(pkt_data, len, 4);
            flow_IP_adr_D = hodnota(pkt_data, len + 4, 4);
            flow_pocet_paketov = hodnota(pkt_data, len + 16, 4);
            flow_pocet_B = hodnota(pkt_data, len + 20, 4);
            flow_src_port = hodnota(pkt_data, len + 32, 2);
            flow_dst_port = hodnota(pkt_data, len + 34, 2);
            flow_protocol = hodnota(pkt_data, len + 38, 1);
            save_record(); //store results
            len += 48;
        }
    }
}

/*sFlow analysis is disabled, because it is not finished. There was a problem to find device, which support sFlow protocol. 
  Finally, we found one, but datagram format didn't match with sFlow specification.*/

/*void S5_analyza(const u_char *pkt_data, int len) {
        int sample_cnt, sample_enterprise, sample_format, sample_length, i, j, record_enterprise, record_format, record_length, record_cnt, pocetbajtov;
        int type = 0;
        int eth_type = 0;

        if (hodnota(pkt_data, len, 4) == 5) { //sFlow v.5 found
                if (hodnota(pkt_data, len + 4, 4) == 1) { //ip v4 header ==1, ip v6 header == 2
                        if (exporter_IP == hodnota(pkt_data, len + 8, 4)) {//chceck exporter IP
                                len += 24;
                                sample_cnt = hodnota(pkt_data, len, 4);
                                len += 4;
                                for (i = 0; i < sample_cnt; i++) {//for all samples
                                        sample_enterprise = hodnota(pkt_data, len, 2);
                                        if (hodnota(pkt_data, len + 2, 1) >= 16) {
                                                sample_enterprise += 1;
                                        }
                                        sample_format = hodnota(pkt_data, len + 2, 2);
                                        sample_length = hodnota(pkt_data, len + 4, 4);
                                        if ((sample_enterprise == 0) && ((sample_format == 1) || (sample_format == 3))) { //flow sample or extended flow sample
                                                if (sample_format == 1) {
                                                        len += 36;
                                                } else {
                                                        len += 48;
                                                }
                                                record_cnt = hodnota(pkt_data, len, 4);
                                                len += 4;
                                                flow_MAC_adr_S = 0;
                                                flow_MAC_adr_D = 0;
                                                flow_IP_adr_S = 0;
                                                flow_IP_adr_D = 0;
                                                flow_pocet_paketov = 0;
                                                flow_pocet_B = 0;
                                                flow_src_port = 0;
                                                flow_dst_port = 0;
                                                flow_protocol = 0;
                                                for (j = 0; j < record_cnt; j++) {//for all records
                                                        record_enterprise = hodnota(pkt_data, len, 2);
                                                        if (hodnota(pkt_data, len + 2, 1) >= 16) {
                                                                record_enterprise += 1;
                                                        }
                                                       
                                                        record_format = hodnota(pkt_data, len + 2, 2);
                                                        record_length == hodnota(pkt_data, len + 4, 4);
                                                        if ((record_enterprise == 0) && ((record_format == 1) || (record_format == 2) || (record_format == 3))) {//usable data
                                                                len += 8;
                                                                
                                                                switch (record_format) {
                                                                case 1:	//Raw packet header
                                                                        flow_pocet_B = hodnota(pkt_data, len + 4, 4);
                                                                        if (hodnota(pkt_data, len, 4) == 1) {
                                                                            if (record_length >= 28) {
                                                                                flow_MAC_adr_S = hodnota(pkt_data, len + 22, 6);
                                                                                flow_MAC_adr_D = hodnota(pkt_data, len + 16, 6);
                                                                            }
                                                                            if (record_length >= 30) {
                                                                                type = hodnota(pkt_data, len + 28, 2);

                                                                            }
                                                                            if(type == 33024){
                                                                                type = hodnota(pkt_data, len + 32, 2);
                                                                                if(type == 2048){//eth + ip
                                                                                if(record_length >=44)
                                                                                    flow_protocol = hodnota(pkt_data, len + 43, 1);
                                                                                if(record_length >=54){
                                                                                    flow_IP_adr_S = hodnota(pkt_data, len + 46, 4);
                                                                                    flow_IP_adr_D = hodnota(pkt_data, len + 50, 4);
                                                                                }
                                                                                if(record_length >=58){
                                                                                    if(flow_protocol == 6 || flow_protocol == 17){//tcp
                                                                                        flow_src_port = hodnota(pkt_data, len + 54, 2);
                                                                                        flow_dst_port = hodnota(pkt_data, len + 56, 2);
                                                                                    }
                                                                                }
                                                                                flow_protocol += 1500;
                                                                                }
                                                                                else if (type <= 1500){//802.3
                                                                                    //zatial nevedno co a ako
                                                                                }
                                                                            }
                                                                        }

                                                                        break;
                                                                case 2:	//Ethernet frame
                                                                        flow_MAC_adr_S = hodnota(pkt_data, len + 4, 6);
                                                                        flow_MAC_adr_D = hodnota(pkt_data, len + 12, 6);
                                                                        eth_type = hodnota(pkt_data, len + 20, 4);
                                                                        break;
                                                                case 3:	//IPv4
                                                                        flow_protocol = hodnota(pkt_data, len + 4, 4);
                                                                        flow_IP_adr_S = hodnota(pkt_data, len + 8, 4);
                                                                        flow_IP_adr_D = hodnota(pkt_data, len + 12, 4);
                                                                        flow_src_port = hodnota(pkt_data, len + 16, 4);
                                                                        flow_dst_port = hodnota(pkt_data, len + 20, 4);
                                                                        break;
                                                                }
                                                                len += record_length;
                                                        } else {
                                                                len = len + 8 + record_length;
                                                        }
                                                }
						
                                                flow_pocet_paketov = 1;//only one sample is stored
                                                save_record();//store results

                                        } else {
                                                len = len + 8 + sample_length;
                                        }
                                }
                        }
                }
        }
}*/

void template_update(time_t actual_time) { //this function will delete all old templates in memory
    int a = 0;

    if (n9_tmpl_first != NULL) {
        while ((n9_tmpl_first != NULL) && ((actual_time - n9_tmpl_first->add_time) > template_timeout)) {
            n9_tmpl_akt = n9_tmpl_first;
            n9_tmpl_first = n9_tmpl_first->next;
            free(n9_tmpl_akt);
        }
        if (n9_tmpl_first != NULL) {
            n9_tmpl_akt = n9_tmpl_first;
            while (n9_tmpl_akt->next != NULL) {
                while ((n9_tmpl_akt->next != NULL) && ((actual_time - n9_tmpl_akt->next->add_time) > template_timeout)) {
                    n9_tmpl = n9_tmpl_akt->next;
                    n9_tmpl_akt->next = n9_tmpl_akt->next->next;
                    free(n9_tmpl);
                }
                n9_tmpl_akt = n9_tmpl_akt->next;
            }
        }
    }
}

void flow_buffer_update(int cas) { //update database_time every minute when writing in database occur
    n9_buf_akt = n9_buf_first;
    while (n9_buf_akt != NULL) {
        if (n9_buf_akt->database_time == 0) { //were data captured in last minute?
            n9_buf_akt->database_time = cas;
        }
        n9_buf_akt = n9_buf_akt->next;
    }
}

void save_record() { //save all measured data into structure in memory for usually 1 minute
    char protokol[9];
    if (((flow_MAC_adr_S != 0 && flow_MAC_adr_D != 0) || (flow_IP_adr_S != 0 && flow_IP_adr_D != 0) || (flow_IPV6_adr_S != NULL && flow_IPV6_adr_D != NULL)) && (flow_pocet_B != 0 || flow_pocet_paketov != 0)) {//data chceck - able to store?
        flow_flag = 1;
        strcpy(protokol, "ALL"); //table for all measured data
        flow_protokoly(&z_protokoly, protokol);


        if (protocol_eth == 1 && flow_protocol >= 1500) // if we specified to watch ETHERNET II frames in config file and we catched 'E'(it means ETHERNET II frame), we adjust particular statistics
        {
            ///// ETHERNET

            strcpy(protokol, "E");
            flow_protokoly(&z_protokoly, protokol);

        }
        if (protocol_8023 == 1 && (flow_protocol >= 500 && flow_protocol < 1500)) {
            ////////// 802.3

            strcpy(protokol, "8");
            flow_protokoly(&z_protokoly, protokol);

        }

        /// ICMP protocol
        if (protocol_icmp == 1 && (flow_protocol == 1501 || flow_protocol == 501 || flow_protocol == 1)) {

            strcpy(protokol, "ICMP");
            flow_protokoly(&z_protokoly, protokol);
            if (protocol_ip == 1) {

                strcpy(protokol, "IP");
                flow_protokoly(&z_protokoly, protokol);

            }

        }
        /// ICMPv6 protocol
        if (protocol_icmp == 1 && flow_protocol == 58) {

            strcpy(protokol, "ICMPv6");
            flow_protokoly(&z_protokoly, protokol);
            if (protocol_ip == 1) {

                strcpy(protokol, "IPv6");
                flow_protokoly(&z_protokoly, protokol);

            }
        }


        /// TCP protocol
        if (protocol_tcp == 1 && (flow_protocol == 1506 || flow_protocol == 506 || flow_protocol == 6)) {

            strcpy(protokol, "TCP");
            flow_protokoly(&z_protokoly, protokol);
            if ((protocol_ip == 1) && (is_ipv6ext == 0)) {

                strcpy(protokol, "IP");
                flow_protokoly(&z_protokoly, protokol);

            }
            if ((protocol_ipv6 == 1) && (is_ipv6ext == 1)) {

                strcpy(protokol, "IPv6");
                flow_protokoly(&z_protokoly, protokol);

            }
        }

        /// UDP protocol
        if (protocol_udp == 1 && (flow_protocol == 1517 || flow_protocol == 517 || flow_protocol == 17)) {

            strcpy(protokol, "UDP");
            flow_protokoly(&z_protokoly, protokol);
            if ((protocol_ip == 1) && (is_ipv6ext == 0)) {

                strcpy(protokol, "IP");
                flow_protokoly(&z_protokoly, protokol);

            }
            if ((protocol_ipv6 == 1) && (is_ipv6ext == 1)) {

                strcpy(protokol, "IPv6");
                flow_protokoly(&z_protokoly, protokol);

            }
        }

        // TCP ports
        if (flow_src_port != 0 || flow_dst_port != 0) {

            int j;
            if (flow_protocol == 1506 || flow_protocol == 506 || flow_protocol == 6) {
                for (j = 0; j < i_TCP; j++) {
                    if (flow_src_port == pole_TCP_portov[j] || flow_dst_port == pole_TCP_portov[j]) {
                        if (flow_src_port == pole_TCP_portov[j])
                            sprintf(protokol, "TCP%d", flow_src_port);
                        if (flow_dst_port == pole_TCP_portov[j])
                            sprintf(protokol, "TCP%d", flow_dst_port);
                        flow_protokoly(&z_protokoly, protokol);
                    }
                }
            }

            // UDP ports
            if (flow_protocol == 1517 || flow_protocol == 517 || flow_protocol == 17) {
                for (j = 0; j < i_UDP; j++) {
                    if (flow_src_port == pole_UDP_portov[j] || flow_dst_port == pole_UDP_portov[j]) {
                        if (flow_src_port == pole_UDP_portov[j])
                            sprintf(protokol, "UDP%d", flow_src_port);
                        if (flow_dst_port == pole_UDP_portov[j])
                            sprintf(protokol, "UDP%d", flow_dst_port);
                        flow_protokoly(&z_protokoly, protokol);
                    }
                }
            }
        }
        //pairing IP and MAC addresses
        if (z_pair_array == NULL) {
            if ((z_pair_array = (PAIR_ARRAY*) malloc(sizeof (PAIR_ARRAY))) == NULL) {
                fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                return;
            }
            z_pair_array->mac_d = flow_MAC_adr_D;
            z_pair_array->mac_s = flow_MAC_adr_S;
            if (is_ipv6ext) {
                z_pair_array->is_ipv6 = is_ipv6ext;
                z_pair_array->ipv6_d = flow_IPV6_adr_D;
                z_pair_array->ipv6_s = flow_IPV6_adr_S;
            } else {
                z_pair_array->is_ipv6 = is_ipv6ext;
                z_pair_array->ip_d = flow_IP_adr_D;
                z_pair_array->ip_s = flow_IP_adr_S;
            }
            z_pair_array->p_next = NULL;
        } else {
            PAIR_ARRAY *pom_array;
            int find = 0;
            for (pom_array = z_pair_array; pom_array != NULL; pom_array = pom_array->p_next) {
                find = 0;
                if ((pom_array->mac_s == flow_MAC_adr_S) && (pom_array->mac_d == flow_MAC_adr_D)) {
                    if ((!is_ipv6ext && (pom_array->is_ipv6 == is_ipv6ext) && (pom_array->ip_s == flow_IP_adr_S) && (pom_array->ip_d == flow_IP_adr_D)) || (is_ipv6ext && (pom_array->is_ipv6 == is_ipv6ext) && (compare_IPv6(pom_array->ipv6_s, flow_IPV6_adr_S) && compare_IPv6(pom_array->ipv6_d, flow_IPV6_adr_D)))) {
                        find = 1;
                        break;
                    }
                }
            }
            if (!find) {
                for (pom_array = z_pair_array; pom_array->p_next != NULL; pom_array = pom_array->p_next) {
                }
                if ((pom_array->p_next = (PAIR_ARRAY*) malloc(sizeof (PAIR_ARRAY))) == NULL) {
                    fprintf(stderr, "Error malloc: %s\n", strerror(errno));
                    return;
                }
                pom_array = pom_array->p_next;
                pom_array->mac_d = flow_MAC_adr_D;
                pom_array->mac_s = flow_MAC_adr_S;
                if (is_ipv6ext) {
                    pom_array->is_ipv6 = is_ipv6ext;
                    pom_array->ipv6_d = flow_IPV6_adr_D;
                    pom_array->ipv6_s = flow_IPV6_adr_S;
                } else {
                    pom_array->is_ipv6 = is_ipv6ext;
                    pom_array->ip_d = flow_IP_adr_D;
                    pom_array->ip_s = flow_IP_adr_S;
                }
                pom_array->p_next = NULL;
            }
        }
    }
}
#endif
