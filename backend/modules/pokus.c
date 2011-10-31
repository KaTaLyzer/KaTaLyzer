
#include <stdio.h>
#include "capture.h"

void pokus(const struct k_header *h, const u_char *bytes);

int main(void){
  
  struct c_net_dev *dev, *pom_dev;
  struct k_capture c;
  
  dev = get_interface(dev);
  
  for(;dev != NULL;dev = dev->p_next){
    printf("%s: %lu\n", dev->name, dev->address);
  }
  
  raw_init(&c, "wlan0");
  
  k_loop(&c, pokus);
  
  
  return 0;
}

void pokus(const struct k_header* h, const u_char* bytes)
{
  FILE *fw;
  
  fw = fopen("Pokus.txt", "a+");
  
  fprintf(fw, "%X\n", bytes);
  
  fclose(fw);

}
