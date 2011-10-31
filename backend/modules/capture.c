
#include "capture.h"
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <unistd.h>
#include <time.h>

struct packet_mreq mr;

struct c_net_dev *get_interface(struct c_net_dev *interface){
 
  DIR *dp;
  struct dirent *dir;
  char help_dir[250];
  struct c_net_dev *pom_interface, *dev_interface = NULL;
  
  if(!(dp = opendir(DEVDIR))){
    fprintf(stderr,"get_interface error: %s", strerror(errno));
    return NULL;
  }
  
  while((dir = readdir(dp))){
    if(dir->d_ino == 0)
      continue;
    if(dir->d_name[0] == '.')
      continue;
    
    if(!interface){
      if((interface =(struct c_net_dev*) malloc(sizeof(struct c_net_dev))) == NULL){
	fprintf(stderr,"Error malloc get_interface: %s", strerror(errno));
	return NULL;
      }
      if((interface->name =(char*) malloc(strlen(dir->d_name)+1)) == NULL){
	fprintf(stderr,"Error malloc get_interface: %s", strerror(errno));
	return NULL;
      }
      strcpy(interface->name,dir->d_name);
      sprintf(help_dir,"%s%s/address",DEVDIR, interface->name);
      interface->address = read_mac(help_dir);
      interface->p_next = NULL;
    }
    else{
      for(pom_interface = interface->p_next, dev_interface = interface;dev_interface->p_next != NULL;  dev_interface = dev_interface->p_next){
      }
      if((dev_interface->p_next = (struct c_net_dev*) malloc(sizeof(struct c_net_dev))) == NULL){
	fprintf(stderr,"Error malloc get_interface: %s", strerror(errno));
	return NULL;
      }
      pom_interface = dev_interface->p_next;
      pom_interface->name =(char*) malloc(strlen(dir->d_name)+1);
      strcpy(pom_interface->name,dir->d_name);
      sprintf(help_dir,"%s%s/address",DEVDIR, pom_interface->name);
      pom_interface->address = read_mac(help_dir);
      pom_interface->p_next = NULL;
    }
  }
  
  return interface;
}

uint64_t read_mac(const char *dir){
  int i, j;
  FILE *fr;
  char s_addr[MAXNUMBER], pom_addr[MAXNUMBER];
  
  if(!(fr = fopen(dir, "r"))){
    fprintf(stderr,"Error read mac: %s", strerror(errno));
    return NULL;
  }
  
  fgets(pom_addr,MAXNUMBER, fr);
  
  fclose(fr);
  
  for(i = 0, j = 0; i < MAXNUMBER && pom_addr[i] != '\0'; i++){
    if(pom_addr[i] != ':'){
      s_addr[j] = pom_addr[i];
      j++;
    }
  }
  
  return strtoll(s_addr,NULL,16);
}

int raw_init(struct k_capture *p_capture,char* device)
{
  struct ifreq ifr;
  int raw_socket;
  int ifindex;
  
  memset(&ifr, 0, sizeof(struct ifreq));
  memset(&mr, 0, sizeof(struct packet_mreq));
  
  p_capture->name = (char*) malloc(strlen(device) + 1);
  strcpy(p_capture->name, device);
  
  if((raw_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 1){
    fprintf(stderr, "ERROR: Could not open socket, Got ?\n");
    return 1;
  }
  
  strcpy(ifr.ifr_name, device);
  
  if(ioctl(raw_socket, SIOCGIFFLAGS, &ifr) == -1){
    fprintf(stderr, "ERROR: Could not retrive the flags from the divice. %s\n", strerror(errno));
    return 1;
  }
  
  ifr.ifr_flags |= IFF_PROMISC;
  
  if(ioctl(raw_socket, SIOCSIFFLAGS, &ifr) == -1){
    fprintf(stderr, "ERROR: Could not set flags IFF_PROMISC. %s\n", strerror(errno));
    return 1;
  }
  
  if(ioctl(raw_socket, SIOCGIFINDEX, &ifr) < 0){
    fprintf(stderr, "ERROR: Error gettinf the device index.\n");
    return 1;
  }
  
  ifindex = ifr.ifr_ifindex;
  mr.mr_ifindex = ifindex;
  mr.mr_type = PACKET_MR_PROMISC;
  
  if(setsockopt(raw_socket, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) < 0){
    fprintf(stderr, "ERROR: Error setsockopt. %s\n", strerror(errno));
    return 1;
  }
  
  p_capture->socket = raw_socket;
  
  return 0;

}

void k_loop(struct k_capture *p_capture, k_handler calback){
  
  struct k_header head;
  
  u_char buf[2050];
  
  while(1){
    read(p_capture->socket,buf, sizeof(buf));
    head.time = time(NULL);
    
    calback(&head,(u_char *) &buf);
  }
  
}

