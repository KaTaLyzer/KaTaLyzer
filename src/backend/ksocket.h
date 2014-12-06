#ifndef KAT_CAPTURE
#define KAT_CAPTURE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <linux/if_packet.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <net/if.h>

#define DEVDIR "/sys/class/net/"
#define MAXNUMBER 30
#define NUMSTATE 5
#define FILEOPERSTATE "operstate"

// do struktury ulozime cas zmeny rozhrania
// tu bola struct dev_time

struct dev_time{
  struct timeval ts;
  char *name_z;
  char *name_do;
};

struct c_net_dev{
  uint64_t address;	// mac address
  char *name;		// interface name
  struct c_net_dev *p_next;
};

struct k_capture{
  unsigned short int interface_auto;	// auto take interface - true/false 
  int socket;
  struct sockaddr_ll sll;
  char *name;
  struct ifreq old_status;	//save old status, of interface
//  char *file_status;	// dir to file operstate, where is status of the interface
};

struct k_header{
  struct timeval ts;
  unsigned int len;
  unsigned short int interface_auto;
  struct dev_time *dt;
};

typedef void (*k_handler)(const struct k_header *header, const u_char *pkt_data);

/* find network interface in the system
 * save it in to structure c_net_dev 
 * if interface don`t allock, must by define as NULL */
//struct c_net_dev *get_interface(struct c_net_dev *interface);


//uint64_t read_mac(const char *dir);

/* create socket
 * return 0 on success
 * return 1 on false */
int raw_init(struct k_capture *p_capture,char *device);

void k_loop(struct k_capture *p_capture, k_handler callback);

/* find running network interface */
char *find_dev(char *dev_name);

/* get status of interface, is up or down 
 * is up return 1
 * is down return 0 */
int get_status(struct k_capture *dev);

/* Close promiscuite mode
 * return 0 on success
 * return 1 on false
 */
int close_promisc(int raw_socket ,char *device);

#endif
