#ifndef __BMSHAL_H
#define __BMSHAL_H
#ifdef __cplusplus
extern "C"
{
#endif	
//#include "r_cg_macrodriver.h"
#include <stdint.h>
#include "htypes.h"
#include "hError.h"
#include "hOS.h"
#include "hlog.h"

typedef enum _hTIMER{
    H_TIMER_RTC = 0x0,
    H_TIMER_1MS_APP ,
    H_TIMER_1MS_TICK,
    H_TIMER_PWMOUT,
    H_TIMER_PWMIN,
    H_TIMER_END
} hTIMER;

typedef enum _hSerCtrlOpt{
    SER_SET_DEBUG,
    SER_SET_PARTIRY,
    SER_SET_STOPBIT,
    SER_SET_RS485TXE,
    SER_CTRL_END
} hSerCtrlOpt;

typedef enum _hCanCtrlOpt{
    CAN_SET_BAUDRATE,
}hCanCtrlOpt;

typedef enum _hPwmCtrlOpt{
    PWM_DUTYSET,
    PWM_DUTYUPDATE,
}hPwmCtrlOpt;

typedef struct _hCanMessage{
    uint32_t id;
    uint8_t data[8];
    uint8_t len;
} hCanMessage;

void hTimInit(void);
void hFlashInit(void);
void hSerInit(void);
void hCanInit(void);
void hPwmInit(void);
void hFlashInit(void);
void hSpiInit(void);

int32_t hLedOpen(void);
void hLedOn(int16_t led);
void hLedOff(int16_t led);
void hLedBlink(int16_t led);
void hLedClose(void);

void hSysInit(void);
void hSysWait(uint32_t msec);
void hSysLock(void);
void hSysUnlock(void);
char *hSysProperty(char *key);

void hOsInit(void);
int32_t hOsCreateThread(hOSThread * thread,void * starFunc,void * param);
int32_t hOsResumeThread(int32_t tid);
int32_t hOsSuspendThread(void);
void hOsStopThread(int32_t tid);
int32_t hOsBiSemaCreate(void);
int32_t hOsBiSemaWait(int32_t sid ,int32_t millsec);
int32_t hOsBiSemaSignal(int32_t sid);
void hOsBisSemaDelete(int32_t sid);
void hOsLock(void);
void hOsUnlock(void);
void hOsRun(void);

int32_t hSerOpen(char *port, int32_t baudrate,__far void  *asyncFunc);
int32_t hSerWrite(int32_t fd, uint8_t* buf, int32_t len);
int32_t hSerRead(int32_t fd, uint8_t *buf, int32_t len);
int32_t hSerCtrl(int32_t fd, int32_t opt, int32_t param);
void hSerClose(int32_t fd);
uint32_t hSerGetHandle(char *port);
int32_t hSerAvailable(int32_t fd);
void hSerFlush(int32_t fd);


int32_t hCanCtrl(int32_t fd, int32_t opt, int32_t param);
int32_t hCanOpen(char *port, int32_t baudrate);
int32_t hCanWrite(int32_t fd, void *buf, int32_t len);
int32_t hCanRead(int32_t fd, void *buf, int32_t len);
void hCanClose(int32_t fd);
uint32_t hCanGetHandle(char *port);
int32_t hCanAvailable(int32_t fd);

void hSwOpen(void);
uint32_t hSwRead(void);
void hSwClose(void);

void hFanOpen(void);
void hFanPowerOn(uint8_t fan);
void hFanPowerOff(uint8_t fan);
void hFanClose(void);
void hFanGpioRead(int *fan1_pw, int *fan2_pw, int *fan1_pw_st, int *fan2_pw_st);


uint32_t hTimCreate(void);
void hTimSet(uint32_t tid, uint32_t usec, __far void *cbfunc);
void hTimStart(uint32_t tid);
void hTimStop(uint32_t tid);
void hTimClose(uint32_t tid);
uint32_t hTimCurrentTime(void);
void hTimDelay(uint32_t usec);


void hFlashWriteEnable(void);
void hFlashWriteDisable(void);
int32_t hFlashProgram(uint32_t address, uint8_t * buf, int32_t len);
int32_t hFlashErase(uint32_t addr, int32_t size);
int32_t hFlashVerify(uint32_t address, uint8_t * buf, int32_t len);

void hBmicOpen(void);
void hBmicShutDown(void);
void hBmicTurnOn(void);
void hBmicEnable(void);
void hBmicDisable(void);
uint8_t hBmicWrite(uint8_t data);
uint32_t hBmicRead(uint32_t addr);
void hBmicGpioRead(int *shutdown);


void hEprOpen(void);
void hEprEnable(void);
void hEprDisable(void);
void hEprWrite(uint32_t addr, uint32_t data);
uint32_t hEprRead(uint32_t addr);
void hEprClose(void);
void hEprGpioRead(int *hold, int *wp);
void hEprGpioWrite(int *hold , int *wp);


int32_t hPwmOpen(char *port );
int32_t  hPwmCtrl(int32_t fd, int32_t opt, int32_t param);
int32_t hPwmStart(int32_t fd);
int32_t hPwmRead(int32_t fd);
int32_t hPwmStop(int32_t fd);
int32_t hPwmGetHandle(char *port);
void hPwmClose(int32_t fd);

int32_t hSpiOpen(char *port, int32_t baudrate);
uint32_t hSpiGetHandle(char *port);
uint8_t hSpi1SendByte(uint8_t data);
uint8_t hSpi1ReceiveByte(void);
uint8_t hSpi2SendByte(uint8_t data);
uint8_t hSpi2ReceiveByte(void);

int32_t hAdcOpen(uint8_t *port, uint32_t chn);
void hAdcStart(int32_t fd);
void hAdcStop(int32_t fd);
void hAdcClose(int32_t fd);
uint32_t hAdcReadData(int32_t fd, uint8_t ch);
uint32_t hAdcGetHandle(uint8_t *port);

uint32_t hLogGetLevel(void);
void hLogProc(uint32_t level, char *fmt, ...);
void hLogSetLevel(uint32_t);

int32_t hI2cOpen(char *port, int32_t baudrate);
void  hI2cSendByte(int fd, uint8_t addr, uint8_t data);
uint8_t hI2cReceiveByte(int fd, uint8_t addr);
int32_t hI2cReadBlock( int fd, uint8_t addr,uint8_t *buf,uint16_t num);

void hBtnOpen(void);
uint32_t hBtnRead(void);

void hWdgOpen(void);
int32_t hWdgRegister(int32_t tid);
void hWdgKick(int32_t tid);



/**
    *  @brief hDiskCtrl 의 Command
    */
typedef enum _hDiskCtrlOpt{
    DISK_GET_SECTORCNT = 1,         /* !< Sector 개수 요청 */
    DISK_GET_SECTORSIZE,              /*!<Sector 의 크기 요청 */
    DISK_GET_BLOCKSIZE                /*!<Block의 크기 요청 */
}hDiskCtrlOpt;

/**
    * @brief    System 초기화시 Disk Device를 사용할수 있도록 Configuration 한다.
    *                
    */
void hDiskInit(void);

/**
    * @brief     SD카드의 Driver의 물리적 장치를 초기화 한다.   
    * @return   성공:0, 실패: ERROR 값
    *                
    */
int32_t hDiskOpen(void);

/**
    * @brief        Disk의 특정 Sector에서 주어진 Block 크기 만큼 읽어서 버퍼에 카피한다.
    *                
    * @param[in]      sector 1~512    
    * @param[out]    buf     Data가 copy 되는 버퍼      
    * @param[in]      count  Copy Block 개수   
    * @return           성공: 0, 실패: ERROR 값 
    */
int32_t hDiskRead(uint32_t sector, uint8_t *buf, uint32_t count);

/**
    * \brief        Disk의 특정 Sector에서 주어진 Block 크기 만큼 버퍼로 부터 Disk로 Write 한다.
    *                
    * @param[in]     sector 1~512    
    * @param[in]     buf    전송될 Data의 버퍼     
    * @param[in]     count  Copy Block 개수   
    * @return           성공: 0, 실패: ERROR 값 
    */
int32_t hDiskWrite(uint32_t sector, uint8_t *buf, uint32_t count);

/**
    * @brief        Disk의 정보 또는 Control 명령을 처리한다.
    *                   만일 입력된 Command가 처리 가 구현 되지 않은 경우 E_NOTSUP  를 반환한다.
    * @param[in]     cmd   command code, HDISK_CTRL 참조
    * @param[out]     buf    Get Command 의 경우 Data를 반환,    Control  Command 인경우  NULL 이 입력
    *                                      DISK_GET_SECTORCNT : 4byte
    *                                      DISK_GET_SECTORSIZE    : 2byte
    *                                      DISK_GET_BLOCKSIZE     : 2byte
    * @return       
    */
int32_t hDiskCtrl(uint8_t cmd, void *buf);    


//#define htrace(l, ...) if (l & hLogGetLevel()) { hLogProc(l, __VA_ARGS__); } else

//#define OLD_REMOTE_IO

#ifdef __cplusplus
}
#endif
#endif //__BMSHAL_H
