/*
 * <모듈 요약>
 * <모듈 상세 설명>
 */

#include "GET_EXT_TEMP.h"

/*== MACRO ===================================================================*/

#define TEMP_TBL_CNT 126
#define TEMP_IDX 0
#define TEMP_AD_IDX 1
/*== GLOBAL VARIABLE =========================================================*/

_EXT_TEMP FET_TEMP;
_EXT_TEMP BD_TEMP;

/*== STATIC VARIABLE =========================================================*/

static const float TEMP_TABLE[2][TEMP_TBL_CNT] = {
    {-40, -39, -38, -37, -36, -35, -34, -33, -32, -31, -30, -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15, -14, -13, -12,
        -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85},
    {3.146153846, 3.137598425, 3.128660436, 3.119277108, 3.109578765, 3.099392097, 3.088732394, 3.077628032, 3.066123317, 3.054098361, 3.041784038,
        3.028841413, 3.015271786, 3.001357466, 2.986996111, 2.972033393, 2.956571964, 2.94056203, 2.924017318, 2.906908874, 2.889245706, 2.871094359,
        2.852299552, 2.833040894, 2.813130717, 2.792775899, 2.771746438, 2.750274863, 2.72817536, 2.70551252, 2.682369455, 2.658725223, 2.634409036,
        2.609767831, 2.584475282, 2.558760108, 2.53255814, 2.505776173, 2.478695869, 2.451016208, 2.423040128, 2.394650206, 2.365949618, 2.336777583,
        2.307220217, 2.27737837, 2.247368421, 2.216967509, 2.186263922, 2.155359001, 2.124367652, 2.092977323, 2.061726079, 2.029792148, 1.998224852,
        1.966666667, 1.93467108, 1.902878916, 1.870809874, 1.839176627, 1.807462687, 1.775750577, 1.744130127, 1.712698413, 1.681559588, 1.65,
        1.618985278, 1.588115371, 1.557563757, 1.527140862, 1.497016882, 1.467175785, 1.437592415, 1.408340499, 1.379385403, 1.350797401, 1.32253116,
        1.294652406, 1.26848067, 1.239946314, 1.213109467, 1.186782787, 1.16075457, 1.135071836, 1.109928325, 1.085234899, 1.060890216, 1.036935948,
        1.013414634, 0.990207881, 0.967679695, 0.94537995, 0.923505689, 0.902266948, 0.88117716, 0.860615021, 0.840617082, 0.820847419, 0.801514234,
        0.782645511, 0.764074387, 0.74601811, 0.728304239, 0.711155566, 0.69418825, 0.677622378, 0.661477573, 0.645773345, 0.630313081, 0.615327042,
        0.600613497, 0.586184211, 0.572276409, 0.558453103, 0.545179063, 0.532240208, 0.519413549, 0.50694033, 0.49483169, 0.483098592, 0.4717518,
        0.460313226, 0.449520601, 0.438893706, 0.428437174, 0.41815562}};

/*== FUNCTION DECLARATION ====================================================*/

/**
 * @param[in] adv voltage from adc of temperature
 * @return temperature in celius. (-40 ~ 85)
 */
static int16_t __calculate(float adv);

/*== FUNCTION DEFINITION =====================================================*/

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

void GET_EXT_TEMPERATURE(void)
{
    FET_TEMP.AVG_TEMP_BUF += EXT_TEMP_ADC_CH10;
    FET_TEMP.AVG_CHK_CNT++;

    if (TEMP_AVG_CNT == FET_TEMP.AVG_CHK_CNT)
    {
        float avgFetTemp;
        FET_TEMP.AVG_CHK_CNT = 0;
        FET_TEMP.AVG_TEMP_BUF = (FET_TEMP.AVG_TEMP_BUF / TEMP_AVG_CNT);

        // To need change real current..
        avgFetTemp = 3.3 * FET_TEMP.AVG_TEMP_BUF / 4096;    // copy
        FET_TEMP.AVG_TEMP_BUF = 0;                          // reset
        FET_TEMP.REAL_TEMP_VALUE = __calculate(avgFetTemp);
    }
#if 1
    BD_TEMP.AVG_TEMP_BUF += EXT_TEMP_ADC_CH11;
    BD_TEMP.AVG_CHK_CNT++;
    if (TEMP_AVG_CNT == BD_TEMP.AVG_CHK_CNT)
    {
        float avgBdTemp;
        BD_TEMP.AVG_CHK_CNT = 0;
        BD_TEMP.AVG_TEMP_BUF = (BD_TEMP.AVG_TEMP_BUF / TEMP_AVG_CNT);

        // To need change real current..
        avgBdTemp = 3.3 * BD_TEMP.AVG_TEMP_BUF / 4096;      // copy
        BD_TEMP.AVG_TEMP_BUF = 0;                           // reset
        BD_TEMP.REAL_TEMP_VALUE = __calculate(avgBdTemp);
    }
#else
#endif

}



