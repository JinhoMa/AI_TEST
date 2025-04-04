
#include <stdio.h>
#include <string.h>
#include <stdint.h>

//#include "hError.h"
//#include "hnet.h"
#include "obshal.h"
#include "main.h"
#include "socket.h"
#include "hnet.h"

#define WM_NETEVENT (WM_USER+1)

#define NET_MAX_SOCKET   _WIZCHIP_SOCK_NUM_
#define NET_MAX_PACKET_LENGTH    1400
#define NET_MULTIPORTn      4

#define SOCKBASE        0
#define PTR2FD(x)  ((uint32_t )x - (uint32_t)&sockets[0])/sizeof(NetSocket)
#define FD2PTR(x)  ((uint32_t)&sockets[0]+x*sizeof(NetSocket))

typedef struct _NetSocket {
    unsigned int used;
    int fd; // physical socket fd 
    unsigned int flags;
    unsigned int closed;
    unsigned int event;
    unsigned int type;
    unsigned int svrport; // port to bind
    unsigned int port;
} NetSocket;

#define PHY_SOCKET_CLOSED               0
//#define PHY_SOCKET_BIND                     1
#define PHY_SOCKET_LISTEN                    2
#define PHY_SOCKET_ESTABLISHED          3

static NetSocket sockets[NET_MAX_SOCKET];
static void (*AsyncFunc)(void *param);

//#define HP_HUB

#ifdef HP_HUB
wiz_NetInfo netConf = { {0x00, 0x08, 0xdc,0x1E, 0x36, 0x7F},{169, 254, 18, 67},{255,255,255,0},{169, 254, 18, 68},{0,0,0,0}, NETINFO_STATIC };
#elif defined(IPTIME_HUB)
//wiz_NetInfo netConf = { {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},{192, 168, 0, 2},{255,255,255,0},{192, 168, 0, 1},{0,0,0,0}, NETINFO_STATIC };
#else
wiz_NetInfo netConf = { {0x00, 0x08, 0xdc,0x11, 0x22, 0x00},{192, 168, 0, 3},{255,255,255,0},{192, 168, 0, 1},{0,0,0,0}, NETINFO_STATIC };
#endif


int32_t eth_spi_fd;

extern void clearSending(uint8_t sn);
  
static void sysLock(){
  //  __disable_irq();

}
static void sysUnlock(){
  //  __enable_irq();
}

static void ethCSEnable(){
	ETH_CS_ENABLE;
	hTimDelay(100);
}
static void ethCSDisable(){
	ETH_CS_DISABLE;
}

static uint8_t ethSpiRcvByte(void){
    uint8_t ret= hSpiReceiveByte(eth_spi_fd);
	return ret;
}

static void ethSpiSendByte(uint8_t data){
    hSpiSendByte(eth_spi_fd, data);
}

static NetSocket* allocVirtualSocket(void)
{
    int i;
    NetSocket *socket;
    for (i = 0; i < NET_MAX_SOCKET; i++)
    {
        if (!sockets[i].used)
        {
            socket = &sockets[i];
            socket->fd =-1;  
            goto ON_FOUND;
        }
    }
    return NULL;
ON_FOUND:
    socket->used = 1;
    return socket;
}

static void freeVirtualSocket(NetSocket *socket)
{
    memset(socket, 0x0, sizeof(NetSocket));
    socket->fd = -1;
}


static NetSocket* getVirtualSocket(int fd)
{
    int i;
    for (i = 0; i < NET_MAX_SOCKET; i++)
    {
        if (sockets[i].used && sockets[i].fd == fd )
        {
            return &sockets[i];
        }
    }
    return 0;
}

static void clearPendingEvent(NetSocket *s,unsigned char ch, int event)
{
    if(s) s->event &= ~(event);
    switch(event){
        case NET_SCOKET_DISCON_EVENT:
            setSn_IR(ch,Sn_IR_DISCON);
            break;
        case NET_SOCKET_WRITE_EVENT:
            setSn_IR(ch,Sn_IR_SENDOK);
            break;
        case NET_SOCKET_CONNECT_EVENT:
        case NET_SOCKET_ACCEPT_EVENT:
            setSn_IR(ch,Sn_IR_CON);
            break;
        case NET_SOCKET_READ_EVENT:
            setSn_IR(ch,Sn_IR_RECV);
            break;
    }
}

