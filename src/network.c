#include <string.h>
#include "usart1.h"
#include "network.h"

void __attribute__((weak)) tcp_receive_callback(char* tcp_addr, word tcp_len) {}
word __attribute__((weak)) tcp_send_callback(char* tcp_addr)
{
  return 0;
}

extern byte macaddr[6];
extern byte ipaddr[4];
extern byte ipgw[4];
extern byte ipdns[4];

static byte macgw[6];
static byte link;
static byte gw;
static byte* icmp;
static byte* iphttp;
byte buf[BUFFER_SIZE + 1];
static byte seqnum = 0x0A;
byte arphdr[] = { 0,1,8,0,6,4,0,1 };
byte iphdr[]  = { 0x45,0,0,0x82,0,0,0x40,0,0x20 };

word checksum(byte *buf, word len, uint32_t sum)
{
  while (len > 1)
  {
    sum += 0xFFFF & (((uint32_t)*buf << 8) | *(buf + 1));
    buf += 2;
    len -= 2;
  }
  if (len) sum += ((uint32_t)(0xFF & *buf)) << 8;
  while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
  return( (word) sum ^ 0xFFFF);
}

byte eth_is_arp(word len)
{
  if (len<41) return(0);
  if (buf[ETH_TYPE] != ETH_ARP_H) return(0);
  if (buf[ETH_TYPE+1] != ETH_ARP_L) return(0);
  for (byte i = 0; i < 4; i++) if(buf[ARP_DST_IP + i] != ipaddr[i]) return(0);
  return(1);
}

byte eth_is_ip(word len)
{
  if (len < 42) return(0);
  if (buf[ETH_TYPE] != ETH_IP_H) return(0);
  if (buf[ETH_TYPE+1] != ETH_IP_L) return(0);
  if (buf[IP_VERSION] != 0x45) return(0);
  for (byte i = 0; i < 4; i++) if(buf[IP_DST + i] != ipaddr[i]) return(0);
return(1);
}

void make_eth_hdr()
{
  memcpy(buf + ETH_DST_MAC, buf + ETH_SRC_MAC, 6);
  memcpy(buf + ETH_SRC_MAC, macaddr, 6);
}

void make_ip_checksum()
{
  buf[IP_CHECKSUM] = 0;
  buf[IP_CHECKSUM + 1] = 0;
  buf[IP_FLAGS] = 0x40;
  buf[IP_FLAGS + 1] = 0;
  buf[IP_TTL] = 64;
  word ck = checksum(buf + ETH_HEADER_LEN, IP_HEADER_LEN, 0);
  buf[IP_CHECKSUM] = ck >> 8;
  buf[IP_CHECKSUM + 1] = ck & 0xFF;
}

void make_ip_hdr()
{
  memcpy(buf+IP_DST, buf + IP_SRC, 4);
  memcpy(buf+IP_SRC, ipaddr, 4);
  make_ip_checksum();
}

void make_icmp_checksum()
{
  buf[ICMP_CHECKSUM] = 0;
  buf[ICMP_CHECKSUM + 1] = 0;
  word ck = checksum(buf + ICMP_TYPE, buf[IP_TOTLEN + 1] - IP_HEADER_LEN, 0);
  buf[ICMP_CHECKSUM] = ck >> 8;
  buf[ICMP_CHECKSUM + 1] = ck & 0xFF;
}

void arp_request(void)
{
  memset(buf + ETH_DST_MAC, 0xff, 6);
  memcpy(buf + ETH_SRC_MAC, macaddr, 6);
  buf[ETH_TYPE] = ETH_ARP_H;
  buf[ETH_TYPE + 1] = ETH_ARP_L;
  buf[ARP_OPCODE] = ARP_REQUEST_H;
  buf[ARP_OPCODE + 1] = ARP_REQUEST_L;
  memcpy(buf + ETH_HEADER_LEN, arphdr, sizeof arphdr);
  memset(buf + ARP_DST_MAC, 0, 6);
  memcpy(buf + ARP_SRC_MAC, macaddr, 6);
  memcpy(buf + ARP_DST_IP, ipgw, 4);
  memcpy(buf + ARP_SRC_IP, ipaddr, 4);
  PacketSend(42, buf); 
}

