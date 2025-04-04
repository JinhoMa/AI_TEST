#include <stdint.h>
#include "main.h"
#include "oMbSlave.h"
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"
#include <stdio.h>


enum {
 OMB_IDLE,
 OMB_READY,
 OMB_ERROR,
 OMB_DONE,
} OMBSTATE;

typedef struct {
    uint8_t mode;
    uint32_t port;
    uint32_t rtubaudrate;
    int32_t sema;
    uint16_t *data;
    uint16_t dsize;
    int32_t state;
    int32_t timeout;
	int32_t mb_id;
	int32_t used;
    uint16_t    usSDiscInStart ;        //                         = S_DISCRETE_INPUT_START;
    uint8_t    *ucSDiscInBuf  ;
    uint32_t    ucDiscInSize ;
    //Slave mode:Coils variables
    uint16_t    usSCoilStart;           //                           = S_COIL_START;
    uint8_t     *ucSCoilBuf;
    uint32_t    ucCoilSize;
    //Slave mode:InputRegister variables
    uint16_t    usSRegInStart;          //                           = S_REG_INPUT_START;
    uint16_t    *usSRegInBuf ;
    uint32_t    ucInRegSize;
    //Slave mode:HoldingRegister variables
    uint16_t    usSRegHoldStart;        //                            = S_REG_HOLDING_START;
    uint16_t    *usSRegHoldBuf;
    uint32_t    ucHoldRegSize;
    void (*cbFunc)(uint8_t *, uint16_t , uint16_t, uint32_t mode);
} oMbSlave_;

static hOSThread mbmaster_thread;
static uint32_t mbslavertu_tid;
static uint32_t mbslavetcp_tid;
static void mbRtuTask (void *param);
static void mbTcpTask (void *param);
oMbSlave_ oMbSlave[MBSLAVEn];

int oMbSlaveErrCode;
int oMbSlaveErrCount;
extern eMBErrorCode eMBTcpPoll( LONG SlaveID );

static oMbSlave_ *allocMbSlave(){
	int i;
    for(i=0;i<MBSLAVEn;i++){
       if(oMbSlave[i].used == 0){
        oMbSlave[i].used = 1;
        return &oMbSlave[i];
	   }
    }
    return NULL;
}
void freeMbSlave(int32_t slave_id){
	oMbSlave_ *s = (oMbSlave_ *) slave_id;
	s->used = 0;
}
static uint32_t getMbSlaveId(uint32_t port)
{
    int i;
    for(i=0;i<MBSLAVEn;i++){
        if(oMbSlave[i].used && oMbSlave[i].port == port){
            return (uint32_t)&oMbSlave[i];
        }
    }
    return 0;
}
/**
   *********************************************************************************************************************************
   * @brief Modbus Slave�� Open�Ѵ�.
   * @param [in] mode   OMB_RTU, OMB_TCP �߿� 1���̴�.
   * @param [in] address             Slave�� ��ȣ�̴�. �ý��ۿ��� Switch�� ���ؼ� ���۵ǰų�, ���ֹ߼��޸𸮿� ����ȴ�. Master 1����
   �ش� Slave �� Uniq �ؾ� �Ѵ�.
   * @param [in] port                OMB_RTU �� ��� COM1(0), COM2(1) .. �� ���� ������. OMB_TCP�� ��� Bind�ϴ� Port ���� ������.
   * @param [in] baudrate            OMB_RTU�� ��� �ø��� �ӵ� �̴�. OMB_TCP�� ��� �� ���� ���õȴ�.
   * @return ������ ��� ������ ��ȯ�ȴ�. ������ ��� slave�� id �� ��ȯ�ȴ�.
   *********************************************************************************************************************************
   **/