#define BUF_SIZE        1024
typedef struct  _NetworkData {
    int socket;
    struct _tx{
        char buf[BUF_SIZE];
        int rear;
        int front;
    } tx;
    struct _rx{
        char buf[BUF_SIZE];
        int rear;
        int front;
   } rx;
} NetworkData;



/////////////////////////////////////////////////
/**
 * Not Used
 */
int32_t hNetSetAsyncHandler(void * func) {
    AsyncFunc = (void (*)(void *))func;
    return 0;
}
int32_t hNetConnect() { return 0; }
int32_t hNetClose() { return 0; }
////////////////////////////////////////////////

/**
 * w5500: socket_(fd, port) API는 socket create시에 bind할 port를 넘기도록 되어있다.
 * 즉 server socket은 socket_() API호출 시점을 뒤로 미뤄야 한다. 
 */
int32_t hNetSocketOpen(int32_t domain, int32_t type)
{
    NetSocket *newSocket;
    newSocket = allocVirtualSocket();
    if (newSocket == NULL) {
        return E_ERROR;
    }
    newSocket->type = type;
    return PTR2FD(newSocket);
}
static void setSockBlockMode(unsigned char ch, unsigned char mode ){
    unsigned char param;
    if(mode){
        param = SOCK_IO_BLOCK;
        ctlsocket(ch, CS_SET_IOMODE, &param);
    }else{
        param = SOCK_IO_NONBLOCK;
        ctlsocket(ch, CS_SET_IOMODE, &param);
    }
}
static int setSockInterruptDisable(unsigned char ch)
{
    unsigned short intmask;
    ctlwizchip(CW_GET_INTRMASK, &intmask);
    intmask &=(ch==0)? ~IK_SOCK_0 :  \
                  (ch==1)? ~IK_SOCK_1:   \
                  (ch==2)? ~IK_SOCK_2: \
                  (ch==3)? ~IK_SOCK_3:   \
                  (ch==4)? ~IK_SOCK_4:   \
                  (ch==5)? ~IK_SOCK_5:  \
                  (ch==6)? ~IK_SOCK_6:  ~IK_SOCK_7;
    ctlwizchip(CW_SET_INTRMASK, (void *)&intmask);
    return 0;
}

int32_t hNetSocketClose(int32_t socketFd)
{
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    clearPendingEvent(s, s->fd, NET_SOCKET_CLOSE_EVENT);
    if ( s->fd >= 0){
        close_(s->fd);
    }
    freeVirtualSocket(s);
    return 0;
}

int hNetW5500State(int32_t socketFd)
{
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    return getSn_SR(s->fd)&0xFF;
}

int hNetSockConnect(int32_t socketFd, uint32_t *ip, uint16_t port)
{
    int8_t ret;
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    s->event &= ~(NET_SOCKET_CONNECT_EVENT);
    if(s->fd == -1 && s->port == 0){
       ret = socket_(socketFd,Sn_MR_TCP,0,0);
       if(ret < 0) { return E_ERROR; }
       s->fd = socketFd & 0xFF;
       s->port = port ;
       setSockBlockMode(s->fd,0);
       setSockInterruptDisable(s->fd);
    }

#ifdef __TMS320C2000__
    ip_addr[0]=(uint8_t) (*ip&0xFF);
    ip_addr[1]=(uint8_t) ((*ip&0xFF00)>>8);
    ip_addr[2]=(uint8_t) ((*ip&0xFF0000)>>16);
    ip_addr[3]=(uint8_t) ((*ip&0xFF000000)>>24);

    ret = connect_(s->fd, ip_addr, port);
#else
    ret = connect_(s->fd, (uint8_t *)ip, port);
#endif
    if(ret ==0) return E_WOULDBLOCK;
    else if(ret == SOCKERR_TIMEOUT) return E_TIMEOUT;
    else if(ret == SOCKERR_SOCKINIT) return E_INVALID;
    else return ret;
}

/**
 * w5500에서는 bind()함수가 없음. 
 * w5500은 serversocket이 accept될 때 새로운 client socket을 만들지 않는다.
 * 그로인해서 bind()함수 없이 listen()함수만 있다.
 * 가상의 bind구현 한다.
 */
int hNetSockBind(int32_t socketFd, unsigned short port)
{
    int ret=0, param;
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    s->svrport = port;
    if( s->type == NET_SOCK_STREAM){
    ret = socket_(socketFd, Sn_MR_TCP, s->svrport, 0);
    }
    if( s->type == NET_SOCK_DGRAM){
        ret = socket_(socketFd, Sn_MR_UDP, s->svrport, 0);
    }
    if (ret != socketFd){
        return E_ERROR;
    }
    s->fd = ret;
    param = SOCK_IO_BLOCK;
    ctlsocket(socketFd, CS_SET_IOMODE, &param);
    return 0;
}

int hNetSockRead(int32_t socketFd, unsigned char *buf, int len, int parmFlag)
{
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    int ret;
    int count=0;
    clearPendingEvent(s, s->fd, NET_SOCKET_READ_EVENT);
    if(s->flags & FD_READ){ //async
        //수신된 데이터가 없다면 즉시 리턴 한다.
        ret = recv_(s->fd, buf, len);
        if(ret == SOCK_BUSY) 
            return E_WOULDBLOCK;
        else if (ret < 0) 
            return E_ERROR;
        return ret;
    }else {
        while(1)
        {
            //수신 된 데이터가 없다면 기다리고, 데이터가 있으면 읽고 그 만큼 리턴한다.
            if(parmFlag & NET_READ_NOWAIT)
            {
                setSockBlockMode(s->fd, 0);
                ret = recv_(s->fd, buf+count, len);
                //hLogProc(LOG_WARN, "read %d\n", ret);
                setSockBlockMode(s->fd, 1);
                return ret;
            }else{
                ret = recv_(s->fd, buf+count, len);
            }
            if(ret <0) 
                return E_ERROR;
            len -=ret;
            count += ret;
            if(len <=0) 
            return count;
        }

    }
}


int32_t hNetSockReadFrom(int32_t socketFd, unsigned char *buf, int len, uint8_t *addr, uint16_t *port, int parmFlag)
{
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    int ret;
    int count=0;
    clearPendingEvent(s, s->fd, NET_SOCKET_READ_EVENT);
    if(s->flags & FD_READ){ //async
        //수신된 데이터가 없다면 즉시 리턴 한다.
        ret = recvfrom_(s->fd, buf, len, addr, port);
        if(ret == SOCK_BUSY) return E_WOULDBLOCK;
        else if (ret < 0) return E_ERROR;
        return ret;
    }else {
        while(1)
        {
            //수신 된 데이터가 없다면 기다리고, 데이터가 있으면 읽고 그 만큼 리턴한다.
            if(parmFlag & NET_READ_NOWAIT)
            {
                setSockBlockMode(s->fd, 0);
                ret = recvfrom_(s->fd, buf+count, len, addr, port);
                //hLogProc(LOG_WARN, "read %d\n", ret);
                setSockBlockMode(s->fd, 1);
                return ret;
            }else{
                ret = recvfrom_(s->fd, buf+count, len, addr, port);
            }
            if(ret <0) return E_ERROR;
            len -=ret;
            count += ret;
            if(len <=0) 
            return count;
        }

    }
}

static void write_interrupt_disable(unsigned char fd){
    unsigned char mask;
    ctlsocket(fd,CS_GET_INTMASK, &mask);
    mask &= ~(Sn_IR_SENDOK);
    ctlsocket(fd,CS_SET_INTMASK, &mask);
}

static void write_interrupt_enable(unsigned char fd){
    unsigned char mask;
    ctlsocket(fd,CS_GET_INTMASK, &mask);
    mask |= Sn_IR_SENDOK;
    ctlsocket(fd,CS_SET_INTMASK, &mask);
}

int hNetSockWrite(int32_t socketFd, const unsigned char *buf,
        int len, int parmFlag)
{
    int count=0;
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    int ret;
    clearPendingEvent(s, s->fd, NET_SOCKET_WRITE_EVENT);
    if(s->flags & FD_WRITE){ //async
        write_interrupt_disable(s->fd);
        ret = send_(s->fd,(unsigned char *) buf, len);
        write_interrupt_enable(s->fd);
        if(ret == SOCK_BUSY) return E_WOULDBLOCK;
        else if (ret < 0)
        	return E_ERROR;
        return ret;
    }
    else {
           while(1){
               hSysWait(1);
               clearSending(s->fd);
               ret = send_(s->fd, (unsigned char *)(buf+count), len);
               if(ret <0)
            	   return E_ERROR;
               if(ret == SOCK_BUSY) {
                    //htrace(LOG_WARN, "[hal] sock busy \n");
                    if(parmFlag ==0 ) return count;
                    continue; // return E_WOULDBLOCK;
                }
               len -=ret;
               count += ret;
               if(len <=0) 
                return count;
           }
    }
}