void arp_reply()
{
  make_eth_hdr();
  buf[ARP_OPCODE] = ARP_REPLY_H;
  buf[ARP_OPCODE + 1] = ARP_REPLY_L;
  memcpy(buf + ARP_DST_MAC, buf + ARP_SRC_MAC, 6);
  memcpy(buf + ARP_SRC_MAC, macaddr, 6);
  memcpy(buf + ARP_DST_IP, buf + ARP_SRC_IP, 4);
  memcpy(buf + ARP_SRC_IP, ipaddr, 4);
  PacketSend(42, buf); 
}

void icmp_request(byte *ip, word num)
{
  memcpy(buf + ETH_DST_MAC, macgw, 6);
  memcpy(buf + ETH_SRC_MAC, macaddr, 6);
  buf[ETH_TYPE] = ETH_IP_H;
  buf[ETH_TYPE + 1] = ETH_IP_L;
  memcpy(buf + ETH_HEADER_LEN, iphdr, sizeof iphdr);
  buf[IP_PROTO] = IP_ICMP;
  buf[IP_TOTLEN] = 0;
  buf[IP_TOTLEN + 1] = IP_HEADER_LEN + ICMP_HEADER_LEN + 32;
  memcpy(buf + IP_DST, ip, 4);
  memcpy(buf + IP_SRC, ipaddr, 4);
  make_ip_checksum();
  buf[ICMP_TYPE] = ICMP_REQUEST;
  buf[ICMP_TYPE + 1] = 0;
  buf[ICMP_PID] = 0;
  buf[ICMP_PID + 1] = 1;
  buf[ICMP_NUM] = num >> 8;
  buf[ICMP_NUM + 1] = num & 0xff;
  for (int i = ICMP_DATA; i < ICMP_DATA + 32; i++) buf[i] = i + 0x37;
  make_icmp_checksum();
  PacketSend(ETH_HEADER_LEN + IP_HEADER_LEN + ICMP_HEADER_LEN + 32, buf);
}

void icmp_reply(word len)
{
  make_eth_hdr();
  make_ip_hdr();
  buf[ICMP_TYPE] = ICMP_REPLY;
  make_icmp_checksum();
  PacketSend(len, buf);
}


void dns_request(char *http, byte *ip)
{
  static byte iddns;
  iphttp = ip;
  memcpy(buf + ETH_DST_MAC, macgw, 6);
  memcpy(buf + ETH_SRC_MAC, macaddr, 6);
  buf[ETH_TYPE] = ETH_IP_H;
  buf[ETH_TYPE + 1] = ETH_IP_L;
  memcpy(buf + ETH_HEADER_LEN, iphdr, sizeof iphdr);
  buf[IP_PROTO] = IP_UDP;
  memcpy(buf + IP_DST, ipdns, 4);
  memcpy(buf + IP_SRC, ipaddr, 4);
  buf[UDP_DST_PORT] = 0;
  buf[UDP_DST_PORT + 1] = DNS_PORT;
  buf[UDP_SRC_PORT] = DNS_SRC_PORT_H;
  buf[UDP_SRC_PORT + 1] = ++iddns;
  buf[UDP_CHECKSUM] = 0;
  buf[UDP_CHECKSUM + 1] = 0;
  memset(buf + UDP_DATA, 0, 12);
  byte *p = buf + UDP_DATA + 12;
  char c;
  do
  {
    byte n = 0;
    for(;;)
    {
      c = *http;
      ++http;
      if (c == '.' || c == 0) break;
      p[++n] = c;
    }
    *p++ = n;
    p += n;
  } while (c != 0);
  *p++ = 0;
  *p++ = 0;
  *p++ = DNS_TYPE_A;
  *p++ = 0;
  *p++ = DNS_CLASS_IN;
  word i = p - buf - UDP_DATA;
  buf[UDP_DATA] = i;
  buf[UDP_DATA + 1] = iddns;
  buf[UDP_DATA + 2] = 1;
  buf[UDP_DATA + 5] = 1;
  i += UDP_HEADER_LEN;
  buf[IP_TOTLEN] = (IP_HEADER_LEN + i) >> 8;
  buf[IP_TOTLEN + 1] = (IP_HEADER_LEN + i) & 0xff;
  make_ip_checksum();
  buf[UDP_LEN] = i >> 8;
  buf[UDP_LEN + 1] = i & 0xff;
  word ck = checksum(buf + IP_SRC, 8 + i, i + IP_UDP);
  buf[UDP_CHECKSUM] = ck >> 8;
  buf[UDP_CHECKSUM + 1] = ck & 0xff;
  PacketSend(ETH_HEADER_LEN + IP_HEADER_LEN + i, buf);
}

