#include "port.h"
#include "lwip/sockets.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "obshal.h"
int server_socket;
char data_buffer[80];
struct sockaddr_in sa,ra,isa;
static int32_t mb_tcpSema;
static hOSThread mdbsTcp_th;
static int32_t tcp_tid;
static uint16_t localport;
static int16_t rcvsize;
static int32_t reqclient;

static void mbSlaveTcpThread(void *param);

BOOL            xMBTCPPortInit( USHORT usTCPPort )
{
    if(!mb_tcpSema){        
        mb_tcpSema =  hOsBiSemaCreate();
        hOsSetThreadName(mdbsTcp_th, "mbtcp");
        mdbsTcp_th.priorty = (int32_t) H_OSTHRD_PRI_NORMAL;
        tcp_tid = hOsCreateThread( &mdbsTcp_th,   (void *)&mbSlaveTcpThread , (void *)usTCPPort);   
        hOsResumeThread(tcp_tid);
    } 
    return TRUE;
}

void            vMBTCPPortClose( void )
{
    close(server_socket);
}

void            vMBTCPPortDisable( void )
{
}

BOOL            xMBTCPPortGetRequest( UCHAR **ppucMBTCPFrame, USHORT * usTCPLength )
{
    *ppucMBTCPFrame = data_buffer;
    *usTCPLength = rcvsize;
    return TRUE;
}

BOOL            xMBTCPPortSendResponse( const UCHAR *pucMBTCPFrame, USHORT usTCPLength )
{
    int pid;
    send(reqclient,pucMBTCPFrame,usTCPLength,0);   
    pid = xMBPortGetEvent(localport);
    xMBPortEventPost(pid, EV_FRAME_SENT);
    return TRUE;
}

static int doTCPServerTask(void)
{
    fd_set readfds;
    struct sockaddr_in address;
    int i, pid;
    int new_socket;
    int max_sd,sd,activity;
    int client_socket[4];
    int addrlen;
    
    for (i = 0; i < 4; i++) 
    {
        client_socket[i] = 0;
    }
    addrlen = sizeof(address);
   //
    // We now put the server into an eternal loop,
    // serving requests as they arrive.
    //
    while (1)
    {
        //clear the socket set
        FD_ZERO(&readfds);
        //add master socket to set
        FD_SET(server_socket, &readfds);

        max_sd = server_socket;

        //add child sockets to set
        for ( i = 0 ; i < 4 ; i++) 
        {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
            FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
            max_sd = sd;
        }
        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
        if (activity < 0) 
        {
            printf("select error");
        }

        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(server_socket, &readfds)) 
        {
            if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                htrace(LOG_MODBUS, "mbtcp accept error\n");
                close(server_socket);
                return -1;
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket ,
            inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
            //add new socket to array of sockets
            for (i = 0; i < 4; i++) 
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);
                    break;
                }
            }
        }

        //else its some IO operation on some other socket :)
        for (i = 0; i < 4; i++) 
        {
            sd = client_socket[i];
            if (FD_ISSET( sd , &readfds)) 
            {
                reqclient = sd;
                //Check if it was for closing , and also read the incoming message
                if ((rcvsize = recv( sd , data_buffer, sizeof(data_buffer),0)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" ,
                    inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }else if(rcvsize){
                    pid = xMBPortGetEvent(localport);
                    xMBPortEventPost(pid, EV_FRAME_RECEIVED);
                }else{
                    printf("recv error\n");
                    close(sd);
                    close(server_socket);
                    return -1;
                }
            }
        }
    }
	  return -1;

}

void mbSlaveTcpThread(void *param)
{
    int addr_size, accept_fd;
    uint16_t port =(uint16_t)param;
restart:
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if ( server_socket < 0 )
    {
        htrace(LOG_MODBUS, "socket alloc error\n");
        goto error_;
    }
    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(MODBUS_LOCALIP);
    sa.sin_port = htons(port);
    localport = port;
    if (bind(server_socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
    {
        printf("Bind to Port Number %d ,IP address %s failed\n", port ,MODBUS_LOCALIP);
        close(server_socket);
        goto error_;
    }
    memset(&ra, 0, sizeof(struct sockaddr_in));
    ra.sin_family = AF_INET;
    ra.sin_addr.s_addr = inet_addr(MODBUS_LOCALIP);
    ra.sin_port = htons(MODBUS_LOCAL_PORT);
    if(listen(server_socket,5) < 0){
        goto error_;
    }
    doTCPServerTask();
error_:
    hSysWait(1000);
    goto restart;
}