int hNetSockRcvFrom(int32_t socketFd, uint8_t *buf,
        int len, uint32_t *ip, uint16_t *port)
{
    int ret=0;
    int count=0;
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    if(s->flags & FD_READ){
        ret = recvfrom_(s->fd,buf,len,(unsigned char *)ip, port);
        if(ret == SOCK_BUSY) return E_WOULDBLOCK;
        else if(ret < 0) return E_ERROR;
    }else{
        while(1){
            ret = recvfrom_(s->fd, buf+count, len, (unsigned char *)ip, port);
            if(ret < 0) return E_ERROR;
            len -= ret;
            count+= ret;
            if(len<=0) return count;
        }
    }
    return ret;
}

int hNetSockSendTo(int32_t socketFd, uint8_t *buf,
        int len, uint8_t *ip, uint16_t port)
{
    int ret=0;
    int count=0;
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    if(s->flags & FD_WRITE){
        ret = sendto_(s->fd,buf,len,(unsigned char *)ip, port);
        if(ret == SOCK_BUSY) return E_WOULDBLOCK;
        else if(ret < 0) return E_ERROR;
    }else{
        while(1){
            ret = sendto_(s->fd, buf+count, len,(unsigned char *)ip, port);
            if(ret < 0) return E_ERROR;
            len -= ret;
            count+= ret;
            if(len<=0) return count;
        }
    }
    return ret;
}

int hNetSockGetOpt(int32_t socketFd, int option)
{
    return 0;
}

int hNetSockSetOpt(int32_t socketFd, int option,
        int val)
{
    int ret=0;
    uint8_t opt;
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    switch(option){
        case NET_SO_KEAPALIVE:
            opt =(uint8_t)(val/5);
            opt = (opt==0)?1: opt;
            ret = setsockopt(s->fd, SO_KEEPALIVEAUTO, &opt);
        break;
            
    }
    return ret;
}

/**
 * w5500에서는 accept가 없고,  BSD socket처럼 client socket이 별도로 생성되는 것이 아니라,
 * 원래 server socket용도로 만든 소켓을 그대로 재활용해서 client와 read/write해야 한다.
 *
 */
int hNetSockListen(int32_t socketFd)
{
    char ret;
    //NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    ret = listen_(socketFd);
    if (ret != SOCK_OK) {
        return E_ERROR;
    }
    return 0;
}

/**
 * Wiznet w5500에서는 accept가 없음()
 * @return 성공시 원래 socketFd 값을 return 한다.`
 * ToDo: 무한루프 도는 걸 interrupt방식으로 수정하도록 해야 함
 */
int hNetSockAccept(int32_t socketFd, uint32_t *ip, uint16_t *port)
{
    NetSocket *s =(NetSocket *) FD2PTR(socketFd);
    //NetSocket *newsock;
    //unsigned char ch=0;
    unsigned char SR;
    for(;;){
        SR = getSn_SR(socketFd);
        if (SR == SOCK_ESTABLISHED){  // Accept event에 대해서는 체크할 필요가 없나?
            break;
        }else if (SR == SOCK_LISTEN){
            return E_WOULDBLOCK;
        }else {
          hLogProc(LOG_WARN, "Accept Erro %d\n", SR);
          return E_ERROR;
        }
    }
    clearPendingEvent(s, socketFd, NET_SOCKET_ACCEPT_EVENT);
    return (int)socketFd;
}

int hNetGetMaxPacketLength()
{
    return NET_MAX_PACKET_LENGTH;	
}

int hNetGetHostByName(const unsigned char *name, uint32_t *ip)
{
    *ip = ((int32_t)netConf.ip[0]<<24)&0xFF000000 |
            ((int32_t)netConf.ip[1]<<16)&0x00FF0000 |
            ((int32_t)netConf.ip[2]<<8) & 0x0000FF00 |
            ((int32_t)netConf.ip[3]) ;
    return(0);
}