int32_t oMbSlaveOpen(uint32_t mode, uint8_t address, uint32_t port, uint32_t baudrate)
{
	int32_t ret= E_ERROR;
	int32_t mb_id;
	oMbSlave_ *s;
	eMBErrorCode  eCode;
	switch(mode){
		
	   case OMB_RTU:
		eCode = eMBInit(&mb_id, MB_RTU, address, port, baudrate,  8 ) ;
		break;
	   case OMB_TCP:
#if MB_SLAVE_TCP_ENABLED > 0
		eCode = eMBTCPInit(&mb_id, port);
#endif
		break;
	}
	if(!eCode){
		s = allocMbSlave();
		if(s == NULL ) {ret = E_ERROR; return ret;}
		s->mb_id = mb_id;
        s->port = port;
        s->mode = mode;
		ret = (int32_t)s;
	}
	
	return ret;
}




PUBLIC void oMbSlaveClose(int32_t slave_id)
{
	freeMbSlave(slave_id);
}

PUBLIC void oMbSlaveStart(int32_t slave_id)
{
	oMbSlave_ *s = (oMbSlave_*)slave_id;
    eMBEnable(s->mb_id);
}

PUBLIC void oMbSlavePoll(int32_t slave_id)
{
    oMbSlave_ *s = (oMbSlave_*)slave_id;
    switch (s->mode)
    {
        case OMB_RTU:
            oMbSlaveErrCode = eMBRTUPoll(s->mb_id);
	        if (oMbSlaveErrCode != 0)
            {
	            oMbSlaveErrCount++;
	        }
            break;
        case OMB_TCP:
            eMBTcpPoll(s->mb_id);
            break;
    }
}

/* -----------------------Slave Defines -------------------------------------*/
#define S_DISCRETE_INPUT_START        0
#define S_DISCRETE_INPUT_NDISCRETES   16
#define S_COIL_START                  0
#define S_COIL_NCOILS                 64
#define S_REG_INPUT_START             0
#define S_REG_INPUT_NREGS             100
#define S_REG_HOLDING_START           0
#define S_REG_HOLDING_NREGS           100
/* salve mode: holding register's all address */
#define          S_HD_RESERVE                     0
#define          S_HD_CPU_USAGE_MAJOR             1
#define          S_HD_CPU_USAGE_MINOR             2
/* salve mode: input register's all address */
#define          S_IN_RESERVE                     0
/* salve mode: coil's all address */
#define          S_CO_RESERVE                     0
/* salve mode: discrete's all address */
#define          S_DI_RESERVE                     0

/*------------------------Slave mode use these variables----------------------*/
//Slave mode:DiscreteInputs variables

/**
   *********************************************************************************************************************************
   * @brief  Application ���� ����ϴ� Coils �޸𸮸� ����Ѵ�.  Modbus Master ����  Coils���� READ, WRITE  Function
   *  �� ȣ��ɶ� ���ٵǴ� �޸��̴�.
   * @param [in] sid   Modbus Slave �� Id, oMbSlaveOpen()�� ���ؼ� �Ҵ�ȴ�.
   * @param [in] ptr    �޸��� ������
   * @param [in] size  �޸���  size �̴�.
   *********************************************************************************************************************************
   **/

PUBLIC void oMbSlaveRegCoilsMemory(int32_t sid, uint8_t *ptr, uint32_t size)
{
    oMbSlave_ *s = (oMbSlave_ *)sid;
    s->ucSCoilBuf = ptr;
    s->ucCoilSize  = size;
}
/**
   *********************************************************************************************************************************
   * @brief  Application ���� ����ϴ� DescreteInput  �޸𸮸� ����Ѵ�.  Modbus Master ����  DescreteInput ���� READ, WRITE  Function
   *  �� ȣ��ɶ� ���ٵǴ� �޸��̴�.
   * @param [in] sid   Modbus Slave �� Id, oMbSlaveOpen()�� ���ؼ� �Ҵ�ȴ�.
   * @param [in] ptr    �޸��� ������
   * @param [in] size  �޸���  size �̴�.
   *********************************************************************************************************************************
   **/
