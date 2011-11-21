
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>

#include "buffer.h"

#define K_BUFFER "/sys/class/net/kbuff"

int k_buffer_write(void *data){
  int res = 0;
  FILE *fw = fopen(K_BUFFER, "w");
  if(f == NULL)
    return -errno;
  if((res = fprintf(fw, "%s", data)) <=0)
    res = res == 0 ? -EIO : -errno;
  
  fclose(f);
  return res;
}

static unsigned int net_buff(unsigned int hooknum,
  struct sk_buff *skb,
  const struct net_device *in,
  const struct net_device *out,
  int (*okfn)(struct sk_buff *)){
  
  printk(KERN_INFO "Capture data?\n");
  
  
  
//   k_buffer_write(skb->data);
  
  return NF_ACCEPT;
} 

static struct nf_hook_ops nf_buff_hook_ops = {
  .hook		= net_buff,
  .owner	= THIS_MODULE,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
  .hooknum	= NF_INET_PRE_ROUTING,
#else
  .hooknum	= NF_IP_PRE_ROUTING,
#endif
  .pf		= PF_INET,
  .priority	= NF_IP_PRI_FIRST
 
};

static int __init buffer_init(void){
//   get_data();

  printk(KERN_INFO "Hello world: buffer\n");
  
  int rvl;
  int rc;
  
//   memset(&nf_buff_hook_ops, 0x00, 
//     sizeof(struct nf_hook_ops));
 
  if ((rvl = nf_register_hook(&nf_buff_hook_ops)) != 0){
    printk(KERN_ERR "nf_register_hook() failed");
    return rvl;
  }
  
  if((rc = genl_register_family(&katalyzer_family)) != 0){
    printk(KERN_ERR "genl_register_family() failed")
    return rc;
  }
//   struct sk_buff_head *p_head;
//   struct sk_buff *skb;
  
//   skb = alloc_skb(100, GFP_KERNEL);
  
//   skb_queue_head_init(p_head);
//   skb_network_header(skb);
//   skb_queue_empty(p_head);
  
//   printk(KERN_INFO "Empty %d\n",skb_queue_empty(p_head));
  return 0;
}

static void __exit buffer_exit(void){
  nf_unregister_hook(&nf_buff_hook_ops);
  printk(KERN_INFO "Exit, buffer \n");
}

/*
void get_data()
{

  struct sk_buff_head *p_head;
  
  skb_queue_head_ini(p_head);
  
  printk(KERN_INFO "Len %d\n",p_head->qlen);
  
}
*/

module_init(buffer_init);
module_exit(buffer_exit);

MODULE_LICENSE("GPL");

MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESC);