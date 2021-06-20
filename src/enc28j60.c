#include "enc28j60.h"
#include "gpio.h"
#include "dwt.h"

uint8_t macaddr[6] = { 0,1,2,3,4,5 };

static uint8_t ENC28J60_Bank;
static uint16_t gNextPacketPtr;

static void cs_on(void)
{
  GPIOA->BRR = GPIO_BRR_BR4;
}

static void cs_off(void)
{
  GPIOA->BSRR = GPIO_BSRR_BS4;
}

static void waitSpi(void)
{
 while (!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY));
}

uint8_t ENC28J60_ReadOp(uint8_t op,uint8_t address)
{
  cs_on();
  SPI1->DR = op | (address & ADDR_MASK);
  waitSpi();
  SPI1->DR = 0x00;
  waitSpi();
  if(address & 0x80)
  {
    SPI1->DR = 0x00;
    waitSpi();
  }
  cs_off();
  return(SPI1->DR);
}

void ENC28J60_WriteOp(uint8_t op, uint8_t address, uint8_t data)
{
  cs_on();
  SPI1->DR = op | (address & ADDR_MASK);
  waitSpi();
  SPI1->DR = data;
  waitSpi();
  cs_off();
}

void ENC28J60_ReadBuffer(uint16_t len, uint8_t* data)
{
  cs_on();
  SPI1->DR = RBM;
  waitSpi();
  while(len)
  {
    len--;
    SPI1->DR = 0x00;
    waitSpi();
    *data = SPI1->DR;
    data++;
  }
  *data='\0';
  cs_off();
}

void ENC28J60_WriteBuffer(uint16_t len, uint8_t* data) {
  cs_on();
  SPI1->DR = WBM;
  waitSpi();
  while(len)
  {
    len--;
    SPI1->DR = *data;
    data++;
    waitSpi();
  }
  cs_off();
}

void ENC28J60_SetBank(uint8_t address)
{
  if((address & BANK_MASK) != ENC28J60_Bank)
  {
    ENC28J60_WriteOp(BFC, ECON1, (BSEL1|BSEL0));
    ENC28J60_WriteOp(BFS, ECON1, (address & BANK_MASK)>>5);
    ENC28J60_Bank = (address & BANK_MASK);
  }
}

uint8_t ENC28J60_Read(uint8_t address)
{
  ENC28J60_SetBank(address);
  return ENC28J60_ReadOp(RCR, address);
}

void ENC28J60_Write(uint8_t address ,uint8_t data)
{
  ENC28J60_SetBank(address);
  ENC28J60_WriteOp(WCR,address,data);
}

uint16_t ENC28J60_PhyRead(uint8_t address)
{
  ENC28J60_Write(MIREGADR,address);
  ENC28J60_Write(MICMD,MIIRD);
  delay_us(15);
  while(ENC28J60_Read(MISTAT) & BUSY);
  ENC28J60_Write(MICMD,0x00);
  return(ENC28J60_Read(MIRDH));
}

void ENC28J60_PhyWrite(uint8_t address, uint16_t data)
{
  ENC28J60_Write(MIREGADR, address);
  ENC28J60_Write(MIWRL, data);
  ENC28J60_Write(MIWRH, data >> 8);
  while(ENC28J60_Read(MISTAT) & BUSY) delay_us(15);
}

