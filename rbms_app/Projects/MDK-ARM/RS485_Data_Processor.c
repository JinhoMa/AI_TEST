/*
 * HMI와의 RS485 통신과정을 처리하는 모듈
 * HMI와의 통신(RS485)를 통해 주고받는 패킷을 처리하는 모듈이다.
 * HMI로부터 수신하는 7byte의 명령 패킷을 수신하고 그에 대한 응답을 처리한다.
 */

#include <stdlib.h>
#include "RS485_Data_Processor.h"
#include "EEProm_Control.h"

/*== MACRO ===================================================================*/
#define TEST_485 0

#define DATA_TIMEOUT 60000      // HMI 전원 TIMEOUT (60초)

#define REQ_PACK_SIZE 7
#define FRAME_HEAD1 0x07
#define FRAME_HEAD2 0xE0
#define SLAVE_ADDRESS 0x01

#define KEY_A 0x9164
#define KEY_C 0x1665
#define KEY_E 0x0000FFFF


/*== GLOBAL VARIABLE =========================================================*/
/*== STATIC VARIABLE =========================================================*/
static uint32_t rs485_fd;
static uint16_t security_key;

typedef enum{
    FAULT_ERASE_REQ  = 0xF0,     // FAULT ERASE(ALARM RESET) 요청
    CAPACITY_CNF_REQ = 0xF1,     // Capacity 값 변경 요청
    DATA_REG = 0xFF              // 데이터 요청
} _EN_COMMAND;

typedef enum
{
    PACKET_VALID = 0,            // 패킷이 유효함
    PACKET_ERR_LENGTH,           // 패킷 길이 오류
    PACKET_ERR_HEADER,           // 패킷 헤더 오류
    PACKET_ERR_SLAVE_ADDRESS,    // 슬레이브 주소 오류
    PACKET_ERR_CHECKSUM          // 체크섬 오류
} PacketErrorCode;

static uint8_t req_data[REQ_PACK_SIZE];
static uint8_t rs_cnt = 0;
Response_BMSData_Packet bmsData;

/*== FUNCTION DECLARATION ====================================================*/
static void rs485_packet_handler(uint8_t *commandPacket);
static uint8_t calculateChecksum(uint8_t *packet, uint8_t size);
static void processDataRequest(uint8_t *packet);
static void processFaultEraseRequest(uint8_t *packet);
static void processCapacityConfiguration(uint8_t *packet);
static PacketErrorCode validate_packet(uint8_t *packet, uint8_t size);
static void createResponsePacket(uint8_t *packet, uint8_t successFlag);
static uint16_t calculateKeyByRandomSeed(uint8_t seed);
static uint16_t little_to_big16(uint16_t value);
static uint32_t little_to_big32(uint32_t value);
static uint8_t convertArrayToBitfield(uint8_t *array);

/*== FUNCTION DEFINITION =====================================================*/

static void rs485_packet_handler(uint8_t *commandPacket)
{
    // 명령 코드 확인
    uint8_t commandCode = commandPacket[3];

    // 명령 코드에 따른 처리
    switch (commandCode)
    {
        case DATA_REG:          // 데이터 요청 명령
            processDataRequest(commandPacket);
            break;

        case FAULT_ERASE_REQ:   // Fault Erase Request 명령
            processFaultEraseRequest(commandPacket);
            break;

        case CAPACITY_CNF_REQ:  // Capacity Configuration Request 명령
            processCapacityConfiguration(commandPacket);
            break;

        default:                // 알 수 없는 명령 코드 - 패킷 무시
            break;
    }
}


// 체크섬 계산 함수
static uint8_t calculateChecksum(uint8_t *packet, uint8_t size)
{
    uint8_t checksum = 0;
    for (int i = 0; i < size; i++)
    {
        checksum ^= packet[i];
    }
    return checksum;
}

// 데이터 요청 처리 함수
static void processDataRequest(uint8_t *packet)
{
    // 데이터 요청 명령일 경우 프레임 데이터가 0x00, 0x00인지 확인
    if (packet[4] == 0x00 && packet[5] == 0x00)
    {
        //데이터 요청에 대한 처리 로직
        PushJob(EXTCOM_SCHEDULER_IDX, EXTCOM_RS485_OUT, 0, NULL);
    }
    else
    {
        // 잘못된 프레임 데이터 - 패킷 무시
        return;
    }
}

