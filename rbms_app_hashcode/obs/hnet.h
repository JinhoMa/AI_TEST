#ifndef _HNET_H
#define _HNET_H

#define NET_AF_INET  1

typedef enum _NetSockTypeEnum {
    NET_SOCK_STREAM = 1,
    NET_SOCK_DGRAM,
    NET_SOCK_RAW,
    NET_SOCK_RDM,
    NET_SOCK_SEQPACKET
} NetSockTypeEnum;



typedef enum _NetSockOptEnum {
    NET_TCP_NODELY = 1000,
    NET_SO_LINGER,
    NET_SO_KEAPALIVE,
    NET_SO_RCVBUF,
    NET_SO_SNDBUF,
    NET_SO_REUSEADDR
} NetSockOptEnum;

typedef enum _hNetEventTypeEnum {
    NET_OPEN_EVENT                      = 0x1,
    NET_CLOSE_EVENT                     = 0x2,
    NET_SOCKET_CONNECT_EVENT    		=0x4,   //socket connect 가 성공
    NET_SOCKET_CLOSE_EVENT          =0x8,      // socket이 끊어졌을때 발생함
    NET_SOCKET_READ_EVENT           =0x10,
    NET_SOCKET_WRITE_EVENT          =0x20,
    NET_SOCKET_ACCEPT_EVENT         =0x40,
    NET_SCOKET_DISCON_EVENT         =0x80,    //server connection 이 끊어진 경우
    NET_SOCKET_TIMEOUT_EVENT           =0x100,  //Keep alive 에 의해서 timeout 발생 시 
    NET_RESOLVE_IP_EVENT            =0x200
}	hNetEventTypeEnum;

#define  NET_READ_NOWAIT    0x1
    
    
#define    FD_ACCEPT                0x1
#define    FD_CLOSE                  0x2
#define    FD_WRITE                  0x4
#define    FD_READ                   0x8
#define    FD_RESIP                   0x10
#define    FD_CONNECT            0x20

typedef struct _hNetEvent {
    int type;
    uint32_t  halId;
} hNetEvent;

#undef FD_SET_SIZE
#define FD_SET_SIZE     16

typedef struct _FD_set{
    unsigned int fd_count;
    unsigned short fd_array[FD_SET_SIZE];  //   [  8bit(set) : 8bit(socket fd)  ]
} FD_set;

#ifndef FD_CLR
#define FD_CLR(fd,set) do { unsigned int __i;	\
    for (__i = 0; __i < ((FD_set *)(set))->fd_count ; __i++) {	\
         if (((FD_set *)(set))->fd_array[__i] == (fd)) {	\
         while (__i < ((FD_set *)(set))->fd_count-1) {	\
             ((FD_set*)(set))->fd_array[__i] = ((FD_set*)(set))->fd_array[__i+1];	\
              __i++;	\
         }	\
          ((FD_set*)(set))->fd_count--;	\
          break;	\
        }	\
    }	\
} while (0)
#endif

#ifndef FD_SET
 #define FD_SET(fd, set) do { unsigned int __i;\
 for (__i = 0; __i < ((FD_set *)(set))->fd_count ; __i++) {\
     if (((FD_set *)(set))->fd_array[__i] &0xFFFF== (fd)) {\
         break;\
     }\
 }\
 if (__i == ((FD_set *)(set))->fd_count) {\
     if (((FD_set *)(set))->fd_count < FD_SET_SIZE) {\
         ((FD_set *)(set))->fd_array[__i] = (fd);\
         ((FD_set *)(set))->fd_count++;\
     }\
 }\
} while(0)
#endif

#ifndef FD_ZERO
#define FD_ZERO(set) (((FD_set *)(set))->fd_count=0)
#endif

#ifndef FD_ISSET
 #define FD_ISSET(fd, set)  hNetSockSelectEnable((int)fd,(int *)set)
 #endif

/**
  * 네트워크를 접속한다. 
  * 과거 Modem 방식과 같이 PPP 접속이나 PPPoe 사용시 회선 접속을 위해서 포팅한다.
  * @return 성공:0, 실패: 음수값 
  */
int32_t hNetConnect(void);
/**
  * 네트워크를 접속을 해제한다.
  * @return 성공 0, 실패: 음수값
  */
