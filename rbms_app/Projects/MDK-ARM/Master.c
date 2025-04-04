/**
 ******************************************************************************
 *     @file         Master.c
 *     @version
 *     @author
 *     @date      2024/09/19
 *     @brief
 ******************************************************************************
*/

#include "main.h"
#include "obshal.h"
#include "bms.h"
#include "bms_os.h"
#include "master.h"
#include "CAN_DiagCom.h"
#define ON 1
#define OFF 0

#define EXP_TIME    3000

uint8_t MasterMode;

#define D_TOTALENG  EEPROMData.soccnf.total_eng
#define D_CWCV      EEPROMData.mgconf.CWCV
#define D_PDCV      EEPROMData.mgconf.PDWNCV
#define D_CHGDETECT EEPROMData.mgconf.CHGJC
#define D_FCCV      EEPROMData.mgconf.FCCV
#define D_FCWCV     EEPROMData.mgconf.FCWJCV
#define D_FCJC      EEPROMData.mgconf.FCJC * 10
#define D_DCHGSCV   EEPROMData.mgconf.DCHGSCV
#define D_DISDETECT EEPROMData.mgconf.DCHGJC * 10
#define D_TAPPER    EEPROMData.mgconf.FCTV

#define SLEEP_TIME_CHG   (24UL * 60 * 60 * 1000)        // 24시간을 밀리초로
#define SLEEP_TIME_DISCHG   (100UL * 60 * 60 * 1000)     // 100시간을 밀리초로

static uint8_t fcc_cnt;
static uint8_t fdc_cnt;
static uint8_t saved = 0;
static uint8_t pdc_cnt;
static uint8_t disj_cnt;
static uint8_t chgj_cnt;
static uint8_t term_tr_cnt;
static uint8_t cw_term_cnt;
static uint8_t isAlarmActive(void);
static void master_sleep_check(uint8_t *next_mode);

static void releaseLog(void)
{
    saved = 0;
}

static void saveLog(uint32_t ps )
{
    if (saved == 1)
    {
        return;
    }
    RackSt_SaveCurrentStAtOnce(ps);
    saved = 1;
}

void master_init(void)
{
    MasterMode = M_INIT;
    fcc_cnt = 0;
}

