
#include "ksocket.h"
#include <stdio.h>
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
#include <sys/time.h>

struct c_net_dev *get_interface(struct c_net_dev *interface){
 
  DIR *dp;
  struct dirent *dir;
  char help_dir[250];
  struct c_net_dev *pom_interface, *dev_interface = NULL;
  
  // open dir
  if(!(dp = opendir(DEVDIR))){
    fprintf(stderr,"get_interface error: %s\n", strerror(errno));
    return NULL;
  }
  
  while((dir = readdir(dp))){
    if(dir->d_ino == 0)
      continue;
    if(dir->d_name[0] == '.')
      continue;
    
    if(!interface){
      if((interface =(struct c_net_dev*) malloc(sizeof(struct c_net_dev))) == NULL){
	fprintf(stderr,"Error malloc get_interface: %s\n", strerror(errno));
	return NULL;
      }
      if((interface->name =(char*) malloc(strlen(dir->d_name)+1)) == NULL){
	fprintf(stderr,"Error malloc get_interface: %s\n", strerror(errno));
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
	fprintf(stderr,"Error malloc get_interface: %s\n", strerror(errno));
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
  
  closedir(dp);
  
  return interface;
}

// funkcia zisti aktualnu MAC adresu daneho rozhrania
uint64_t read_mac(const char *dir){
  int i, j;
  FILE *fr;
  char s_addr[MAXNUMBER], pom_addr[MAXNUMBER];
  
  if(!(fr = fopen(dir, "r"))){
    fprintf(stderr,"Error read mac: %s\n", strerror(errno));
    return 0;
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

int get_status (const char *file)
{
  char state[NUMSTATE];
  FILE *fr;
  char *f_help = NULL;
  
  if(!file){
    fprintf(stderr,"Get_status, error in file\n");
    return 0;
  }
  
  if(!(fr = fopen(file, "r"))){
    fprintf(stderr,"Get_status(): Error read operstate file: %s\n", strerror(errno));
    return 0;
  }
  
  fgets(state, NUMSTATE, fr);
  
  if(fclose(fr)){
    fprintf(stderr,"Get_status(): Error close file: %s\n", strerror(errno));
    exit(1);
  }
  
  // ak sa v danom retazci vyskytuje koniec riadku tak ho odstranime a nahradime koncom retezca
  // je to zlozitehsie ako keby sme pouzili strncmp(), ale istejsie    
  if((f_help = strchr(state, '\n')) != NULL)
    *f_help='\0';
  
  if(!(strcmp(state,"up")))
    return 1;
  
  return 0;
}


char *find_dev(char *dev_name){
  
  struct c_net_dev *dev_interface = NULL;
  struct c_net_dev *dev_help = NULL;
  char *file = NULL;
  
  dev_interface = get_interface(dev_interface);
  
  for(dev_help = dev_interface; dev_help != NULL; dev_help = dev_help->p_next){
    if((file = (char*) malloc((strlen(DEVDIR)+strlen(dev_help->name)+strlen(FILEOPERSTATE)+1)*sizeof(char))) == NULL){
      fprintf(stderr,"Find_dev(): Error malloc: %s\n", strerror(errno));
      return NULL;
    }
    if(sprintf(file,"%s%s/%s",DEVDIR, dev_help->name, FILEOPERSTATE) < 0){
      fprintf(stderr,"Find_dev(): Error sprintf: %s\n", strerror(errno));
      return NULL;
    }
    
    if(get_status(file)){
      free(file);
      file = NULL;
      dev_name = dev_help->name;
      return dev_name;
    }
    free(file);
    file = NULL;
  }
  return NULL;
}

int raw_init(struct k_capture *p_capture,char* device)
{
  struct ifreq ifr;
  struct packet_mreq mr;
  struct sockaddr_ll sll;
  int raw_socket;
  int ifindex;
  
  if(p_capture->name){
    free(p_capture->name);
    p_capture->name = NULL;
  }
  
  if(p_capture->file_status){
    free(p_capture->file_status);
    p_capture = NULL;
  }
  
  if(strcmp(device,"auto")){
    p_capture->interface_auto = 0;
    p_capture->file_status = NULL;
    if((p_capture->name=(char*) malloc(strlen(device)*sizeof(char) + 1)) == NULL){
      fprintf(stderr,"Raw_init(): Error malloc p_capture->name: %s\n", strerror(errno));
      return 1;
    }
    strcpy(p_capture->name, device);
  }
  else{
    char* file;
    if((file = find_dev(file)) == NULL)
      return 1;
     if((p_capture->name=(char*) malloc(strlen(file)*sizeof(char) + 1)) == NULL){
      fprintf(stderr,"Raw_init(): Error malloc: %s\n", strerror(errno));
      return 1;
    }
    p_capture->interface_auto = 1;
    strcpy(p_capture->name, file);
    if((p_capture->file_status=(char*) malloc(strlen(DEVDIR)*strlen(file)*strlen(FILEOPERSTATE)*sizeof(char) + 1)) == NULL){
      fprintf(stderr,"Raw_init(): Error malloc p_capture->filestatus: %s\n", strerror(errno));
      return 1;
    }
    if(sprintf(p_capture->file_status,"%s%s/%s",DEVDIR, file, FILEOPERSTATE) < 0){
      fprintf(stderr,"raw_init(): Error sprintf: %s\n", strerror(errno));
      return 0;
    }
  }
  
  memset(&ifr, 0, sizeof(struct ifreq));
  memset(&mr, 0, sizeof(struct packet_mreq));
  
  if((raw_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 1){
    fprintf(stderr, "ERROR: Could not open socket, Got ?\n");
    return 1;
  }
  
  strcpy(ifr.ifr_name, p_capture->name);
  
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
  
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = ifr.ifr_ifindex;
  sll.sll_protocol = ETH_P_ALL;
//   bind(raw_socket, (struct sockaddr_ll*)&sll, sizeof(sll));
  
  ifindex = ifr.ifr_ifindex;
  mr.mr_ifindex = ifindex;
  mr.mr_type = PACKET_MR_PROMISC;
  
  if(setsockopt(raw_socket, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) < 0){
    fprintf(stderr, "ERROR: Error setsockopt. %s\n", strerror(errno));
    return 1;
  }
  
  p_capture->socket = raw_socket;
  p_capture->sll = sll;
  
  return 0;

}

void k_loop(struct k_capture *p_capture, k_handler calback){
  
  struct k_header head;
  int len;
  
  u_char buf[BUFSIZ];
  
//   while(!connect(p_capture->socket,(struct sockaddr_ll*) &p_capture->sll, sizeof(p_capture->sll))){
  while(1){
    // if interface is down, find new interface and create new socket
    if(p_capture->interface_auto){
      if((p_capture->file_status == NULL) || (!get_status(p_capture->file_status))){
	if(p_capture->file_status){
	  free(p_capture->file_status);
	  p_capture->file_status = NULL;
	}
	if(p_capture->name){
	  free(p_capture->name);
	  p_capture->name = NULL;
	}
	if(p_capture->socket){
	  close(p_capture->socket);
	  p_capture->socket = 0;
	}
	if(raw_init(p_capture, "auto"))
	  continue;
      }
    }
//    fprintf(stderr,"Rozhranie: %s\n",p_capture->file_status);
    len = recvfrom(p_capture->socket,buf, sizeof(buf), 0, NULL, NULL);
    if(len < 0){
      fprintf(stderr, "Error recv packet. %s\n", strerror(errno));
      continue;
    }
    gettimeofday(&head.ts, NULL);
    head.len = len;
    
//     printf("Len: %d\n", len);
  
    calback(&head,(u_char *) &buf);
  }
  
}