int hNetGetHostName(char *buf, int buflen)
{
    sprintf(buf,"%d.%d.%d.%d\r\n", netConf.ip[0],netConf.ip[1],netConf.ip[2],netConf.ip[3]);
    return 0;
}

uint32_t hNetHtonl(uint32_t val)
{
    return hNetNtohl(val);
}

uint16_t hNetHtons(uint16_t val)
{
    return ((val && 0xFF) << 8) | ((val&0xFF00) >> 8);
}

uint32_t hNetNtohl(uint32_t val)
{
    //char  pointer 로 값이 저장되어 있어서 뒤집혀 있다.
#ifdef __TMS320C2000__
	return ( ((uint64_t)(val&0xFF)<<24) | (((uint64_t)(val&0xFF00)<<8)) | (((uint64_t)(val&0xFF0000)>>8)) | (((uint64_t)(val & 0xFF000000)>>24)) );
#else
    return  ((val &0xFF) <<24) |((val&0xFF00)<<8) | ((val&0xFF0000)>>8) |((val & 0xFF000000)>>24);
#endif
}

uint16_t hNetNtohs(uint16_t val)
{
    // short pointer 로 저장되어 있어서 그대로 카피된다. 상태
    // ---------------------------------------
    // client port[0x1234]
    // client(0x1234) -> clientNet(0x34-> 0x12->) |  wiznet( 0x12<<8)|0x34 )
    return val;
}


int hNetGetAvailableSize(int32_t socketId)
{
	return 1;
}

void hNetFilnalize(void)
{
}

void hNetAsyncStartup(void *func)
{
    AsyncFunc = (void (*)(void *))func;
}
void hNetAsyncCleanup(void)
{
    AsyncFunc = NULL;
}
void hNetAsyncSelect(int32_t socket, int32_t flag){
   int param;
   int intmask=0;
   NetSocket *s = (NetSocket *)socket;
   s->flags = flag;
   param = SOCK_IO_NONBLOCK;
   ctlsocket(s->fd, CS_SET_IOMODE, &param);
   //interrupt enable
   
   /* interrupt Enable */
   ctlwizchip(CW_GET_INTRMASK, &intmask);
   #if 1
   intmask |=(s->fd==0)? IK_SOCK_0 :  \
                  (s->fd==1)? IK_SOCK_1:   \
                  (s->fd==2)? IK_SOCK_2: \
                  (s->fd==3)? IK_SOCK_3:   \
                  (s->fd==4)? IK_SOCK_4:   \
                  (s->fd==5)? IK_SOCK_5:  \
                  (s->fd==6)? IK_SOCK_6:  IK_SOCK_7;
   #else
   intmask |= IK_SOCK_ALL;
   #endif
   ctlwizchip(CW_SET_INTRMASK, (void *)&intmask);
}

static void doUpdateEvent(void)
{
    NetSocket *sock=NULL;
    unsigned char snir;
    //unsigned char sr;
    unsigned char ch;
    unsigned int event=0;
    hNetEvent nEvent;
    for(ch=0;ch< NET_MAX_SOCKET;ch++)
    {
        event= 0;
        if (sockets[ch].fd < 0) {
            continue;
        }
        snir = getSn_IR(ch);
        if(snir & Sn_IR_SENDOK){
            setSn_IR(ch,Sn_IR_SENDOK);
            clearSending(ch);
            event |= NET_SOCKET_WRITE_EVENT;
            //printf("ch(%d) : wr evt \n", ch);
        }
        if(snir & Sn_IR_RECV) {
            event |= NET_SOCKET_READ_EVENT;
            //printf("ch(%d) :rd evt %d\n", ch, event);
        }
        if(snir & Sn_IR_CON){
            setSn_IR(ch,Sn_IR_CON);
            event |= NET_SOCKET_CONNECT_EVENT;
            printf("ch(%d) :conn evt \n", ch);
        }
        if(snir & Sn_IR_DISCON){
            setSn_IR(ch,Sn_IR_DISCON);
            event |= NET_SOCKET_CLOSE_EVENT;
            printf("ch(%d) :disconn evt \n", ch);
        }

        if(snir & Sn_IR_TIMEOUT){
        	setSn_IR(ch,Sn_IR_TIMEOUT);
            event |= NET_SOCKET_CLOSE_EVENT;    //keepalive �� ���� timeout �߻� 
        }
         //server socket �� ���� �Ѵ�. 
  
        if (!event) {
            continue;
        }
        if (( sock = getVirtualSocket(ch)) == NULL) {
            continue;
        }
        sock->event |= event;
        if(AsyncFunc){
            nEvent.halId = (uint32_t)PTR2FD(sock);
            nEvent.type = event;
            AsyncFunc(&nEvent);
        }
    }    
}