void master_mode_check(void)
{
    uint8_t next_mode = MasterMode;
    if (MasterMode > M_WAKE)
    {
        if (R.StringModel.CellVMin <= D_PDCV)
        {
            if (pdc_cnt++ > 8)
            {
                RLY_CTL.RELAY_MODE = RELAY_FORCE_OFF;
                MasterMode = M_DOWN;
            }
        }
        else
        {
            pdc_cnt = 0;
        }
    }

    switch (MasterMode)
    {
        case M_INIT:
            next_mode = M_WAKE;
            hLogProc(LOG_MBMS, "wakeup\n");
            break;
        case M_WAKE:
            R.StringModel.SetEna = STRING_ENABLE;
            R.StringModel.SetCon = STRING_ENABLE;
            if (R.StringModel.CellVMin <= D_CWCV || R.StringModel.BatV <= (D_CWCV * TRAY_CELL_NUMS) / 10)
            {
                RLY_CTL.RELAY_MODE = CHG_ONLY_MODE;  // Normal Mode
                next_mode = M_CWIT;
                hLogProc(LOG_MBMS, "cwait\n");
                break;
            }

            if (R.StringModel.BatV >= (D_FCWCV * TRAY_CELL_NUMS) / 10)
            {
                RLY_CTL.RELAY_MODE = DIS_ONLY_MODE;  // Normal Mode
                next_mode = M_TERM;
                hLogProc(LOG_MBMS, "term\n");
            }
            else
            {
                RLY_CTL.RELAY_MODE = ALL_MODE;
                next_mode = M_DIS;
                hLogProc(LOG_MBMS, "discharge\n");
            }
            break;
        case M_CWIT:
            if (++cw_term_cnt < 4)
            {
                break;
            }
            cw_term_cnt = 0;
            // CFET ON, DFET OFF 상태 
            if ( (R.StringModel.A > D_CHGDETECT) && (R.StringModel.CellVMin > D_CWCV ))
            {
                RLY_CTL.RELAY_MODE = ALL_MODE;
                next_mode = M_CHG;
                hLogProc(LOG_MBMS, "charge\n");
            }
            break;
        case M_TERM:
            if (++term_tr_cnt < 4)
            {
                break;
            }
            term_tr_cnt =0;
            if (R.StringModel.A <= D_DISDETECT)
            {
                RLY_CTL.RELAY_MODE = ALL_MODE;
                next_mode = M_DIS;
                hLogProc(LOG_MBMS, "discharge\n");
            }
            else
            {
                if ((R.StringModel.BatV <= ((D_FCCV * TRAY_CELL_NUMS) / 10 - D_TAPPER)) &&
                    (R.StringModel.CellVMax <= (D_FCCV - D_TAPPER)))
                {
                    RLY_CTL.RELAY_MODE = ALL_MODE;  // Normal Mode
                    next_mode = M_CHG;
                    hLogProc(LOG_MBMS, "charge\n");
                }
            }
            break;
        case M_CHG:
            if ((R.StringModel.BatV >= ((D_FCCV * TRAY_CELL_NUMS) / 10)) || 
                (R.StringModel.CellVMax >= D_FCCV))
            {
                if (fcc_cnt++ > (EXP_TIME / MMBS_EVT_TIME))
                {
                    PushJob(RBMS_SCHEDULER_IDX, RBMS_FULLCHARGE, 0, 0);
                    RLY_CTL.RELAY_MODE = DIS_ONLY_MODE;
                    next_mode = M_TERM;
                    hLogProc(LOG_MBMS, "term\n");
                    fcc_cnt = 0;
                    break;
                }
            }
            else
            {
                fcc_cnt = 0;
            }
            if (R.StringModel.A <= D_DISDETECT)
            {
                if (disj_cnt++ > (1000 / MMBS_EVT_TIME))    //1sec
                {
                    RLY_CTL.RELAY_MODE = ALL_MODE;
                    next_mode = M_DIS;
                    hLogProc(LOG_MBMS, "discharge\n");
                    disj_cnt =0;
                    break;
                }
            }
            else
            {
                disj_cnt = 0;
            }
            break;
        case M_DIS:
            if (R.StringModel.CellVMin <= D_DCHGSCV || R.StringModel.BatV <= (D_DCHGSCV * TRAY_CELL_NUMS) / 10)
            {
                if (fdc_cnt++ > (EXP_TIME / MMBS_EVT_TIME))
                {
                    PushJob(RBMS_SCHEDULER_IDX, RBMS_FULLDIS, 0, 0);
                    RLY_CTL.RELAY_MODE = CHG_ONLY_MODE;  // Normal Mode
                    next_mode = M_CWIT;
                    hLogProc(LOG_MBMS, "cwait\n");
                    fdc_cnt = 0;
                }
            }
            else
            {
                fdc_cnt = 0;
                if (R.StringModel.A > D_CHGDETECT)
                {
                    if (chgj_cnt++ > (1000 / MMBS_EVT_TIME))    //1sec
                    {
                        RLY_CTL.RELAY_MODE = ALL_MODE;  // Normal Mode
                        next_mode = M_CHG;
                        hLogProc(LOG_MBMS, "charge\n");
                        chgj_cnt=0;
                    }
                }
                else
                {
                    chgj_cnt = 0;
                }
            }
            break;
        case M_DOWN:
            // Power Down
            f_pdown = ON;
            saveLog(F_PDOWN);
            hLogProc(LOG_MBMS, "powerdown\n");
#if (D_PV==1)
            PushJob(RBMS_SCHEDULER_IDX,RBMS_SYS_POWERDOWN, 0, 0);
#endif
            hSysWait(10000);
            break;
        case M_FMODE:
            f_error = ON;
            hLogProc(LOG_MBMS, "fail\n");
            saveLog(F_ERROR);
            if (!isAlarmActive())   // 활성화된 에러가 없을 때
            {
                f_error = OFF;
                releaseLog();
                next_mode = M_WAKE;
                hLogProc(LOG_MBMS, "wakeup\n");
            }
            if (R.StringModel.Evt1 & STREV_CONTACTOR_ERROR)
            {
                releaseLog();
                RLY_CTL.RELAY_MODE = RELAY_FORCE_OFF;
                next_mode = M_PFAIL;
            }
            break;
        case M_PFAIL:
            f_pfail = ON;
            hLogProc(LOG_MBMS, "p fail!!\n");
            saveLog(F_PFAIL);
#if (D_PV==1)
            //SCP OFF
            PushJob(SUBMIC_SCHEDULER_IDX, RBMS_BLOWOFF_FUSE, 0, NULL);
#endif
            break;
    }
    master_sleep_check(&next_mode);
    MasterMode = next_mode;
}

void master_fault_check(void)
{
    if (R.StringModel.St & (1 << STRING_STATUS_RELAY_BIT))
    {
        return;
    }
    if (MasterMode == M_PFAIL)
    {
        return;
    }
    if (isAlarmActive())
    {
        RLY_CTL.RELAY_MODE = RELAY_FORCE_OFF;
        MasterMode = M_FMODE;
    }
}

/**
 * @brief 현재 활성화된 에러 알람이 있는지 확인하여 알람이 있다면 1, 없다면 0을 리턴한다.
 *         체크하는 에러 알람은 다음과 같다.
 *         통신 에러, 셀 고온 에러, 과충전 전류 에러, 과방전 전류 에러, 셀 고전압 에러, 샐 저전압 에러, 시스템 에러, 릴레이 에러
 *
 * @return 1 : 현재 활성화된 에러 알람이 있음. 0 : 현재 활성화된 에러 알람이 없음
 */