PUBLIC void oMbSlaveRegDescretInputMemory(int32_t sid, uint8_t *ptr, uint32_t size)
{
    oMbSlave_ *s = (oMbSlave_ *)sid;
    s->ucSDiscInBuf = ptr;
    s->ucDiscInSize = size;
}

/**
   *********************************************************************************************************************************
   * @brief  Application ���� ����ϴ� HoldingRegister  �޸𸮸� ����Ѵ�.  Modbus Master ����  HoldingRegister ���� READ, WRITE  Function
   *  �� ȣ��ɶ� ���ٵǴ� �޸��̴�.
   * @param [in] sid   Modbus Slave �� Id, oMbSlaveOpen()�� ���ؼ� �Ҵ�ȴ�.
   * @param [in] ptr    �޸��� ������
   * @param [in] size  �޸���  size �̴�.
   *********************************************************************************************************************************
   **/

PUBLIC void oMbSlaveRegHoldingRegMemory(int32_t sid, uint16_t *ptr, uint32_t size)
{
    oMbSlave_ *s = (oMbSlave_ *)sid;
    s->usSRegHoldBuf = ptr;
    s->ucHoldRegSize = size;
    
}
/**
   *********************************************************************************************************************************
   * @brief  Application ���� ����ϴ� InputRegister  �޸𸮸� ����Ѵ�.  Modbus Master ����  InputRegister ���� READ, WRITE  Function
   *  �� ȣ��ɶ� ���ٵǴ� �޸��̴�.
   * @param [in] sid   Modbus Slave �� Id, oMbSlaveOpen()�� ���ؼ� �Ҵ�ȴ�.
   * @param [in] ptr    �޸��� ������
   * @param [in] size  �޸���  size �̴�.
   *********************************************************************************************************************************
   **/
PUBLIC void oMbSlaveRegInputRegMemory(int32_t sid, uint16_t *ptr, uint32_t size)
{
    oMbSlave_ *s = (oMbSlave_ *)sid;
    s->usSRegInBuf = ptr;
    s->ucInRegSize = size;
}


