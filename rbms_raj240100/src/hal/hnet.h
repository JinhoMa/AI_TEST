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
    NET_SOCKET_CONNECT_EVENT    		=0x4,   //socket connect �� ����
    NET_SOCKET_CLOSE_EVENT          =0x8,      // socket�� ���������� �߻���
    NET_SOCKET_READ_EVENT           =0x10,
    NET_SOCKET_WRITE_EVENT          =0x20,
    NET_SOCKET_ACCEPT_EVENT         =0x40,
    NET_SCOKET_DISCON_EVENT         =0x80,    //server connection �� ������ ���
    NET_SOCKET_TIMEOUT_EVENT           =0x100,  //Keep alive �� ���ؼ� timeout �߻� �� 
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
  * ��Ʈ��ũ�� �����Ѵ�. 
  * ���� Modem ��İ� ���� PPP �����̳� PPPoe ���� ȸ�� ������ ���ؼ� �����Ѵ�.
  * @return ����:0, ����: ������ 
  */
int32_t hNetConnect(void);
/**
  * ��Ʈ��ũ�� ������ �����Ѵ�.
  * @return ���� 0, ����: ������
  */
int32_t hNetClose(void);

/**
  * Socket �� open �Ѵ�.
  * @param [in] domain ������NET_AF_INET ���� Default �̴�.
  * @param [in] type ������ type, TCP�� ��� NET_SOCK_STREAM, UDP�� ��� NET_SOCK_DGRAM
  * @return ����: ���� ID ��, ����: ������ 
  */
int32_t hNetSocketOpen(int32_t domain, int32_t type);

/**
  * Socket�� Close �Ѵ�.
  * @param  [in] socketFd Socket ID ��
  * @return ����: 0 , ����: ������
  */
int32_t hNetSocketClose(int32_t socketFd);


/**
  * Socket���� Client�� Listen ���� Server�� IP, Port�� ������ �õ��Ѵ�.
  * IP�� port�� ����ϱ� ���� �ݵ�� Host ���� Network �������� ��ȯ���Ѿ� �Ѵ�.
  * hNetHtonl, hNetHtons �Լ� ���.
  * @param  [in] socketFd Socket ID  ��
  * @param  [in] ip Server�� IP
  * @param  [in] port Server�� Bind �Ǿ� �ִ� Port
  * @return ����: 0 , ����: ������
  */

int hNetSockConnect(int32_t socketFd, uint32_t *ip,  uint16_t port);

/**
  * Socket�� �ش� Port �� Binding �Ѵ�.
  * Client���� Server���� ������ �õ� �ϰų� Client�� UDP Stream�� �ޱ� ���ؼ�
  * Bind�ϴ� ��� 
  * @param [in] socketFd socket�� ID��
  * @param [in] port ���̵� �ϰ��� �ϴ� port, �̰��� server �� port�� �ƴϰ� �ڽ��� port�̴�.
  * @return ����: 0 , ����: ������
  */

int hNetSockBind(int32_t socketFd, unsigned short port);

/**
  * �����͸� �д´�.
  * @param [in] socketFd socket ID �� 
  * @param [out] buf buf �����͸� ���� ���� ������
  * @param [in] len len ������ ����
  * @param [in] parmFlag option - NET_READ_NOWAIT : len ��ŭ ���� ���ϴ��� ��� �����Ѵ�.
  * @return ���� ������ Size, �����Ͱ� 0 �ΰ��: EOF, E_WOULBLOCK: ���� ������, �ٽ� �õ�
  */
int hNetSockRead(int32_t socketFd, uint8_t *buf, int len, int parmFlag);
/**
  * �����͸� �����Ѵ�.
  * @param [in] socketFd socket ID �� 
  * @param [out] buf �����Ͱ� ��� ���� ������
  * @param [in] len ������ �������� ������
  * @param [in] parmFlag option - reserved
  * @return ���۵� ������ Size, E_WOULBLOCK: ���� ������, �ٽ� �õ�
  */
int hNetSockWrite(int32_t socketFd, const uint8_t *buf, int len, int parmFlag);

