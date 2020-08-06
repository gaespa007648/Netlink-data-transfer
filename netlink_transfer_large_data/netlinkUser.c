#include "include/netlinkUser.h"
#define CALCULATE_TRANSFER_TIME

int main(int argc, char **argv)
{
    if(initialize_netlink_setting()==0){
        printf("Successfully initialize netlink setting\n");
    }else{
        printf("Fail in initializing netlink setting\n");
        exit(-1);
    }

#ifdef CALCULATE_TRANSFER_TIME
    clock_t t_start, t_end;
    t_start = clock();
#endif

    unsigned long received_msg[RECEIVE_MESSAGE_LENGTH];
    memset(received_msg, (unsigned long)0 ,RECEIVE_MESSAGE_LENGTH);

    int request_times = (int)ceil((double)RECEIVE_MESSAGE_LENGTH/(double)MSG_LEN);
    printf("requst_times = %i\n",request_times);

    /* For the data which size is larger than sk_buff tolerance size(16KB), 
    we should slicing the transfer data and receive multiple times */
    for( int times=0; times<request_times ; times++ ){

        user_request_index = times*MSG_LEN;
        memcpy(NLMSG_DATA(nlh), &user_request_index, sizeof(user_request_index));
        printf("send kernel : %i\n", user_request_index); 
        ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&daddr, sizeof(struct sockaddr_nl));
        if(!ret)
        {
            perror("sendto error\n");
            close(skfd);
            exit(-1);
        }
        
        memset(&u_info, 0, sizeof(u_info));
        len = sizeof(struct sockaddr_nl);

        ret = recvfrom(skfd, &u_info, sizeof(u_info),0, (struct sockaddr *)&daddr, &len);
        if(ret<0)
        {
            perror("recv form kernel error");
        }

        int data_received = 0;
        if((int)RECEIVE_MESSAGE_LENGTH-user_request_index>MSG_LEN){
            data_received = MSG_LEN;
        }else{
            data_received = (int)RECEIVE_MESSAGE_LENGTH-user_request_index;
        }

        for( int i=0 ; i<data_received ; i++ ){
            received_msg[user_request_index+i] = u_info.msg[i];
        }

    }

    for(int j=0;j<RECEIVE_MESSAGE_LENGTH;j++){
        printf("received_msg[%i] = %ld\n", j, received_msg[j]);
    }
    printf("End of receiving\n");

#ifdef CALCULATE_TRANSFER_TIME
    t_end = clock();
    printf("Data transfer time = %f s\n", (double)(t_end-t_start)/CLOCKS_PER_SEC);
#endif

    close(skfd);
    free((void *)nlh);

    return 0;
}


int initialize_netlink_setting(){
    /* Create NETLINK socket */
    skfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if(skfd == -1)
    {
        perror("create socket error\n");
        return -1;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.nl_family = AF_NETLINK; //AF_NETLINK
    saddr.nl_pid = getpid() ;  // port ID
    saddr.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        perror("bind() error\n");
        close(skfd);
        return -1;
    }

    memset(&daddr, 0, sizeof(daddr));
    daddr.nl_family = AF_NETLINK;
    daddr.nl_pid = 0; // to kernel
    daddr.nl_groups = 0;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    /* NLMSG_SPACE returns the number of bytes that a netlink message with payload of len would occupy. */
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD); 
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = saddr.nl_pid; //self port

    return 0;
}