int32_t hNetClose(void);

/**
  * Socket 을 open 한다.
  * @param [in] domain 도메인NET_AF_INET 값이 Default 이다.
  * @param [in] type 소켓의 type, TCP의 경우 NET_SOCK_STREAM, UDP의 경우 NET_SOCK_DGRAM
  * @return 성공: 소켓 ID 값, 실패: 음수값 
  */
int32_t hNetSocketOpen(int32_t domain, int32_t type);

/**
  * Socket을 Close 한다.
  * @param  [in] socketFd Socket ID 값
  * @return 성공: 0 , 실패: 음수값
  */
int32_t hNetSocketClose(int32_t socketFd);


/**
  * Socket을연 Client가 Listen 중인 Server의 IP, Port에 연결을 시도한다.
  * IP와 port는 사용하기 전에 반드기 Host 에서 Network 포맷으로 변환시켜야 한다.
  * hNetHtonl, hNetHtons 함수 사용.
  * @param  [in] socketFd Socket ID  값
  * @param  [in] ip Server의 IP
  * @param  [in] port Server의 Bind 되어 있는 Port
  * @return 성공: 0 , 실패: 음수값
  */

int hNetSockConnect(int32_t socketFd, uint32_t *ip,  uint16_t port);

/**
  * Socket을 해당 Port 에 Binding 한다.
  * Client에서 Server에게 접속을 시도 하거나 Client가 UDP Stream을 받기 위해서
  * Bind하는 경우 
  * @param [in] socketFd socket의 ID값
  * @param [in] port 바이딩 하고자 하는 port, 이값은 server 의 port가 아니고 자신의 port이다.
  * @return 성공: 0 , 실패: 음수값
  */

int hNetSockBind(int32_t socketFd, unsigned short port);

/**
  * 데이터를 읽는다.
  * @param [in] socketFd socket ID 값 
  * @param [out] buf buf 데이터를 읽을 버퍼 포인터
  * @param [in] len len 버퍼의 길이
  * @param [in] parmFlag option - NET_READ_NOWAIT : len 만큼 읽지 못하더라도 즉시 리턴한다.
  * @return 읽은 데이터 Size, 데이터가 0 인경우: EOF, E_WOULBLOCK: 접속 대기상태, 다시 시도
  */
int hNetSockRead(int32_t socketFd, uint8_t *buf, int len, int parmFlag);
/**
  * 데이터를 전송한다.
  * @param [in] socketFd socket ID 값 
  * @param [out] buf 데이터가 담긴 버퍼 포인터
  * @param [in] len 전송할 데이터의 사이즈
  * @param [in] parmFlag option - reserved
  * @return 전송된 데이터 Size, E_WOULBLOCK: 접속 대기상태, 다시 시도
  */
int hNetSockWrite(int32_t socketFd, const uint8_t *buf, int len, int parmFlag);

/**
 * Host의 32bit 값을 Network 32bit값으로 변환한다.
 * IP의 값을 Network의 Packet 형태로 보내기 위해 변환한다.
 * System에 맞게 Endian을 맞춰서 포팅해야 한다.
 * @param [in] val Host 의 IP 주소
 * @return 변환된 값
 */
uint32_t hNetHtonl(uint32_t val);
/**
 * Host의 16bit 값을 Network 16bit값으로 변환한다.
 * Port의 값을 Network의 Packet 형태로 보내기 위해 변환한다.
 * System에 맞게 Endian을 맞춰서 포팅해야 한다.
 * @param [in] val Host의 Port
 * @return 변환된 값
 */
uint16_t hNetHtons(uint16_t val);
/**
 * Network의 32bit 값을 Host 32bit값으로 변환한다.
 * Network Packet 수신된 IP의 값을 Host 의 변수 형태로변환한다.
 * System에 맞게 Endian을 맞춰서 포팅해야 한다.
 * @param [in] val 네트p 패킷상의 IP 주소
 * @return 변환된 값
 */