void dns_parse(word len)
{
  byte *p = buf + UDP_DATA;
  p += *p;
  for (;;)
  {
    if (*p & 0xC0)
    {
      p += 2;
    }
    else
    {
      while (++p < buf + len)
      {
        if (*p == 0)
        {
          ++p;
          break;
        }
      }
    }
    if (p + 14 > buf + len) break;
    if (p[1] == DNS_TYPE_A && p[9] == 4)
    {
      memcpy(iphttp, p + 10, 4);
      break;
    }
    p += p[9] + 10;
  }
}

void tcp_syn(byte *ip, word port)
{
  memcpy(buf + ETH_DST_MAC, macgw, 6);
  memcpy(buf + ETH_SRC_MAC, macaddr, 6);
  buf[ETH_TYPE] = ETH_IP_H;
  buf[ETH_TYPE + 1] = ETH_IP_L;
  memcpy(buf + ETH_HEADER_LEN, iphdr, sizeof iphdr);
  buf[IP_PROTO] = IP_TCP;
  buf[IP_TOTLEN+1] = IP_HEADER_LEN+TCP_LEN_PLAIN + 4;
  memcpy(buf + IP_DST, ip, 4);
  memcpy(buf + IP_SRC, ipaddr, 4);
  make_ip_checksum();
  buf[TCP_DST_PORT] = port >> 8;
  buf[TCP_DST_PORT + 1] = port & 0xff;
  buf[TCP_SRC_PORT] = 0x0b;
  buf[TCP_SRC_PORT + 1] = 0x21;
  memset(buf + TCP_SEQ, 0, 8);
  buf[TCP_SEQ + 2] = seqnum;
  seqnum += 3;
  buf[TCP_HEADER_LEN] = 0x60;
  buf[TCP_FLAGS] = TCP_SYN;
  buf[TCP_WIN_SIZE] = 0x3;
  buf[TCP_WIN_SIZE + 1] = 0x0;
  *(uint32_t*)(buf + TCP_CHECKSUM) = 0;
  buf[TCP_OPTIONS] = 2;
  buf[TCP_OPTIONS + 1] = 4;
  buf[TCP_OPTIONS + 2] = (CLIENTMSS >> 8);
  buf[TCP_OPTIONS + 3] = (byte) CLIENTMSS;
  word j = checksum(buf + IP_SRC, 8 + TCP_LEN_PLAIN + 4, TCP_LEN_PLAIN + 4 + IP_TCP);
  buf[TCP_CHECKSUM] = j >> 8;
  buf[TCP_CHECKSUM+1] = j & 0xFF;
  PacketSend(ETH_HEADER_LEN + IP_HEADER_LEN + TCP_LEN_PLAIN + 4, buf);
}

void tcp_ack(word add_seq)
{
  byte i = 4;
  while (i--)
  {
    add_seq += buf[TCP_SEQ + i];
    byte tseq = buf[TCP_SEQACK + i];
    buf[TCP_SEQACK + i] = add_seq;
    buf[TCP_SEQ + i] = tseq;
    add_seq >>= 8;
  }
  make_eth_hdr();
  buf[IP_TOTLEN] = 0;
  buf[IP_TOTLEN + 1] = IP_HEADER_LEN + TCP_LEN_PLAIN;
  make_ip_hdr();
  buf[TCP_DST_PORT] = buf[TCP_SRC_PORT];
  buf[TCP_DST_PORT + 1] = buf[TCP_SRC_PORT + 1];
  buf[TCP_SRC_PORT] = 0x0b;
  buf[TCP_SRC_PORT + 1] = 0x21;
  buf[TCP_HEADER_LEN] = 0x50;
  buf[TCP_WIN_SIZE] = 0x3;
  buf[TCP_WIN_SIZE + 1] = 0x0;
  *(uint32_t*)(buf + TCP_CHECKSUM) = 0;
  word j = checksum(buf + IP_SRC, 8 + TCP_LEN_PLAIN, TCP_LEN_PLAIN + IP_TCP);
  buf[TCP_CHECKSUM] = j >> 8;
  buf[TCP_CHECKSUM + 1] = j & 0xFF;
  PacketSend(ETH_HEADER_LEN + IP_HEADER_LEN + TCP_LEN_PLAIN, buf);
}

