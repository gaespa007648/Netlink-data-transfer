#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/errno.h>

#define NETLINK_TEST     31
#define MSG_LEN             300000
#define MAX_TRANSFER_MSG_LEN 1500

struct sock *nlsk = NULL;
extern struct net init_net;
unsigned long kmsg[MSG_LEN];