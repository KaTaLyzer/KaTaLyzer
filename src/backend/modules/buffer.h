#ifndef _BUFFER
#define _BUFFER


#define AUTHOR	"Andrej Kozemcak <akozemcak@gmail.com>"
#define DESC	"Capture networ packet"

#include <net/netlink.h>
#include <net/genetlink.h>

enum{
  KATALYZER_NONE,
  KATALYZER_DATA,
};

static struct nla_policy katalyzer_policy[KATALYZER_DATA+1] = {
  [KATALYZER_DATA] = { .type = NLA_NUL_STRING},
};

static struct genl_family katalyzer_family = {
  .id = GENL_ID_GENERATE,
  .hdrsize = 0,
  .name = "KATALYZER",
  .version = 1,
  .maxattr = KATALYZER_DATA,
};



// void get_data();

#endif