eMBErrorCode eMBRegInputCB( ULONG port,  UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex;
    USHORT *        pusRegInputBuf;
    USHORT          REG_INPUT_START;
    USHORT          REG_INPUT_NREGS;
    USHORT          usRegInStart;
    oMbSlave_ *s = (oMbSlave_ *)getMbSlaveId(port);
    pusRegInputBuf = s->usSRegInBuf;
    REG_INPUT_START = S_REG_INPUT_START;
    REG_INPUT_NREGS = s->ucInRegSize;
    usRegInStart = s->usSRegInStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= REG_INPUT_START)
            && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
    {
        iRegIndex = usAddress - usRegInStart;
        while (usNRegs > 0)
        {
            *pucRegBuffer++ = (UCHAR) (pusRegInputBuf[iRegIndex] >> 8);
            *pucRegBuffer++ = (UCHAR) (pusRegInputBuf[iRegIndex] & 0xFF);
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

/**
 * Modbus slave holding register callback function.
 *
 * @param pucRegBuffer holding register buffer
 * @param usAddress holding register address
 * @param usNRegs holding register number
 * @param eMode read or write
 *
 * @return result
 */
 
USHORT          iRegIndex;
USHORT *        pusRegHoldingBuf;
USHORT          REG_HOLDING_START;
USHORT          REG_HOLDING_NREGS;
USHORT          usRegHoldStart;

eMBErrorCode eMBRegHoldingCB(ULONG port,  UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    oMbSlave_ *s = (oMbSlave_ *)getMbSlaveId(port);
    pusRegHoldingBuf = s->usSRegHoldBuf;
    REG_HOLDING_START = S_REG_HOLDING_START;
    REG_HOLDING_NREGS = s->ucHoldRegSize;
    usRegHoldStart = s->usSRegHoldStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= REG_HOLDING_START)
            && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
        iRegIndex = usAddress - usRegHoldStart;
        switch (eMode)
        {
        /* read current register values from the protocol stack. */
        case MB_REG_READ:
            while (usNRegs > 0)
            {
                *pucRegBuffer++ = (UCHAR) (pusRegHoldingBuf[iRegIndex] >> 8);
                *pucRegBuffer++ = (UCHAR) (pusRegHoldingBuf[iRegIndex] & 0xFF);
                iRegIndex++;
                usNRegs--;
            }
            break;

        /* write current register values with new values from the protocol stack. */
        case MB_REG_WRITE:
            while (usNRegs > 0)
            {
                pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**
 * Modbus slave coils callback function.
 *
 * @param pucRegBuffer coils buffer
 * @param usAddress coils address
 * @param usNCoils coils number
 * @param eMode read or write
 *
 * @return result
 */
eMBErrorCode eMBRegCoilsCB( ULONG port, UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNCoils, eMBRegisterMode eMode)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    UCHAR *         pucCoilBuf;
    USHORT          COIL_START;
    USHORT          COIL_NCOILS;
    USHORT          usCoilStart;
    oMbSlave_ *s = (oMbSlave_ *)getMbSlaveId(port);
    
    iNReg =  usNCoils / 8 + 1;

    pucCoilBuf = s->ucSCoilBuf;
    COIL_START = S_COIL_START;
    COIL_NCOILS = s->ucCoilSize;
    usCoilStart = s->usSCoilStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if( ( usAddress >= COIL_START ) &&
        ( usAddress + usNCoils <= COIL_START + COIL_NCOILS ) )
    {
        iRegIndex = (USHORT) (usAddress - usCoilStart) / 8;
        iRegBitIndex = (USHORT) (usAddress - usCoilStart) % 8;
        switch ( eMode )
        {
        /* read current coil values from the protocol stack. */
        case MB_REG_READ:
            while (iNReg > 0)
            {
                *pucRegBuffer++ = xMBUtilGetBits(&pucCoilBuf[iRegIndex++],
                        iRegBitIndex, 8);
                iNReg--;
            }
            pucRegBuffer--;
            /* last coils */
            usNCoils = usNCoils % 8;
            /* filling zero to high bit */
            *pucRegBuffer = *pucRegBuffer << (8 - usNCoils);
            *pucRegBuffer = *pucRegBuffer >> (8 - usNCoils);
            break;

            /* write current coil values with new values from the protocol stack. */
        case MB_REG_WRITE:
            while (iNReg > 1)
            {
                xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, 8,
                        *pucRegBuffer++);
                iNReg--;
            }
            /* last coils */
            usNCoils = usNCoils % 8;
            /* xMBUtilSetBits has bug when ucNBits is zero */
            if (usNCoils != 0)
            {
                xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, usNCoils,
                        *pucRegBuffer++);
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**
 * Modbus slave discrete callback function.
 *
 * @param pucRegBuffer discrete buffer
 * @param usAddress discrete address
 * @param usNDiscrete discrete number
 *
 * @return result
 */
eMBErrorCode eMBRegDiscreteCB( ULONG port, UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    UCHAR *         pucDiscreteInputBuf;
    USHORT          DISCRETE_INPUT_START;
    USHORT          DISCRETE_INPUT_NDISCRETES;
    USHORT          usDiscreteInputStart;
    
    oMbSlave_ *s = (oMbSlave_ *)getMbSlaveId(port);
    iNReg =  usNDiscrete / 8 + 1;

    pucDiscreteInputBuf = s->ucSDiscInBuf;
    DISCRETE_INPUT_START = S_DISCRETE_INPUT_START;
    DISCRETE_INPUT_NDISCRETES = s->ucDiscInSize;
    usDiscreteInputStart = s->usSDiscInStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= DISCRETE_INPUT_START)
            && (usAddress + usNDiscrete    <= DISCRETE_INPUT_START + DISCRETE_INPUT_NDISCRETES))
    {
        iRegIndex = (USHORT) (usAddress - usDiscreteInputStart) / 8;
        iRegBitIndex = (USHORT) (usAddress - usDiscreteInputStart) % 8;

        while (iNReg > 0)
        {
            *pucRegBuffer++ = xMBUtilGetBits(&pucDiscreteInputBuf[iRegIndex++],
                    iRegBitIndex, 8);
            iNReg--;
        }
        pucRegBuffer--;
        /* last discrete */
        usNDiscrete = usNDiscrete % 8;
        /* filling zero to high bit */
        *pucRegBuffer = *pucRegBuffer << (8 - usNDiscrete);
        *pucRegBuffer = *pucRegBuffer >> (8 - usNDiscrete);
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

/**
   *********************************************************************************************************************************
   * @brief Function Code 1
   * @param [in] slav_id   Modbus Slave �� Id, oMbSlaveOpen()�� ���ؼ� �Ҵ�ȴ�.
   * @param [in] cFunc	   Callback Function �� ����Ѵ�. Master�� ���� Coils Func�� �߻��� ��� ����� Callback Function�� ȣ��ȴ�.      
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbSlaveRegCoils(int slave_id, void *cFunc)
{
	int32_t ret = E_ERROR;
	oMbSlave_ *s= (oMbSlave_ *)slave_id;

	s->cbFunc = ( void (*)(uint8_t*, uint16_t, uint16_t, uint32_t))cFunc;
    return ret;

}
/**
   *********************************************************************************************************************************
   * @brief Function Code 1
   * @param [in] slav_id   Modbus Slave �� Id, oMbSlaveOpen()�� ���ؼ� �Ҵ�ȴ�.
   * @param [in] cFunc	   Callback Function �� ����Ѵ�. Master�� ���� Coils Func�� �߻��� ��� ����� Callback Function�� ȣ��ȴ�.      
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbSlaveRegDisInput(uint32_t slave_id, void *cFunc)
{
	int32_t ret = E_ERROR;
	oMbSlave_ *s= (oMbSlave_ *)slave_id;
	s->cbFunc = ( void (*)(uint8_t*, uint16_t, uint16_t, uint32_t))cFunc;
    return ret;

}
/**
   *********************************************************************************************************************************
   * @brief Function Code 1
   * @param [in] slav_id   Modbus Slave �� Id, oMbSlaveOpen()�� ���ؼ� �Ҵ�ȴ�.
   * @param [in] cFunc	   Callback Function �� ����Ѵ�. Master�� ���� Coils Func�� �߻��� ��� ����� Callback Function�� ȣ��ȴ�.      
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbSlaveRegHoldings(uint32_t slave_id, void *cFunc)
{
	int32_t ret= E_ERROR;
	oMbSlave_ *s= (oMbSlave_ *)slave_id;
	s->cbFunc = ( void (*)(uint8_t*, uint16_t, uint16_t, uint32_t))cFunc;
    return ret;

}

/**
   *********************************************************************************************************************************
   * @brief Function Code 1
   * @param [in] slav_id   Modbus Slave �� Id, oMbSlaveOpen()�� ���ؼ� �Ҵ�ȴ�.
   * @param [in] cFunc	   Callback Function �� ����Ѵ�. Master�� ���� Coils Func�� �߻��� ��� ����� Callback Function�� ȣ��ȴ�.      
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbSlaveRegInputRegs(uint32_t slave_id, void *cFunc)
{
	int32_t ret = E_ERROR;
	oMbSlave_ *s= (oMbSlave_ *)slave_id;
	s->cbFunc = ( void (*)(uint8_t*, uint16_t, uint16_t, uint32_t))cFunc;
    return ret;

}

