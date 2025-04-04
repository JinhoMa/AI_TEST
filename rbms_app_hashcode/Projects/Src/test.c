/**
 ******************************************************************************
 *     @file         test.c
 *     @version   
 *     @author    
 *     @date      2024/10/12 
 *     @brief     
 ******************************************************************************
*/
/**
 ******************************************************************************
 *     @file         test.c
 *     @version   
 *     @author    
 *     @date      2024/10/12 
 *     @brief     
 ******************************************************************************
*/
/**
 ******************************************************************************
 *     @file         test.c
 *     @version   
 *     @author    
 *     @date      2024/10/12 
 *     @brief     
 ******************************************************************************
*/
/**
 ******************************************************************************
 *     @file         test.c
 *     @version   
 *     @author    
 *     @date      2024/10/12 
 *     @brief     
 ******************************************************************************
*/
/**
 ******************************************************************************
 *     @file         test.c
 *     @version   
 *     @author    
 *     @date      2024/10/11 
 *     @brief     
 ******************************************************************************
*/
#include "main.h"
#include "obshal.h"
#include <stdio.h>
#include <string.h>
#

#define CAN_TEST         1
#define EEPROM_TEST      1

void can_write_test(void)
{
    hCanMessage msg;
    msg.id = 0x7F1;
    msg.data[0] = 0x1;
    msg.len = 8;
    while (1)
    {
        hCanWrite(hCanGetHandle("CANB"), &msg, 1);
        hSysWait(1000);
    }
}

void can_read_test(void)
{
    int ret;
    hCanMessage msg;
    while (1)
    {
        ret = hCanRead(hCanGetHandle("CAN1"), &msg,1);
        if (ret > 0)
        {
            printf("canid=%x, data[0]=%x\n", msg.id, msg.data[0]);
        }
        hSysWait(10);
    }
}

#if (EEPROM_TEST == 1)
uint8_t testbuf[1000];
uint8_t verify[1000];
/**********************************************************
     @brief eeprom 에 byte 단위로 write 하는 경우와 array 단위로 write 하는 경우
     의 시간 비교 및 write 값과 read 값을 비교하여 verification 한다.
**********************************************************/
     
void eeprom_test(void)
{
    int i, TEST;
    uint64_t p, t;
    
    hLedOff(LED1);
    hLedOff(LED2);
    hLedOff(LED3);
    for (i = 0; i < 1000; i++)
    {
    testbuf[i] = i%256;
        hEprEnable();
        hEprWriteByte(i, 0xff);
        hEprDisable();
    }
    printf("-------------------------------------\n");
    printf("            write test\n");    
    printf("-------------------------------------\n");
    for (TEST = 100; TEST <= 1000; TEST += 100)
    {
        hLedBlink(LED2);
        p = hTimCurrentTime();
        for (i = 0; i < TEST; i++)
        {
        hEprEnable();
        hEprWriteByte(i, testbuf[i]);
        hEprDisable();
        }
        t = hTimCurrentTime() - p;
        printf(" %d byte time= (%d) \n", TEST, (uint32_t)t);
        
        for (i = 0; i < TEST; i++)
        {
            verify[i] = hEprReadByte(i);
            if (verify[i] != testbuf[i])
            {
                printf("byte error(%d)d\n", i);
                hLedOn(LED3);
                break;
            }
        }

        p = hTimCurrentTime();
        hEprEnable();
        hEprWrite(i, testbuf, 1000);
        hEprDisable();
        t = hTimCurrentTime() - p;
        printf(" %d array time= (%d) \n",TEST, (uint32_t)t);
        for (i = 0; i < TEST; i++)
        {
            verify[i] = hEprReadByte(i);
            if (verify[i] != testbuf[i])
            {
                printf("array error(%d)\n", i);
                hLedOn(LED3);
                break;
            }
        }
    }

    printf("-------------------------------------\n");
    printf("            read test\n");    
    printf("-------------------------------------\n");
    for (TEST = 100; TEST <= 1000; TEST += 100)
    {
        hLedBlink(LED2);
        p = hTimCurrentTime();
        for (i = 0; i < TEST; i++)
        {
            testbuf[i] = hEprReadByte(i);
        }
        t = hTimCurrentTime() - p;
        printf(" %d byte time= (%d) \n", TEST, (uint32_t)t);
        
        p = hTimCurrentTime();
        hEprRead(0, verify, TEST);
        t = hTimCurrentTime() - p;
        printf(" %d array time= (%d) \n", TEST, (uint32_t)t);
        for (i = 0; i < TEST; i++)
        {
            verify[i] = hEprReadByte(i);
            if (verify[i] != testbuf[i])
            {
                printf("array error(%d)\n", i);
                hLedOn(LED3);
                break;
            }
        }
    }
    for (i = 0; i < 1000; i++)
    {
        hEprEnable();
        hEprWriteByte(i, 0xff);
        hEprDisable();
   }
    for (i = 0; i < 1000; i++)
    {
        testbuf[i] = hEprReadByte(i);
    }
    hLedOn(LED1);
    while(1);

}