void tcp_ack_with_data(word len)
{
  buf[IP_TOTLEN] = 0;
  buf[IP_TOTLEN + 1] = IP_HEADER_LEN + TCP_LEN_PLAIN + len;
  make_ip_checksum();
  *(uint32_t*)(buf + TCP_CHECKSUM) = 0;
  word j = checksum(buf + IP_SRC, 8 + TCP_LEN_PLAIN + len, TCP_LEN_PLAIN + IP_TCP + len);
  buf[TCP_CHECKSUM] = j >> 8;
  buf[TCP_CHECKSUM + 1] = j & 0xFF;
  PacketSend(ETH_HEADER_LEN + IP_HEADER_LEN + TCP_LEN_PLAIN + len, buf);
}

uint16_t getTcpLen()
{
  return ((((int16_t)buf[IP_TOTLEN]) << 8) | buf[IP_TOTLEN + 1]) - IP_HEADER_LEN - TCP_LEN_PLAIN;
}

void tcp_connect(byte *ip, word port)
{
  if (!link || !gw) return;
  tcp_syn(ip, port);
}

void ping_ip(byte *ip, word num, byte *reply)
{
  if (!link || !gw) return;
  icmp = reply;
  icmp_request(ip, num);
}

byte LinkFunc()
{
  link = LinkUp();
  if (link)
  {
    if (!gw) arp_request();
  }
  else
  {
  gw = 0;
  }
  return link;
}

void PacketFunc()
{
  word plen = PacketReceive(BUFFER_SIZE, buf);
  if (!plen) return;

  if(eth_is_arp(plen))
  {
    if      (buf[ARP_OPCODE + 1] == ARP_REQUEST_L) arp_reply();
    else if (buf[ARP_OPCODE + 1] == ARP_REPLY_L)
         {
           memcpy(macgw, buf + ARP_SRC_MAC, 6);
           gw = 1;
         }
    return;
  }

  if (eth_is_ip(plen) == 0) return;

  if (buf[IP_PROTO] == IP_ICMP)
  {
    if      (buf[ICMP_TYPE] == ICMP_REPLY) *icmp = ~(buf[ICMP_TYPE + 1]);
    else if (buf[ICMP_TYPE] == ICMP_REQUEST) icmp_reply(plen);
    return;
  }

  if (buf[IP_PROTO]==IP_UDP)
  {
    if (buf[UDP_SRC_PORT + 1] == DNS_PORT && !(buf[UDP_DATA + 3] & 0x0F)) dns_parse(plen);
    return;
  }

  if (buf[IP_PROTO]==IP_TCP && (buf[TCP_FLAGS] & TCP_ACK))
  {
    word tcp_len = getTcpLen();
    if (buf[TCP_FLAGS] & TCP_RST) return;

    if (buf[TCP_FLAGS] & TCP_SYN)
    {
      buf[TCP_FLAGS] = TCP_ACK;
      tcp_ack(1);
      word send_len = tcp_send_callback((char*)(buf + TCP_OPTIONS));
      if (send_len!=0)
      {
        buf[TCP_FLAGS] = TCP_ACK | TCP_PSH;
        tcp_ack_with_data(send_len);
      }
      return;
    }

    if (buf[TCP_FLAGS] & TCP_PSH && tcp_len != 0)
    {
      tcp_receive_callback((char*)(buf + TCP_OPTIONS), tcp_len);
      buf[TCP_FLAGS] = TCP_ACK | TCP_FIN | TCP_PSH;
      tcp_ack(tcp_len);
      return;
    }

    if (buf[TCP_FLAGS] & TCP_FIN && tcp_len == 0)
    {
      buf[TCP_FLAGS] = TCP_ACK | TCP_FIN | TCP_PSH;
      tcp_ack(1);
      return;
    }
  }
}
