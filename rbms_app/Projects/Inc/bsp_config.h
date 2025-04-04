/*
 * 모듈을 선택한다.
 */

#ifndef __BSP_CONFIG__
#define __BSP_CONFIG__

#ifdef __cplusplus
extern "C" {
#endif

/*== MACRO ===================================================================*/

/* RTOS */
#if defined(__CMSIS_RTOS)
#define BSP_CONFIG_RTOS_RTX  1
#elif defined(__FREERTOS__)
#define BSP_CONFIG_RTOS_FREERTOS  1
#else
    error " define OS "
#endif

/*== TYPE DEFINITION =========================================================*/
/*== FUNCTION DECLARATION ====================================================*/

#ifdef __cplusplus
}
#endif

#endif // __BSP_CONFIG__