PUBLIC void HalNetThread(void *param)
{
    doUpdateEvent();
}

extern void sysLock();
extern void sysUnlock();
extern void ethCSEnable();
extern void ethCSDisable();
extern uint8_t ethSpiRcvByte(void);
extern void ethSpiSendByte(uint8_t data);


static void print_ipconfig(void){
    uint8_t tmpstr[6];
    // Display Network Information
    ctlwizchip(CW_GET_ID,(void*)tmpstr);
    printf( "\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
    printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n",netConf.mac[0],netConf.mac[1],netConf.mac[2],
	  netConf.mac[3],netConf.mac[4],netConf.mac[5]);
    printf("SIP: %d.%d.%d.%d\r\n", netConf.ip[0],netConf.ip[1],netConf.ip[2],netConf.ip[3]);
    printf("GAR: %d.%d.%d.%d\r\n", netConf.gw[0],netConf.gw[1],netConf.gw[2],netConf.gw[3]);
    printf("SUB: %d.%d.%d.%d\r\n", netConf.sn[0],netConf.sn[1],netConf.sn[2],netConf.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", netConf.dns[0],netConf.dns[1],netConf.dns[2],netConf.dns[3]);
    printf("======================\r\n");
}

PUBLIC void hNetExReadIpConfig(uint8_t* ip, uint8_t *gw, uint8_t *mask)
{
    memcpy(ip, netConf.ip, 4);
    memcpy(gw, netConf.gw, 4);
    memcpy(mask, netConf.sn, 4);
}

PUBLIC void hNetExIpConfig(uint8_t* ip, uint8_t* gw, uint8_t* mask)
{
    if(*ip){
        memcpy( netConf.ip, ip, 4);
        netConf.mac[5] = netConf.ip[3];
    }
    if(*gw) memcpy( netConf.gw, gw, 4);
    if(*mask) memcpy( netConf.sn, mask, 4);
    ctlnetwork(CN_SET_NETINFO, (void*)&netConf);
    memset(&netConf,0x0, sizeof(wiz_NetInfo));
    ctlnetwork(CN_GET_NETINFO, (void*)&netConf);
    print_ipconfig();
}

PUBLIC int hNetExPhyConnect(void)
{
    uint8_t tmp;
    ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) ;
    if(tmp == PHY_LINK_OFF ){    
        // printf("Unknown PHY Link stauts.\r\n");
        return PHY_LINK_OFF;
    }
    return PHY_LINK_ON;
}

PUBLIC void hNetInitialize(void)
{
    int intmask=0;
    //int retry =100;
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
    //Reset
    char *port = hSysProperty("ethernet.spi.port");
    eth_spi_fd = hSpiOpen(port,0);
    assert_param(eth_spi_fd>0);
    ETH_RESET_ENABLE;
    hSysWait(1);
    ETH_RESET_DISABLE;
    hSysWait(20);
    ETH_CS_ENABLE;
    ETH_CS_DISABLE;
#ifdef _RBMS
    reg_wizchip_cris_cbfunc(sysLock, sysUnlock);
    reg_wizchip_cs_cbfunc(ethCSEnable, ethCSDisable);
    reg_wizchip_spi_cbfunc(ethSpiRcvByte,ethSpiSendByte);
#endif
			/* WIZCHIP SOCKET Buffer initialize */
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
     printf("WIZCHIP Initialized fail.\r\n");
     while(1);  //abort ó�� �ʿ�
    }
    intmask = IK_WOL | IK_PPPOE_TERMINATED |IK_IP_CONFLICT|IK_DEST_UNREACH;
    ctlwizchip(CW_SET_INTRMASK, (void *)&intmask);
    memset(sockets,0x0, sizeof(sockets));
}

