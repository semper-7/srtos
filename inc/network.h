#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "stm32f1xx.h"

/* ---------- Ethernet driver setup --------- */
#include "enc28j60.h"
#define PacketSend ENC28J60_PacketSend
#define PacketReceive ENC28J60_PacketReceive
#define LinkUp ENC28J60_LinkUp
/* ------------------------------------------ */

#define BUFFER_SIZE    400
#define DNS_PORT       53
#define DNS_SRC_PORT_H 0xE0
#define DNS_TYPE_A     1
#define DNS_CLASS_IN   1
#define CLIENTMSS      550

#define ETH_HEADER_LEN 0x0E
#define ETH_DST_MAC    0x00
#define ETH_SRC_MAC    0x06
#define ETH_TYPE       0x0C
#define ETH_ARP_H      0x08
#define ETH_ARP_L      0x06
#define ETH_IP_H       0x08
#define ETH_IP_L       0x00

#define ARP_OPCODE     0x14
#define ARP_SRC_MAC    0x16
#define ARP_SRC_IP     0x1C
#define ARP_DST_MAC    0x20
#define ARP_DST_IP     0x26
#define ARP_REQUEST_H  0x00
#define ARP_REQUEST_L  0x01
#define ARP_REPLY_H    0x00
#define ARP_REPLY_L    0x02
#define ARP_DST_IP     0x26

#define IP_HEADER_LEN  0x14
#define IP_VERSION     0x0E
#define IP_TOTLEN      0x10
#define IP_FLAGS       0x14
#define IP_TTL         0x16
#define IP_PROTO       0x17
#define IP_CHECKSUM    0x18
#define IP_SRC         0x1A
#define IP_DST         0x1E
#define IP_ICMP        0x01
#define IP_TCP         0x06
#define IP_UDP         0x11

#define ICMP_HEADER_LEN   8
#define ICMP_TYPE      0x22
#define ICMP_CHECKSUM  0x24
#define ICMP_PID       0x26
#define ICMP_NUM       0x28
#define ICMP_DATA      0x2A
#define ICMP_REQUEST   0x08
#define ICMP_REPLY     0x00

#define UDP_HEADER_LEN    8
#define UDP_SRC_PORT   0x22
#define UDP_DST_PORT   0x24
#define UDP_LEN        0x26
#define UDP_CHECKSUM   0x28
#define UDP_DATA       0x2a

#define TCP_SRC_PORT   0x22
#define TCP_DST_PORT   0x24
#define TCP_SEQ        0x26
#define TCP_SEQACK     0x2A
#define TCP_HEADER_LEN 0x2E
#define TCP_FLAGS      0x2F
#define TCP_WIN_SIZE   0x30
#define TCP_CHECKSUM   0x32
#define TCP_OPTIONS    0x36
#define TCP_URG        0x20
#define TCP_ACK        0x10
#define TCP_PSH        0x08
#define TCP_RST        0x04 
#define TCP_SYN        0x02
#define TCP_FIN        0x01
#define TCP_LEN_PLAIN  0x14

typedef uint8_t byte;
typedef uint16_t word;

void PacketFunc(void);
byte LinkFunc(void);
void tcp_connect_ip(byte *ip, word port);
void tcp_connect_http(char *host);
word tcp_send(char *off);
void tcp_receive(char *off, word len);
void ping_ip(byte *ip, word num);

#endif // __NETWORK_H__