// Fault Erase 처리 함수
static void processFaultEraseRequest(uint8_t *packet)
{
    uint16_t receivedKey = (packet[4] << 8) | packet[5];
    if(security_key == receivedKey)   // 성공
    {
        PushJob( EEPROM_SCHEDULER_IDX,  CLEAR_FAULT_RECORD , 0, 0);
        Diag_ErrorReset();
        DTC_SM.reason_code=0;
        createResponsePacket(packet, COMMAND_SUCCESS);
    }
    else    // 실패
    {
        createResponsePacket(packet, COMMAND_FAIL);
    }
}

// Capacity 설정 요청 처리 함수
static void processCapacityConfiguration(uint8_t *packet)
{
    if(1)   // 성공
    {
        // Capacity 값 설정
        EEPROMData.soccnf.cell_100mA = (packet[4] << 8) | packet[5];
        EEprom_WriteBuf(EEP_SOC_CELLCAP, (uint8_t*)&EEPROMData.soccnf.cell_100mA, 2);
        EEPROMData.soccnf.total_eng = 3.2*EEPROMData.soccnf.cell_100mA *TRAY_CELL_NUMS/10 ;
        EEprom_WriteBuf(EEP_SOC_TOTALENG, (uint8_t*)&EEPROMData.soccnf.total_eng, 2);
        EEPROMData.soccnf.total_rc = EEPROMData.soccnf.cell_100mA*3600;
        EEprom_WriteBuf(EEP_SOC_TOTALCAP, (uint8_t*)& EEPROMData.soccnf.total_rc, 4);
        EEprom_ReadBuf(EEP_SOC_TOTALCAP, (uint8_t*)& EEPROMData.soccnf.total_rc, 4);
        RackSt_SaveCurrentStAtOnce(F_RCCHG);
        createResponsePacket(packet, COMMAND_SUCCESS);
    }
    else    // 실패
    {
        createResponsePacket(packet, COMMAND_FAIL);
    }
}

static PacketErrorCode validate_packet(uint8_t *packet, uint8_t size)
{
    // 패킷 길이 검증
    if (size < REQ_PACK_SIZE)
    {
        return PACKET_ERR_LENGTH;
    }

    // 패킷 헤더 검증
    if (packet[0] != FRAME_HEAD1 || packet[1] != FRAME_HEAD2)
    {
        return PACKET_ERR_HEADER;
    }

    // 슬레이브 주소 검증
    if (packet[2] != SLAVE_ADDRESS)
    {
        return PACKET_ERR_SLAVE_ADDRESS;
    }

    // 체크섬 검증
    uint8_t calculatedChecksum = calculateChecksum(packet, REQ_PACK_SIZE - 1);
    if (calculatedChecksum != packet[REQ_PACK_SIZE - 1])
    {
        return PACKET_ERR_CHECKSUM;
    }

    // 모든 검증 통과
    return PACKET_VALID;
}

static uint16_t calculateKeyByRandomSeed(uint8_t seed)
{
    return (uint16_t)(((seed * KEY_A) + KEY_C) & KEY_E);
}

PUBLIC __WEAK void rs485_rdpacket_process(void)
{
    uint8_t discardBuf; // 초과된 데이터를 임시 저장하고 버리기 위한 버퍼
    uint8_t cnt = hSerAvailable(rs485_fd);
    if(cnt == 0) return;

    //
    if( (rs_cnt + cnt) < REQ_PACK_SIZE ){
        hSerRead(rs485_fd, req_data+rs_cnt, cnt);
        rs_cnt += cnt;
        return;
    }else{
        // 패킷 사이즈에서 여유있는 만큼 Read
        uint8_t remainingBytes = REQ_PACK_SIZE - rs_cnt;                        // 패킷 사이즈에서 여유 Byte 수
        if (remainingBytes > 0)
        {
            hSerRead(rs485_fd, req_data + rs_cnt, remainingBytes);
            rs_cnt += remainingBytes;
        }

        // 패킷 사이즈를 초과한 Byte만큼 읽어서 discardBuf에 담은 후 버림
        if (hSerAvailable(rs485_fd) > 0)
        {
            while (hSerAvailable(rs485_fd) != 0)
            {
                hSerRead(rs485_fd, &discardBuf, 1);  // 초과된 바이트를 하나씩 처리
            }
        }

        // 패킷 헤더, 패킷 사이즈, 체크섬을 검증하여 유효한 패킷인지 확인
        PacketErrorCode errorCode = validate_packet(req_data, REQ_PACK_SIZE);
        if (errorCode != PACKET_VALID)
        {
            hLogProc(LOG_ERROR, "RS485 Packet Error(Error code : %d)\n", errorCode);
            rs_cnt = 0;                                                     // 잘못된 패킷 처리 후 READ 카운트 초기화
            return;
        }
        rs485_packet_handler(req_data);
        rs_cnt = 0;                                 // 패킷 처리 후 READ 카운트 초기화
    }
}