#endif

void rs485_test(void)
{
    // 통신 메모리 맵에 맞춰 데이터를 구성합니다.
    uint8_t memory_map_data[90];

	uint8_t test_data1[5] = {0x01, 0x02, 0x03,0x04, 0x05};
    uint8_t test_data2[8]={'1','2','3','4','5','6','7','8'};
    // RS485 포트 핸들 가져오기
    uint32_t fd = hSerGetHandle(hSysProperty("rs485.port"));
	// 송신 제어 (TXE 활성화)
    hSerCtrl(fd, SER_SET_RS485TXE, 1);
    while (1)
    {
        // 데이터 송신
        memcpy(memory_map_data, (uint8_t[]){
        0xA5, 0x5A,       // 프레임 헤더
        0x5D,             // 프레임 길이
        0x82,             // 명령 코드
        0x10, 0x00,       // 출발 주소
        0x6D, 0xC0,       // 배터리 전압 (예: 28000mV)
        0xFF, 0xEC,       // 배터리 전류 (예: -20A)
        0x00, 0x00,       // 예약
        0x00, 0x50,       // 남은 전력 (예: 80%)
        0x00, 0x10,       // 최대 압력차 (예: 16mV)
        0x00, 0x1D,       // 모스 온도 (예: 29℃)
        0x00, 0x1E,       // 배터리 온도 (예: 30℃)
        0x00, 0x00,       // 시스템 경고 (알람 없음)
        0x0D, 0xAC,       // 셀 평균 전압 (예: 3500mV)
        0x00, 0x01,       // 밸런스 스위치 상태 (열림)
        0x00, 0x00,       // 요금 모스 상태 (폐쇄)
        0x00, 0x00,       // 해고 모스 상태 (폐쇄)
        // 셀 전압 데이터 (예: 일부 수정됨)
        0x0D, 0xAF, 0x0D, 0xA0, 0x0D, 0xAC, 0x0D, 0xAC,
        0x0D, 0xAE, 0x0D, 0xAB, 0x0D, 0xAC, 0x0D, 0xAD,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,       // 단일 장치 과방 전 경보 (알람 없음)
        0x00, 0x00,       // 셀 과충전 경보 (알람 없음)
        0x00, 0x00,       // 과전류 경보 (알람 없음)
        0x00, 0x00,       // 모스 과열 경보 (알람 없음)
        0x00, 0x00,       // 배터리 과열 경보 (알람 없음)
        0x00, 0x00,       // 단락 경보 (알람 없음)
        0x00, 0x00,       // 공동 처리 통신 예외 (알람 없음)
        0x00, 0x00,       // 밸런스 라인 저항이 너무 큽니다 (알람 없음)
        0x00, 0x00        // 문자열 개수 불일치 (알람 없음)
    }, sizeof(memory_map_data));

    hSerWrite(fd, memory_map_data, 90);
    // 100ms 대기 후 반복
    hSysWait(100);
    }
	// 송신 제어 (TXE 비활성화)
    hSerCtrl(fd, SER_SET_RS485TXE, 0);
}

int buzzer_clk= 1;
int prev_buzzer_clk;
int buz_id;
void buzzer_onoff(void)
{
    static uint8_t toggle;
    toggle ^= 1;
    if (toggle)
    {
        BUZ_ON() ;
    }
    else
    {
        BUZ_OFF();
    }
    if (buzzer_clk != prev_buzzer_clk)
    {
        prev_buzzer_clk = buzzer_clk;
        hTimSet(buz_id, buzzer_clk, buzzer_onoff);
    }
}

void pwm_test(void)
{
    int i;
    buz_id = hTimCreate();
    hTimSet(buz_id, buzzer_clk, buzzer_onoff);
	
    // for(int j = 1; j < 6; j++)
    // {
    //     hTimSet(buz_id, j, buzzer_onoff);
    //     for (i = 0; i < 5; i++)
    //     {
    //         hTimStart(buz_id);
    //         hSysWait(300);
    //         hTimStop(buz_id);
    //         hSysWait(1000);
    //     }
    // }

    // for(int j = 1; j < 6; j++)
    // {
    hTimSet(buz_id, 1, buzzer_onoff);
    for (i = 0; i < 5; i++)
    {
        hTimStart(buz_id);
        hSysWait(800);
        hTimStop(buz_id);
        hSysWait(300);
    }
    hSysWait(2000);
    for (i = 0; i < 5; i++)
    {
        hTimStart(buz_id);
        hSysWait(800);
        hTimStop(buz_id);
        hSysWait(200);
    }
    hSysWait(2000);
    for (i = 0; i < 5; i++)
    {
        hTimStart(buz_id);
        hSysWait(600);
        hTimStop(buz_id);
        hSysWait(300);
    }
}

