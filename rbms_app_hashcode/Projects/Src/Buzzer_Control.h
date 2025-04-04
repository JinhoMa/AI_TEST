/*
 * <모듈 요약>
 * <모듈 상세 설명>
 */

#ifndef __BUZZER_CONTROL_H__
#define __BUZZER_CONTROL_H__

#include "obshal.h"
#include "main.h"
#include "bms.h"
#include "glovar.h"
#include "master.h"

#ifdef __cplusplus
extern "C" {
#endif

/*== MACRO ===================================================================*/

/*== TYPE DEFINITION =========================================================*/
/*== FUNCTION DECLARATION ====================================================*/
void buzzerInit(void);
void buzzerOnShort(void);
void buzzerOnLong(void);
void buzzerControl(void);

#ifdef __cplusplus
}
#endif

#endif // __BUZZER_CONTROL_H__