PUBLIC __WEAK void rs485_wrpacket_process(void)
{
    int i;
    int err_table;
    hSerCtrl(rs485_fd, SER_SET_RS485TXE, 1);
    bmsData.str.header = little_to_big16((FRAME_HEAD1 << 8) | FRAME_HEAD2);
    bmsData.str.addr = 0x01;
    bmsData.str.command = DATA_REG;
    bmsData.str.BV = little_to_big16(R.StringModel.BatV);
    bmsData.str.PV = little_to_big16(R.StringModel.V * 10);
    bmsData.str.C =  little_to_big16(R.StringModel.A * 10);
    bmsData.str.P =  little_to_big16((R.StringModel.V / 10) * (R.StringModel.A / 10));
    bmsData.str.SOC = little_to_big16(R.StringModel.SoC / 10);
    for(i= 0; i< 16; i++)
    {
        if(i < TRAY_CELL_NUMS)
        {
            bmsData.str.CV[i] = little_to_big16(RbmsComTray[0].cellvol[i]);
        }else{
            bmsData.str.CV[i]= 0;
        }
    }
    bmsData.str.ExtT1 = RbmsComTray[0].tmp[0] / 100;
    bmsData.str.ExtT2 = RbmsComTray[0].tmp[1] / 100;
    bmsData.str.ExtT3 = RbmsComTray[0].tmp[2] / 100;
    bmsData.str.ExtT4 = 0;
    bmsData.str.BdT1 = RbmsComTray[0].brdtmp[0] / 100;
    bmsData.str.BdT2 = RbmsComTray[0].brdtmp[1] / 100;
    bmsData.str.PackST = Rack_St;
    bmsData.str.CellBal = little_to_big16(convertArrayToBitfield(RbmsComTray[0].cellbal));
    bmsData.str.StrEvt1 = little_to_big32(R.StringModel.Evt1);
    bmsData.str.StrEvt2 = little_to_big32(R.StringModel.Evt2);
    err_table = (uint16_t)ConvertDTCTbleToErrTble(EEPROMData.evlog.dtc, EEPROMData.evlog.sub_code );
    bmsData.str.ERRORCODE = little_to_big16(err_table);
    bmsData.str.Capacity = little_to_big16(EEPROMData.soccnf.cell_100mA);
    bmsData.str.Reserved1 = 0xFF;
    bmsData.str.chksum =calculateChecksum(bmsData.buffer, sizeof(Response_BMSData_Packet)-1);
    bmsData.str.Reserved2 = 0xFF;
    hSerWrite(rs485_fd, bmsData.buffer, sizeof(Response_BMSData_Packet));
    hSysWait(200);
    hSerCtrl(rs485_fd, SER_SET_RS485TXE, 0);
}

static void createResponsePacket(uint8_t *packet, uint8_t successFlag)
{
    // 프레임 데이터 부분만 성공 여부로 변경 (1: 성공, 2: 실패)
    packet[4] = 0x00;
    packet[5] = successFlag & 0xFF;

    // 체크섬 재계산 및 추가
    uint8_t checksum = calculateChecksum(packet, 6);
    packet[6] = checksum;
    hSerCtrl(rs485_fd, SER_SET_RS485TXE, 1);
    hSysWait(1);
    // 응답 패킷 전송
    hSerWrite(rs485_fd, packet, REQ_PACK_SIZE);
    hSysWait(250);
    hSerCtrl(rs485_fd, SER_SET_RS485TXE, 0);

}
PUBLIC __WEAK void RS485Init(void)
{
    hSerOpen("COM2",9600, 0);
    rs485_fd = hSerGetHandle("COM2");
    hSerCtrl(rs485_fd, SER_SET_RS485TXE, 0);
    srand((int32_t)hTimCurrentTime());
    bmsData.str.Random_Seed = rand()%10+1;
    security_key = calculateKeyByRandomSeed(bmsData.str.Random_Seed);
}

uint16_t little_to_big16(uint16_t value)
{
    return (value >> 8) | (value << 8);
}

uint32_t little_to_big32(uint32_t value)
{
    return ((value >> 24) & 0xFF) |
           ((value >> 8) & 0xFF00) |
           ((value << 8) & 0xFF0000) |
           ((value << 24) & 0xFF000000);
}

static uint8_t convertArrayToBitfield(uint8_t *array)
{
    uint8_t bitfield = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (array[i] == 1)
        {
            bitfield |= (1 << i);
        }
    }
    return bitfield;
}
