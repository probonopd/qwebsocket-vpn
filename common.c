#include "common.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#endif

int if_up(const char *if_name)
{
    int sockfd, ret =-1;
    struct ifreq ifr;

    if(!if_name) return ret;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        return ret;
    memset(&ifr, 0, sizeof ifr);
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ);
    ifr.ifr_flags |= IFF_UP;
    ret = ioctl(sockfd, SIOCSIFFLAGS, &ifr);
    close(sockfd);
    return ret;
}

int create_tun_interface(void)
{
    struct ifreq ifr;
    int err, fd = -1;
    if((fd= open("/dev/net/tun", O_RDWR)) < 0){
        return fd;
    }
    memset(&ifr, 0, sizeof(ifr));
    sprintf(ifr.ifr_name, "ws_vpn_%d", fd);
    ifr.ifr_flags = (IFF_TUN | IFF_NO_PI|IFF_UP);
    err = ioctl(fd, TUNSETIFF, &ifr);
    if(err){
        perror("ioctl:");
        close(fd);
        fd = -1;
        return fd;
    }
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd,F_SETFL,flags | O_NONBLOCK);
    return fd;
}
