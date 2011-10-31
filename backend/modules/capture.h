#ifndef KAT_CAPTURE
#define KAT_CAPTURE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define DEVDIR "/sys/class/net/"
#define MAXNUMBER 30

struct c_net_dev{
 char *name;		// meno sietoveho rozhranie
 uint64_t address;	// mac adresa
 struct c_net_dev *p_next;
};

struct k_capture{
  int socket;
  char *name;
};

struct k_header{
  time_t time;
};

typedef void (*k_handler)(const struct k_header *h, const u_char *bytes);

extern struct packet_mreq mr;

/* funkcia zisti existujuce sietove rozhrania v pocitaci 
 *a naplni nimi strukturu c_net_dev*/
struct c_net_dev *get_interface(struct c_net_dev *interface);
uint64_t read_mac(const char *dir);
int raw_init(struct k_capture *p_capture,char *device);
void k_loop(struct k_capture *p_capture, k_handler callback);

#endif