#define  TEMP_TBL_CNT 126
#define  TEMP_IDX 0
#define  TEMP_AD_IDX 1
const float TEMP_TABLE[2][TEMP_TBL_CNT] = {
	{-40,-39,-38,-37,-36,-35,-34,-33,-32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,-17,-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85},
    {3.146153846,3.137598425,3.128660436,3.119277108,3.109578765,3.099392097,3.088732394,3.077628032,3.066123317,3.054098361,3.041784038,3.028841413,3.015271786,3.001357466,2.986996111,2.972033393,2.956571964,2.94056203,2.924017318,2.906908874,2.889245706,2.871094359,2.852299552,2.833040894,2.813130717,2.792775899,2.771746438,2.750274863,2.72817536,2.70551252,2.682369455,2.658725223,2.634409036,2.609767831,2.584475282,2.558760108,2.53255814,2.505776173,2.478695869,2.451016208,2.423040128,2.394650206,2.365949618,2.336777583,2.307220217,2.27737837,2.247368421,2.216967509,2.186263922,2.155359001,2.124367652,2.092977323,2.061726079,2.029792148,1.998224852,1.966666667,1.93467108,1.902878916,1.870809874,1.839176627,1.807462687,1.775750577,1.744130127,1.712698413,1.681559588,1.65,1.618985278,1.588115371,1.557563757,1.527140862,1.497016882,1.467175785,1.437592415,1.408340499,1.379385403,1.350797401,1.32253116,1.294652406,1.26848067,1.239946314,1.213109467,1.186782787,1.16075457,1.135071836,1.109928325,1.085234899,1.060890216,1.036935948,1.013414634,0.990207881,0.967679695,0.94537995,0.923505689,0.902266948,0.88117716,0.860615021,0.840617082,0.820847419,0.801514234,0.782645511,0.764074387,0.74601811,0.728304239,0.711155566,0.69418825,0.677622378,0.661477573,0.645773345,0.630313081,0.615327042,0.600613497,0.586184211,0.572276409,0.558453103,0.545179063,0.532240208,0.519413549,0.50694033,0.49483169,0.483098592,0.4717518,0.460313226,0.449520601,0.438893706,0.428437174,0.41815562}
};

int16_t __calculate(float adv)
{
    float temperature;
    float diff, diff2;

    uint16_t i;
    temperature = TEMP_TABLE[TEMP_IDX][0];  // Minimum
    // Compare Measure Real Temp V to R-T Table
    for (i = 0; i < TEMP_TBL_CNT; i++)
    {
        if (TEMP_TABLE[TEMP_AD_IDX][i] < adv)
        {
            break;
        }
        temperature += 1.0f; // 1도씩 올라감.
    }

    if (i == 0)
    {
        return (int16_t)TEMP_TABLE[TEMP_IDX][0] * 100;
    }

    // 소수점 첫째자리까지 계산 한다. 
    // Current Temp Table 1Step Down
    // Current Temp - 1 Step Down Temp
    diff = TEMP_TABLE[TEMP_AD_IDX][i-1] - TEMP_TABLE[TEMP_AD_IDX][i]; 
    diff2 = TEMP_TABLE[TEMP_AD_IDX][i-1] - adv;
    diff = diff2 / diff;
    temperature += diff- 1;

    // scale factor is 10e-2 i.e. (1/100)
    return (int16_t)(temperature * 100);
}

float test_v;
void adc_test(void)
{
    float f;
    int32_t fd = hAdcGetHandle("ADC1");
    //hAdcStart(fd);
    //hExtEnable(HEXT_1);
    while (1)
    {
        f = (3.3*hAdcReadData(fd, 0)) / 4096 ;
        printf(" ch1 v (%f), temp = (%d)\n", f, __calculate(test_v));;
        f = (3.3*hAdcReadData(fd, 1)) / 4096 ;
        printf(" ch2 v (%f), temp = (%d)\n", f, __calculate(test_v));;
        hSysWait(1000);
    }
}

#include "EEprom_ProcessData.h"
void board_test(void)
{
    //uart1 - printf      - OK
    //uart2 - rs485_test() - OK
    //uart3 - raj
    //eeprom - eeprom_test - OK
    //printf("eeprom test\n");
    //eeprom_test();
    //printf("can test\n");
    //can_write_test();
    //printf("rs485 test\n");
    //rs485_test();
#if 0 
    {
        int i;
        extern EEP_COM_CAL  ComCal ;
        hEprEnable();
        hEprWrite(EEP_COM_CAL_ADDR, (uint8_t *)&ComCal, sizeof(EEP_COM_CAL));
        hEprDisable();
/*
            for (i = 0; i < sizeof(EEP_COM_CAL); i++)
            {
                hEprEnable();
                hEprWriteByte(EEP_COM_CAL_ADDR+i, *((uint8_t *)&ComCal+i));
                hEprDisable();
            }
*/
        hEprRead(EEP_COM_CAL_ADDR, (uint8_t *)&EEPROMData.cal, sizeof(EEP_COM_CAL));
        while(1);
    }
#endif
    //pwm_test();
    //adc_test();
}