uint32_t hNetNtohl(uint32_t val);
/**
 * Network의 16bit 값을 Host 16bit값으로 변환한다.
 * Network Packet 수신된 Port의 값을 Host 의 변수 형태로변환한다.
 * System에 맞게 Endian을 맞춰서 포팅해야 한다.
 * @param  [in] val 네트p 패킷상의Port
 * @return 변환된 값
 */
uint16_t hNetNtohs(uint16_t val);

/**
  * socket 을 incomming connection을 위한 listen 상태로 변환한다.
  * @param [in] socketFd socket ID값
  * @return 성공: 0 , 실패: 음수값
  */
int hNetSockListen(int32_t socketFd);

/**
  * socket 상에서 incomming connection을 시도하는 것을 허용한다.
  * 성공하게 되면 접속을 시도한 client의 ip 와 port 가 수신된다.
  * @param [in] socketFd socket ID 값
  * @param [out] ip 접속된 IP값의 포인터
  * @param [out] port 접속된 client Port값 의 포인터
  * @return 성공: socket ID, 실패: 음수값 
  */
int hNetSockAccept(int32_t socketFd, uint32_t *ip, uint16_t *port);

/** 
  * network API 동작은 default 가 blocking 으로 동작 하며, 비동기적으로 사용하기 위해서는
  * 이 함수를 반드시 socket을 열기 전에 호출되어야 한다.
  * event를 수신할 handler 함수를 등록 해야 하며 handler의 prototype은 "void func(void *param)"
  * 형태이면 param은 hNetEvent 구조의 포인터를 넘겨 받는다. 첫번째 field type은 hNetEventTypeEnum
  * 값이며 두번째 field halId는 socket의 ID값이다.
  * @param [in] func event를 수신할 function 포인터
  */
void hNetAsyncStartup(void *func);

/**
  * 비동기 사용을 종료한다.
  * network의 resource를 최종적으로 종료하는 시점에 호출하면 된다.
  */
void hNetAsyncCleanup(void);
/**
  * 비동기 Event를 수신할 flag를 OR 해서 등록한다.
  * Event 가 발생 이후 계속 flag 상태는 유지 된다.
  * 더이상 Event를 수신하지 않기를 원한다면 flag를 0으로 설정되어야 한다.
  * 그 이후 network api는 blocking api와 같은 동작을 하게 된다.
  * @param [in] socket socket ID값
  * @param [in] flag  hNetAsyncFlagEnum 값 
  */
void hNetAsyncSelect(int32_t socket, int32_t flag);

int hNetSockSetOpt(int32_t socket, int option, int val);

/**
  * 데이터를 읽는다.
  * @param [in] socketFd socket ID 값 
  * @param [out] buf buf 데이터를 읽을 버퍼 포인터
  * @param [in] len len 버퍼의 길이
  * @param [out] addr  해당 포트로 전송한 장치 의 IP 
  * @param [out] port   해당 포트로 전송한 장치의 Port
  * @param [in] parmFlag option - NET_READ_NOWAIT : len 만큼 읽지 못하더라도 즉시 리턴한다.
  * @return 읽은 데이터 Size, 데이터가 0 인경우: EOF, E_WOULBLOCK: 접속 대기상태, 다시 시도
  */
int32_t hNetSockReadFrom(int32_t socketFd, unsigned char *buf, int len, uint8_t *addr, uint16_t *port, int parmFlag);

/**
  * 데이터를 읽는다.
  * @param [in] socketFd socket ID 값 
  * @param [in] buf buf 데이터를 읽을 버퍼 포인터
  * @param [in] len len 버퍼의 길이
  * @param [in] addr  데이터 전송 목적 IP
  * @param [in] port   데이터 전송 목적 Host의 Port
  * @return 읽은 데이터 Size, 데이터가 0 인경우: EOF, E_WOULBLOCK: 접속 대기상태, 다시 시도
  */
int32_t hNetSockSendTo(int32_t socketFd, unsigned char *buf, int len, uint8_t *addr, uint16_t port);


/**
**/
void hNetInitialize(void);
void hNetExIpConfig(uint8_t* ip, uint8_t *gw, uint8_t *mask);
int hNetExPhyConnect(void);
PUBLIC void hNetExReadIpConfig(uint8_t* ip, uint8_t *gw, uint8_t *mask);

PUBLIC void HalNetThread(void *param);

#endif