uint8_t ENC28J60_Init()
{
  dwtInit();
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  /* --- GPIO setup --- */
  GPIOA_CRL( GP_M(4)    | GP_M(5)     | GP_M(6)  | GP_M(7),
             GP_PP50(4) | GP_APP50(5) | GP_IF(6) | GP_APP50(7) );
  cs_off();
  SPI1->CR1 = SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_BR_0 | SPI_CR1_SSI | SPI_CR1_SSM;
  ENC28J60_WriteOp(SC, 0, SC);
  delay_us(40);
  gNextPacketPtr = RXSTART_INIT;
  ENC28J60_Write(ERXSTL, RXSTART_INIT & 0xFF);
  ENC28J60_Write(ERXSTH, RXSTART_INIT >> 8);
  ENC28J60_Write(ERXRDPTL, RXSTART_INIT & 0xFF);
  ENC28J60_Write(ERXRDPTH, RXSTART_INIT >> 8);
  ENC28J60_Write(ERXNDL, RXSTOP_INIT & 0xFF);
  ENC28J60_Write(ERXNDH, RXSTOP_INIT >> 8);
  ENC28J60_Write(ETXSTL, TXSTART_INIT & 0xFF);
  ENC28J60_Write(ETXSTH, TXSTART_INIT >> 8);
  ENC28J60_Write(ETXNDL, TXSTOP_INIT & 0xFF);
  ENC28J60_Write(ETXNDH, TXSTOP_INIT >> 8);
  ENC28J60_Write(ERXFCON, UCEN | CRCEN | PMEN);
  ENC28J60_Write(EPMM0, 0x3f);
  ENC28J60_Write(EPMM1, 0x30);
  ENC28J60_Write(EPMCSL, 0xf9);
  ENC28J60_Write(EPMCSH, 0xf7);
  ENC28J60_Write(MACON1, (MARXEN | TXPAUS | RXPAUS));
  ENC28J60_Write(MACON2, 0x00);
  ENC28J60_WriteOp(BFS, MACON3, (PADCFG0 | TXCRCEN | FRMLNEN));
  ENC28J60_Write(MAIPGL, 0x12);
  ENC28J60_Write(MAIPGH, 0x0C);
  ENC28J60_Write(MABBIPG, 0x12);
  ENC28J60_Write(MAMXFLL, MAX_FRAMELEN & 0xFF);
  ENC28J60_Write(MAMXFLH, MAX_FRAMELEN >> 8);
  ENC28J60_Write(MAADR5, macaddr[0]);
  ENC28J60_Write(MAADR4, macaddr[1]);
  ENC28J60_Write(MAADR3, macaddr[2]);
  ENC28J60_Write(MAADR2, macaddr[3]);
  ENC28J60_Write(MAADR1, macaddr[4]);
  ENC28J60_Write(MAADR0, macaddr[5]);
  ENC28J60_PhyWrite(PHCON2, HDLDIS);
  ENC28J60_SetBank(ECON1);
  ENC28J60_WriteOp(BFS, EIE, (INTIE|PKTIE));
  ENC28J60_WriteOp(BFS, ECON1,RXEN);
  ENC28J60_Write(ECOCON, 2);
  delay_us(10);
  ENC28J60_PhyWrite(PHLCON, 0x0476);
  delay_us(10);
  return(ENC28J60_Read(EREVID));
}

uint16_t ENC28J60_PacketReceive(uint16_t maxlen, uint8_t* packet)
{
  uint16_t rxstat;
  uint16_t len;
  if (ENC28J60_Read(EPKTCNT)==0) return(0);
  ENC28J60_Write(ERDPTL, (gNextPacketPtr & 0xFF));
  ENC28J60_Write(ERDPTH, gNextPacketPtr >> 8);
  gNextPacketPtr  = ENC28J60_ReadOp(RBM, 0);
  gNextPacketPtr |= ENC28J60_ReadOp(RBM, 0) << 8;
  len  = ENC28J60_ReadOp(RBM, 0);
  len |= ENC28J60_ReadOp(RBM, 0) << 8;
  len -= 4;
  rxstat  = ENC28J60_ReadOp(RBM, 0);
  rxstat |= ((uint16_t)ENC28J60_ReadOp(RBM, 0)) << 8;
  if (len > maxlen - 1) len = maxlen - 1;
    if ((rxstat & 0x80) == 0) len = 0;
    else ENC28J60_ReadBuffer(len, packet);
  ENC28J60_Write(ERXRDPTL, (gNextPacketPtr & 0xFF));
  ENC28J60_Write(ERXRDPTH, gNextPacketPtr >> 8);
  if ((gNextPacketPtr-1 < RXSTART_INIT) || (gNextPacketPtr-1 > RXSTOP_INIT))
  {
    ENC28J60_Write(ERXRDPTL, RXSTOP_INIT & 0xFF);
    ENC28J60_Write(ERXRDPTH, RXSTOP_INIT >> 8);
  }
  else
  {
    ENC28J60_Write(ERXRDPTL, (gNextPacketPtr-1) & 0xFF);
    ENC28J60_Write(ERXRDPTH, (gNextPacketPtr-1) >> 8);
  }
  ENC28J60_WriteOp(BFS, ECON2, PKTDEC);
  return(len);
}

void ENC28J60_PacketSend(uint16_t len, uint8_t* packet)
{
  while (ENC28J60_ReadOp(RCR, ECON1) & TXRTS)
  {
    if(ENC28J60_Read(EIR) & TXERIF)
    {
      ENC28J60_WriteOp(BFS, ECON1, TXRST);
      ENC28J60_WriteOp(BFC, ECON1, TXRST);
    }
  }
  ENC28J60_Write(EWRPTL, TXSTART_INIT & 0xFF);
  ENC28J60_Write(EWRPTH, TXSTART_INIT >> 8);
  ENC28J60_Write(ETXNDL, (TXSTART_INIT + len) & 0xFF);
  ENC28J60_Write(ETXNDH, (TXSTART_INIT + len) >> 8);
  ENC28J60_WriteOp(WBM,0,0x00);
  ENC28J60_WriteBuffer(len, packet);
  ENC28J60_WriteOp(BFS, ECON1, TXRTS);
}

uint8_t ENC28J60_LinkUp(void) {
return((ENC28J60_PhyRead(PHSTAT2) >> 2) & 1);
}