/**
 * Host�� 32bit ���� Network 32bit������ ��ȯ�Ѵ�.
 * IP�� ���� Network�� Packet ���·� ������ ���� ��ȯ�Ѵ�.
 * System�� �°� Endian�� ���缭 �����ؾ� �Ѵ�.
 * @param [in] val Host �� IP �ּ�
 * @return ��ȯ�� ��
 */
uint32_t hNetHtonl(uint32_t val);
/**
 * Host�� 16bit ���� Network 16bit������ ��ȯ�Ѵ�.
 * Port�� ���� Network�� Packet ���·� ������ ���� ��ȯ�Ѵ�.
 * System�� �°� Endian�� ���缭 �����ؾ� �Ѵ�.
 * @param [in] val Host�� Port
 * @return ��ȯ�� ��
 */
uint16_t hNetHtons(uint16_t val);
/**
 * Network�� 32bit ���� Host 32bit������ ��ȯ�Ѵ�.
 * Network Packet ���ŵ� IP�� ���� Host �� ���� ���·κ�ȯ�Ѵ�.
 * System�� �°� Endian�� ���缭 �����ؾ� �Ѵ�.
 * @param [in] val ��Ʈ�p ��Ŷ���� IP �ּ�
 * @return ��ȯ�� ��
 */
uint32_t hNetNtohl(uint32_t val);
/**
 * Network�� 16bit ���� Host 16bit������ ��ȯ�Ѵ�.
 * Network Packet ���ŵ� Port�� ���� Host �� ���� ���·κ�ȯ�Ѵ�.
 * System�� �°� Endian�� ���缭 �����ؾ� �Ѵ�.
 * @param  [in] val ��Ʈ�p ��Ŷ����Port
 * @return ��ȯ�� ��
 */
uint16_t hNetNtohs(uint16_t val);

/**
  * socket �� incomming connection�� ���� listen ���·� ��ȯ�Ѵ�.
  * @param [in] socketFd socket ID��
  * @return ����: 0 , ����: ������
  */
int hNetSockListen(int32_t socketFd);

/**
  * socket �󿡼� incomming connection�� �õ��ϴ� ���� ����Ѵ�.
  * �����ϰ� �Ǹ� ������ �õ��� client�� ip �� port �� ���ŵȴ�.
  * @param [in] socketFd socket ID ��
  * @param [out] ip ���ӵ� IP���� ������
  * @param [out] port ���ӵ� client Port�� �� ������
  * @return ����: socket ID, ����: ������ 
  */
int hNetSockAccept(int32_t socketFd, uint32_t *ip, uint16_t *port);

/** 
  * network API ������ default �� blocking ���� ���� �ϸ�, �񵿱������� ����ϱ� ���ؼ���
  * �� �Լ��� �ݵ�� socket�� ���� ���� ȣ��Ǿ�� �Ѵ�.
  * event�� ������ handler �Լ��� ��� �ؾ� �ϸ� handler�� prototype�� "void func(void *param)"
  * �����̸� param�� hNetEvent ������ �����͸� �Ѱ� �޴´�. ù��° field type�� hNetEventTypeEnum
  * ���̸� �ι�° field halId�� socket�� ID���̴�.
  * @param [in] func event�� ������ function ������
  */
void hNetAsyncStartup(void *func);

/**
  * �񵿱� ����� �����Ѵ�.
  * network�� resource�� ���������� �����ϴ� ������ ȣ���ϸ� �ȴ�.
  */
void hNetAsyncCleanup(void);
/**
  * �񵿱� Event�� ������ flag�� OR �ؼ� ����Ѵ�.
  * Event �� �߻� ���� ��� flag ���´� ���� �ȴ�.
  * ���̻� Event�� �������� �ʱ⸦ ���Ѵٸ� flag�� 0���� �����Ǿ�� �Ѵ�.
  * �� ���� network api�� blocking api�� ���� ������ �ϰ� �ȴ�.
  * @param [in] socket socket ID��
  * @param [in] flag  hNetAsyncFlagEnum �� 
  */
void hNetAsyncSelect(int32_t socket, int32_t flag);

int hNetSockSetOpt(int32_t socket, int option, int val);


/**
**/
void hNetInitialize(void);
void hNetExIpConfig(uint8_t* ip, uint8_t *gw, uint8_t *mask);
int hNetExPhyConnect(void);


#endif
