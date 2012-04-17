
#include "ksocket.h"
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#ifdef _DEBUG_SOCKET
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

// Not use
/*
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
*/

/*
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
*/

int get_status (struct k_capture *dev)
{

  struct ifreq ifr;
  
  if(!dev->name)
    return 0;
  
  memset(&ifr, 0, sizeof(struct ifreq));
 
  strcpy(ifr.ifr_name, dev->name);
  
  if(ioctl(dev->socket, SIOCGIFFLAGS, &ifr) == -1){
    fprintf(stderr, "ERROR get_status(): Could not retrieve the flags from the device. %s\n", strerror(errno));
    exit(1);
  }
  
  if((ifr.ifr_flags & IFF_UP) && (ifr.ifr_flags & IFF_RUNNING))
    return 1;
  else
    return 0;
  
  
/* Old solution */
/*
  char state[NUMSTATE];
  FILE *fr;
  char *f_help = NULL;
  
  name = dev->name;
  
  if(!name){
    fprintf(stderr,"Get_status, error in file\n");
    return 0;
  }
  
  if(!(fr = fopen(name, "r"))){
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
*/
}

char *find_dev(char *dev_name){
  
  /*  
   *  struct c_net_dev *dev_interface = NULL;
   *  struct c_net_dev *dev_help = NULL;
   */  
  char *file = dev_name;
  // struct for interface
  struct ifaddrs *ifap;
  struct ifaddrs *ifa;
  
  if(getifaddrs(&ifap) == -1){
    fprintf(stderr, "Error getifaddrs in raw_init(): %s", strerror(errno));
    exit(1);
  }
  
  for(ifa = ifap;ifa != NULL; ifa = ifa->ifa_next){
    fprintf(stderr, "Interface: %s, Flag: %d\n", ifa->ifa_name, ifa->ifa_flags);
    
    if((ifa->ifa_flags & IFF_UP) && ( ifa->ifa_flags & IFF_RUNNING) && !(ifa->ifa_flags & IFF_LOOPBACK)){
      if((file = (char*) malloc((strlen(ifa->ifa_name)+1)*sizeof(char))) == NULL){
	fprintf(stderr,"Find_dev(): Error malloc: %s\n", strerror(errno));
	return NULL;
      }
      strcpy(file, ifa->ifa_name);
      freeifaddrs(ifap);
      return file;
    }
  }
  freeifaddrs(ifap);
  
  //old solution
  /*  
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
*/
  
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
  
/*  
  if(p_capture->file_status){
    free(p_capture->file_status);
    p_capture = NULL;
  }
*/

  if(strcmp(device,"auto")){
    p_capture->interface_auto = 0;
    /*
    p_capture->file_status = NULL;
    */
    if((p_capture->name=(char*) malloc(strlen(device)*sizeof(char) + 1)) == NULL){
      fprintf(stderr,"Raw_init(): Error malloc p_capture->name: %s\n", strerror(errno));
      return 1;
    }
    strcpy(p_capture->name, device);
  }
  else{
    char* file=NULL;    
    if((file = find_dev(file)) == NULL)
      return 1;
    if((p_capture->name=(char*) malloc(strlen(file)*sizeof(char) + 1)) == NULL){
      fprintf(stderr,"Raw_init(): Error malloc: %s\n", strerror(errno));
      return 1;
    }
    strcpy(p_capture->name, file);
    p_capture->interface_auto = 1;
    
/*    
    if((p_capture->file_status=(char*) malloc(strlen(DEVDIR)*strlen(file)*strlen(FILEOPERSTATE)*sizeof(char) + 1)) == NULL){
      fprintf(stderr,"Raw_init(): Error malloc p_capture->filestatus: %s\n", strerror(errno));
      return 1;
    }
    if(sprintf(p_capture->file_status,"%s%s/%s",DEVDIR, file, FILEOPERSTATE) < 0){
      fprintf(stderr,"raw_init(): Error sprintf: %s\n", strerror(errno));
      return 0;
    }
*/    
    if(!file){
      free(file);
      file=NULL;
    }
  }
  
  memset(&ifr, 0, sizeof(struct ifreq));
  memset(&mr, 0, sizeof(struct packet_mreq));
  
  if((raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 1){
    fprintf(stderr, "ERROR: Could not open socket, Got ?\n");
    return 1;
  }
  
  strcpy(ifr.ifr_name, p_capture->name);
  
#ifdef _DEBUG_SOCKET
  fprintf(stderr,"Interface: %s\n",ifr.ifr_name);
#endif
  
  if(ioctl(raw_socket, SIOCGIFFLAGS, &ifr) == -1){
    fprintf(stderr, "ERROR: Could not retrieve the flags from the device. %s\n", strerror(errno));
    return 1;
  }
  
// save old interface state  
  p_capture->old_status = ifr;
  
  ifr.ifr_flags |= IFF_PROMISC;
  
  if(ioctl(raw_socket, SIOCSIFFLAGS, &ifr) == -1){
    fprintf(stderr, "ERROR: Could not set flags IFF_PROMISC. %s\n", strerror(errno));
    return 1;
  }
  
 fprintf(stderr,"Flag: %d\n",ifr.ifr_flags);
  
  if(ioctl(raw_socket, SIOCGIFINDEX, &ifr) < 0){
    fprintf(stderr, "ERROR: Error getting the device index.\n");
    return 1;
  }
  
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = ifr.ifr_ifindex;
  sll.sll_protocol = htons(ETH_P_ALL);
  bind(raw_socket, (struct sockaddr_ll*)&sll, sizeof(sll));
  
  ifindex = ifr.ifr_ifindex;
  mr.mr_ifindex = ifindex;
  mr.mr_type = PACKET_MR_PROMISC;
    
  if(setsockopt(raw_socket, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) < 0){
    fprintf(stderr, "ERROR: Error setsockopt. %s\n", strerror(errno));
    return 1;
  }
  
/*  Set timeout
 * We set how long, we will wait for data
*/
  struct timeval tv;
  
  tv.tv_sec = 10;  /* 10 Secs Timeout */
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  
  if(setsockopt(raw_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval))){
    fprintf(stderr, "ERROR: Error setsockopt for time: %s\n", strerror(errno));
    return 1;
  };

  p_capture->socket = raw_socket;
  p_capture->sll = sll;
  
  return 0;

}

