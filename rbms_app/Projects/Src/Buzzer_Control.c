/*
 * <모듈 요약>
 * <모듈 상세 설명>
 */

#include "Buzzer_Control.h"

/*== MACRO ===================================================================*/
#define BUZZER_SHORT_TIME 150 // 짧은 부저음 시간 (ms)
#define BUZZER_LONG_TIME 800  // 긴 부저음 시간 (ms)
#define BUZZER_LOW_CLOCK 2    // 낮은 부저음 클럭
#define BUZZER_HIGH_CLOCK 5   // 높은 부저음 클럭

/*== GLOBAL VARIABLE =========================================================*/
/*== STATIC VARIABLE =========================================================*/
static int buzzer_id;
static int buzzer_timer_id;
static int prev_buzzer_clock;
static int new_buzzer_clock;
static int buzzer_time;         // buzzer 울리는 시간

/*== FUNCTION DECLARATION ====================================================*/
static void buzzerFunc(void);

/*== FUNCTION DEFINITION =====================================================*/

void buzzerInit(void)
{
	buzzer_timer_id = hTimCreate();
    hTimSet(buzzer_timer_id, 3000, buzzerControl);
    hTimStart(buzzer_timer_id);
    buzzer_id = hTimCreate();
    prev_buzzer_clock = BUZZER_HIGH_CLOCK;
    hTimSet(buzzer_id, prev_buzzer_clock, buzzerFunc);
}

static void buzzerFunc(void)
{
    if (new_buzzer_clock != prev_buzzer_clock)
    {
        hTimSet(buzzer_id, new_buzzer_clock, buzzerFunc);
        prev_buzzer_clock = new_buzzer_clock;
    }

    buzzer_time -= prev_buzzer_clock;
    if (buzzer_time > 0)
    {
        hTimStart(buzzer_id);
    }
    else
    {
        BUZ_OFF();
        return;
    }

    static uint8_t toggle;
    toggle ^= 1;
    if (toggle)
    {
        BUZ_ON();
    }
    else
    {
        BUZ_OFF();
    }

    
}

/**
 * @brief 짧은 부저음이 한 번 울리도록 구현한 함수.
 * @param none
 * @return none
 */
void buzzerOnShort(void)
{
    new_buzzer_clock = BUZZER_LOW_CLOCK;
    buzzer_time = BUZZER_SHORT_TIME;
    hTimStart(buzzer_id);
}

/**
 * @brief 긴 부저음이 울리도록 구현한 함수.
 * @param none
 * @return none
 */
void buzzerOnLong(void)
{
    new_buzzer_clock = BUZZER_HIGH_CLOCK;
    buzzer_time = BUZZER_LONG_TIME;
    hTimStart(buzzer_id);
}

void buzzerControl(void)
{
#if 1
    if (R.StringModel.St & (1 << STRING_STATUS_RELAY_BIT))
    {
        return;
    }
    if (R.StringModel.Evt1
        & (STREV_COMM_ERROR | STREV_OVTEMP_ALARM | STREV_OVCHAR_CUR_ALARM | STREV_OVDSCH_CUR_ALARM | STREV_OVVOL_ALARM | STREV_UNVOL_ALARM
            | STREV_UNSOC_ALARM | STREV_OVSOC_ALARM | STREV_TEMP_IMBAL_ALARM | STREV_OTHER_ALARM | STREV_CONTACTOR_ERROR))
    {
        buzzerOnLong();
        return;
    }
    if (R.StringModel.Evt2 & (STREXTEV_OVCVOL_ALARM | STREXTEV_UNCVOL_ALARM | STREXTEV_OCD_ALARM))
    {
        buzzerOnLong();
        return;
    }
#endif
}
