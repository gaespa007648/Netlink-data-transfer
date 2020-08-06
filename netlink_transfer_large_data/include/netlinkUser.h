#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <time.h>

#define NETLINK_TEST            31
#define MSG_LEN                                             1500       // 控制每次接收訊息的最長長度
#define RECEIVE_MESSAGE_LENGTH      300000
#define MAX_PLOAD                                        100      // MAX_PLOAD用來控制傳至核心的訊息長度

typedef struct _user_msg_info
{
    struct nlmsghdr hdr;
    unsigned long msg[MSG_LEN];
} user_msg_info;

int skfd;
int ret;
user_msg_info u_info;
socklen_t len;
struct nlmsghdr *nlh = NULL;
struct sockaddr_nl saddr, daddr;
int user_request_index = 0;
int initialize_netlink_setting();