void k_loop(struct k_capture *p_capture, k_handler calback){
  
  struct k_header head;
  struct dev_time *dt;
  int len;
  u_char buf[BUFSIZ];
  char *old_interface;
  
  if((old_interface=(char*) malloc(sizeof(char)*strlen("NOT"))) == NULL){
    fprintf(stderr, "k_loop: Error malloc old_interface: %s", strerror(errno));
    exit(1);
  }
  
  strcpy(old_interface,"NOT");
  
  head.dt = NULL;
  
//   while(!connect(p_capture->socket,(struct sockaddr_ll*) &p_capture->sll, sizeof(p_capture->sll))){
  while(1){
    dt=NULL;
    
    if(p_capture->name != NULL){
      if(old_interface != NULL){
	free(old_interface);
	old_interface=NULL;
      }
      
      if((old_interface = (char*) malloc(strlen(p_capture->name)*sizeof(char))) == NULL){
	fprintf(stderr, "k_loop(): Error malloc old_interface: %s", strerror(errno));
	exit(1);
      }
      strcpy(old_interface,p_capture->name);
    }
    
    // if interface is down, find new interface and create new socket
    if(p_capture->interface_auto){
      /*
       if((p_capture->file_status == NULL) || (!get_status(p_capture->file_status))){
          if(*p_capture->file_status){
       free(p_capture->file_status);
       p_capture->file_status = NULL;
       }
       */
      if(!get_status(p_capture)){
	if(p_capture->name){
	  free(p_capture->name);
	  p_capture->name = NULL;
	}
	//return interface to old state
	if(ioctl(p_capture->socket,SIOCSIFFLAGS, &p_capture->old_status) == -1){
	  fprintf(stderr, "Error, set old state in k_loop(): %s\n",strerror(errno));
	}
	if(p_capture->socket){
	  close(p_capture->socket);
	  p_capture->socket = 0;
	}
	if(raw_init(p_capture, "auto")){
	  sleep(2);
	  continue;
	}
	if((dt = (struct dev_time *) malloc(sizeof(struct dev_time))) == NULL){
	  fprintf(stderr, "k_loop(): Error malloc struct dev_time: %s", strerror(errno));
	  exit(1);
	}
	gettimeofday(&dt->ts, NULL);
	if((dt->name_do = (char*) malloc(strlen(p_capture->name)*sizeof(char))) == NULL){
	  fprintf(stderr,"k_loop(): Error malloc dt->name: %s", strerror(errno));
	  exit(1);
	}
	strcpy(dt->name_do,p_capture->name);
	
	if((dt->name_z = (char*) malloc(strlen(old_interface)*sizeof(char))) == NULL){
	  fprintf(stderr,"k_loop(): Error malloc old_interface: %s", strerror(errno));
	  exit(1);
	}
	strcpy(dt->name_z,old_interface);
      }
       }
//    fprintf(stderr,"Rozhranie: %s\n",p_capture->file_status);
    len = recvfrom(p_capture->socket,buf, sizeof(buf), 0, NULL, NULL);
    if(len <= 0){
      fprintf(stderr, "Warring recv packet. %s\n", strerror(errno));
      continue;
    }
    gettimeofday(&head.ts, NULL);
    head.len = len;
    head.interface_auto = p_capture->interface_auto;
    
    if(p_capture->interface_auto && (head.dt == NULL))
      head.dt=dt;
    
#ifdef _DEBUG_SOCKET
      struct ether_header *ethh_d = NULL; //ethernetova header
      struct iphdr *iph_d = NULL;       //ip header
      struct tcphdr *tcph_d = NULL;     //tcp header
      struct udphdr *udph_d = NULL;     //udp header
      struct in_addr in_s, in_d;
      unsigned const char *pom_buf;
      unsigned short int type;
      
      pom_buf = (unsigned char*) &buf;
      ethh_d = (struct ether_header*) pom_buf;
      type = ntohs(ethh_d->ether_type);
      fprintf(stderr,"Type: %d, %X\n", type, type);
      if(type == ETHERTYPE_IP){
	iph_d = (struct iphdr*) (pom_buf + sizeof(struct ether_header));
	if(iph_d->protocol == IPPROTO_TCP){
	  tcph_d = (struct tcphdr*) (pom_buf + sizeof(struct ether_header) + iph_d->ihl*4);
	  in_s.s_addr = iph_d->saddr;
	  in_d.s_addr = iph_d->daddr;
	  fprintf(stderr, "IP_S: %s, IP_D: %s, Port_S: %d, Port_D: %d\n", inet_ntoa(in_s), inet_ntoa(in_d), ntohs(tcph_d->source), ntohs(tcph_d->dest));
	}
	if(iph_d->protocol == IPPROTO_UDP){
	  udph_d = (struct udphdr*) (pom_buf + sizeof(struct ether_header) + iph_d->ihl*4);
	  in_s.s_addr = iph_d->saddr;
	  in_d.s_addr = iph_d->daddr;
	  fprintf(stderr, "IP_S: %s, IP_D: %s, Port_S: %d, Port_D: %d\n", inet_ntoa(in_s), inet_ntoa(in_d), ntohs(udph_d->source), ntohs(udph_d->dest));
	}
      }
#endif
//     printf("Len: %d\n", len);
  
    calback(&head,(u_char *) &buf);
    
    if(head.dt !=NULL){
      if(head.dt->name_z != NULL){
	free(head.dt->name_z);
	head.dt->name_z = NULL;
      }
      if(head.dt->name_do){
	free(head.dt->name_do);
	head.dt->name_do = NULL;
      }
    }
    free(head.dt);
    head.dt = NULL;
  }
  
}