static uint8_t isAlarmActive(void)
{
    if (R.StringModel.Evt1 & (STREV_COMM_ERROR | STREV_OVTEMP_ALARM | STREV_OVCHAR_CUR_ALARM | STREV_OVDSCH_CUR_ALARM |
        STREV_OVVOL_ALARM | STREV_UNVOL_ALARM | STREV_OTHER_ALARM | STREV_CONTACTOR_ERROR))
    {
        return 1;
    }

    if (R.StringModel.Evt2 & (STREXTEV_OVCVOL_ALARM | STREXTEV_UNCVOL_ALARM | STREXTEV_OCD_ALARM))
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Charge, Charge Wait, Discharge 모드에서 장시간 충방전 전류 감지가 되지 않을 경우 BMS 동작을 중지(Power Down)하여
 *        배터리의 과방전 혹은 과충전(충전 모드에서 BMS가 감지하지 못하는 미세전류로 인해 발생)을 방지한다.
 *
 *        MasterMode가 위의 3가지 모드 이외에 다른 모드로 진입하거나, 충/방전이 감지되면
 *        시작 시간을 현재 시간으로 업데이트하여 타이머를 재시작한다.
 *
 * @param[in] next_mode 다음 master mode를 변경하기 위해 입력받는 변수
 * @return NONE
 */
static void master_sleep_check(uint8_t *next_mode)
{
    static uint64_t start_time = 0;             // sleep mode timer 시작 시간
    static uint64_t sleep_limit = 0;            // sleep mode 설정 시간
    uint64_t current_time;                      // 현재 시간
    uint64_t elapsed_time;                      // 경과 시간

    // MasterMode가 M_CHG, M_CWIT, M_DIS인 경우에만 슬립 모드 타이머를 동작
    if (MasterMode != M_CHG && MasterMode != M_CWIT && MasterMode != M_DIS)
    {
        // 해당 모드가 아니면 시작 시간을 현재 시간으로 업데이트하고 종료
        start_time = hTimCurrentTime();
        return;
    }

    // 충전 또는 방전 중이면 시작 시간을 현재 시간으로 업데이트하고 종료
    if (f_dis == 1 || f_chg == 1)
    {
        start_time = hTimCurrentTime();
        return;
    }

    // MasterMode에 따른 슬립 시간 설정
    sleep_limit = (MasterMode == M_DIS) ? SLEEP_TIME_DISCHG : SLEEP_TIME_CHG;

    // 현재 시간 가져오기
    current_time = hTimCurrentTime();

    // 경과 시간 계산
    if (current_time >= start_time)
    {
        elapsed_time = current_time - start_time;
    }

    // 지정된 시간이 지났는지 확인
    if (elapsed_time >= sleep_limit)
    {
        *next_mode = M_DOWN;  // next_mode를 변경하여 슬립 모드로 전환
        hLogProc(LOG_MBMS, "Sleep Mode\n");
        start_time = current_time;  // 시작 시간 초기화
        sleep_limit = 0;            // 슬립 시간 초기화
    }
}

PUBLIC void can_rack_cursimulation_handler(uint8_t *msg)
{
    uint8_t lid = msg[0];
    uint8_t sid = msg[1];
    int32_t data;
    if (CanGW.AccessStatus != SUCESSACCESS)
    {
        responseNegtiveMessage(lid, N_NOACESS_MODE );
        return;
    }

    if (sid == CUR_SIMUL_ON)
    {
        CanGW.Simul.cur_sim = 1;
        data = (int32_t)(int16_t)((msg[2] << 8) | msg[3]);
        CanGW.Simul.sim_data = data * 1000;
    }
    else
    {
        CanGW.Simul.cur_sim = 0;
    }
    responseNegtiveMessage(lid, 0);
}

PUBLIC void can_mbms_logout(void)
{
    hCanMessage msg;
    uint16_t r_eng = (uint32_t)(D_TOTALENG * R.StringModel.SoC) / 1000;
    msg.id = 0x4F7;
    msg.len = 8;
    msg.data[0] = (D_TOTALENG) >> 8; // 충전 용량(TOTAL)
    msg.data[1] = D_TOTALENG;
    msg.data[2] = r_eng >> 8;       //
    msg.data[3] = r_eng & 0xFF;
    msg.data[4] = Rack_St >> 8;                 // 최대 충전 Rate
    msg.data[5] = Rack_St;
    msg.data[6] = MasterMode;                 // 최대 방전 Rate
    msg.data[7] = 0;
    hCanWrite(diag_can_fd, &msg, 1);

    msg.id = 0x4F2;
    msg.len = 8;
    msg.data[0] = FET_TEMP.REAL_TEMP_VALUE >> 8;
    msg.data[1] = FET_TEMP.REAL_TEMP_VALUE;
    msg.data[2] = BD_TEMP.REAL_TEMP_VALUE >> 8;
    msg.data[3] = BD_TEMP.REAL_TEMP_VALUE;
    msg.data[4] = 0;                 // 최대 충전 Rate
    msg.data[5] = 0;
    msg.data[6] = 0;                 // 최대 방전 Rate
    msg.data[7] = 0;
    hCanWrite(diag_can_fd, &msg, 1);
}
