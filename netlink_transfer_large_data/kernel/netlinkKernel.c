#include "include/netlinkKernel.h"

int send_usrmsg(unsigned long *send_msg_buf, uint16_t msg_len, unsigned int return_pid)
{
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;

    /* Create sk_buff space */
    nl_skb = nlmsg_new(msg_len, GFP_ATOMIC);
    if(!nl_skb)
    {
        return -ENOMEM;  
    }

    nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_TEST, msg_len, NLM_F_MULTI);
    
    if(nlh == NULL)
    {
        nlmsg_free(nl_skb);
        return -EMSGSIZE;
    }
    memcpy(nlmsg_data(nlh), send_msg_buf, msg_len);  /* nlmsg_data(nlh) returns the pointer to start of message payload */

    return netlink_unicast(nlsk, nl_skb, return_pid, MSG_DONTWAIT);
}

static void netlink_rcv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    unsigned int nlh_pid = 0;
    int *user_request_index = NULL;

    if(skb->len >= nlmsg_total_size(0))
    {
        nlh = nlmsg_hdr(skb);
        nlh_pid = nlh->nlmsg_pid;
        user_request_index = (int *)NLMSG_DATA(nlh);

        int ret = 0;
        if(user_request_index)
        {   
            printk("kernel recv from user: %i\n", *user_request_index);
            int send_msg_len = (int)MSG_LEN-(*user_request_index);

            if(send_msg_len > MAX_TRANSFER_MSG_LEN){
               ret = send_usrmsg(&(kmsg[*user_request_index]), MAX_TRANSFER_MSG_LEN*sizeof(unsigned long), nlh_pid);
            }else{
               ret = send_usrmsg(&(kmsg[*user_request_index]), send_msg_len*sizeof(unsigned long), nlh_pid);
            }
            
            printk("ret = %i\n", ret);
        }
    }
}

struct netlink_kernel_cfg cfg = {
        .input  = netlink_rcv_msg, /* set recv callback */
};

int test_netlink_init(void)
{
    /* create netlink socket */
    nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if(nlsk == NULL)
    {
        printk("netlink_kernel_create error !\n");
        return -1;
    }
    printk("test_netlink_init\n");

    /* set the transfer data */
    unsigned long i=0;
    for(i;i<MSG_LEN;i++){
        kmsg[i]=i;
    }

    return 0;
}

void test_netlink_exit(void)
{
    if (nlsk){
        netlink_kernel_release(nlsk); /* release ..*/
        nlsk = NULL;
    }
    printk("test_netlink_exit!\n");

}

module_init(test_netlink_init);
module_exit(test_netlink_exit);
