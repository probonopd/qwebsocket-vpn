#ifndef COMMON_H
#define COMMON_H
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#ifdef __cplusplus
extern "C" {
#endif
struct UdpMsg {
    struct sockaddr_in6 r_addr;
    socklen_t r_addrlen;
    char payload[0];
};


#define ADDR_INFO_LEN (sizeof(struct UdpMsg))
#define MAX_MSG_LEN 4096
#define VPN_IF "ws_vpn"

int if_up(const char *if_name);

int create_share_rcv_udp_socket(unsigned short port);

int create_snd_udp_socket();

int create_snd_ip_raw_socket(const int family);

int create_tun_interface(void);

/*implemented according with RFC 1071 and1141*/
/*
static inline unsigned short csum_incremental_update(unsigned short old_csum, unsigned short old_field, unsigned short new_field)
{
   unsigned long csum = old_csum + old_field + (~new_field & 0xFFFF);
   csum = (csum >> 16) + (csum & 0xFFFF);
   csum +=  (csum >> 16);
   return csum;
}
*/
/*incremental update IP ,TCP, UDP checksum,
implemented in Assembly according with RFC1624, used subtraction to update checksum*/
static inline unsigned short csum_incremental_update(unsigned short old_csum,
                                unsigned short old_field,
                                unsigned short new_field)
{
   __asm__ __volatile__(
       "notw %1; \n"
       "subw %1, %0; \n"
       "sbbw %2, %0; \n"
       "sbbw $0, %0; \n"
       :"=r" (old_csum)
       :"r"(old_field), "r"(new_field),"0"(old_csum));
   return old_csum;
}

/* get_local_ip_from_route() given a remote ip , ask kernel which local address will be chosen to be used for communication
 * @af, AF_INET or AF_INET6
 * @ipstr, remote ip, string format
 * @ip, remote ip, network byte order format
 * @srcstr, local ip, string format
 * @src, local ip, network byte order format
 */
//int get_local_ip_from_route(int af, const char *ipstr, const void *ip, char *srcstr, void *src);
#ifdef __cplusplus
}
#endif













#endif // COMMON_H
