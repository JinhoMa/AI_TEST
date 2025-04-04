/**
    ******************************************************************************
    *     @file         hadc.c
    *     @version
    *     @author
    *     @date      2017/12/16
    *     @brief      E400에서는 ADC 채널수가 많아서 DMA를 사용하였으나 DMA 의 사용 경우
    *                      Software Debugging이 어럽고 DMA 인터럽트가 매우 빈번하게 뜬다.
    *                      E850에서 ADC 채널수가 1개이고 보드온도 만 사용하기 때문에 Interrupt 방식을 사용한다.
    *                      ADC READ가 시작 할때 Start 하여 Interrupt Callback에서 저장하는 방식으로 구현한다.
    *                     따라서 첫 read는 값이 존재하지 않고 다음 read 에서 값을 읽을수 있다.
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

#define USE_DMA 0

#define CHn     12
#define SAMPLE_CNT  1
#define USE_CHn         2

struct adc_struct
{
	uint8_t used;
    void (*func)(void *);
};

typedef struct _HAdc
{
    ADC_HandleTypeDef adc;
    uint8_t used;
    uint8_t dma_on;
    uint8_t  d_pos[USE_CHn];
    uint32_t DATA[SAMPLE_CNT][USE_CHn];
    uint16_t norData[CHn];
    uint8_t cpt[USE_CHn];
    uint8_t now;
} HAdc;


HAdc adc_fd[2];

extern DMA_HandleTypeDef hdma_adc1;



void hAdcInit(void)
{


}

uint32_t hAdcGetHandle(char *port)
{
    if (strcmp(port, "ADC1") == 0)
    {
        return (uint32_t)&adc_fd[0];
    }
    return 0;
}

int32_t hAdcOpen(uint8_t *port, uint32_t chn)
{
    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    if (strcmp((char *)port, "ADC1") != 0)
    {
        return E_INVALID;
    }
    if (chn > USE_CHn)
    {
        return E_INVALID;
    }

    if (adc_fd[0].used == 0)
    {
        adc_fd[0].used = 1;
        adc_fd[0].adc.Instance = ADC1;

      /* USER CODE BEGIN ADC1_Init 1 */

      /* USER CODE END ADC1_Init 1 */

        /** Common config
         */
        adc_fd[0].adc.Instance = ADC1;
        adc_fd[0].adc.Init.ScanConvMode = ADC_SCAN_DISABLE;
        adc_fd[0].adc.Init.ContinuousConvMode = DISABLE;
        adc_fd[0].adc.Init.DiscontinuousConvMode = DISABLE;
        adc_fd[0].adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        adc_fd[0].adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
        adc_fd[0].adc.Init.NbrOfConversion = 1;
        if (HAL_ADC_Init((ADC_HandleTypeDef *)&adc_fd[0]) != HAL_OK)
        {
            Error_Handler();
        }
    }
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

    return (int32_t)&adc_fd[0];
}

void hAdcStart(int32_t fd)
{
    int i = 0;
    ADC_ChannelConfTypeDef sConfig = {0};
    HAdc *adc = (HAdc*)fd;
    //adc->dma_on = 1;
    /** Configure Regular Channel
     */
    for (i = 0; i < USE_CHn; i++)
    {
        adc->cpt[i] = 0;
    }
    adc->now = 0;
    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel((ADC_HandleTypeDef *)&adc_fd[0], &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_ADC_Start_IT((ADC_HandleTypeDef *)&adc_fd[0]);
}

void hAdcStop(int32_t fd)
{
    HAdc *adc = (HAdc*)fd;
    HAL_ADC_Stop((ADC_HandleTypeDef *)adc);
}

uint32_t hAdcReadData(int32_t fd, uint8_t ch)
{
    int i = 0;
    ADC_ChannelConfTypeDef sConfig = {0};
    uint32_t data = 0;
    HAdc *adc = (HAdc *)fd;

    if (ch < USE_CHn)
    {
        for (i = 0; i < SAMPLE_CNT; i++)
        {
            data += adc->DATA[i][ch];
        }
        adc->norData[ch] = data / SAMPLE_CNT;
    }

    adc->now = (adc->now + 1) % USE_CHn;
    if (adc->now == 0)
    {
        sConfig.Channel = ADC_CHANNEL_10;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
        if (HAL_ADC_ConfigChannel((ADC_HandleTypeDef *)&adc_fd[0], &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
    }
    if (adc->now == 1)
    {
        sConfig.Channel = ADC_CHANNEL_11;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
        if (HAL_ADC_ConfigChannel((ADC_HandleTypeDef *)&adc_fd[0], &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
    }
    HAL_ADC_Start_IT((ADC_HandleTypeDef *)&adc_fd[0]);
    return adc->norData[ch];
}

uint32_t hAdcReadPollData(int32_t fd , uint8_t ch)
{
    int i;
    uint32_t data = 0;
    HAdc *adc = (HAdc *)fd;
    adc->cpt[ch] = 0;

    if (adc->dma_on)
    {
        while (!adc->cpt[ch]);
    }
    if (ch < USE_CHn)
    {
        for (i = 0; i < SAMPLE_CNT; i++)
        {
            data += adc->DATA[i][ch];
        }
        adc->norData[ch] = data / SAMPLE_CNT;
    }

    return adc->norData[ch];
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    int i;
    if (hadc->Instance == ADC1)
    {
        for (i = 0; i < USE_CHn; i++)
        {
            adc_fd[0].cpt[i] = 1;
        }
    }
    if (adc_fd[0].now == 0)
    {
        adc_fd[0].DATA[adc_fd[0].d_pos[0]][0] = HAL_ADC_GetValue((ADC_HandleTypeDef*)&adc_fd[0]);
        adc_fd[0].d_pos[0] = (adc_fd[0].d_pos[0] + 1) % SAMPLE_CNT;
    }
    if (adc_fd[0].now == 1)
    {
        adc_fd[0].DATA[adc_fd[0].d_pos[1]][1] = HAL_ADC_GetValue((ADC_HandleTypeDef*)&adc_fd[0]);
        adc_fd[0].d_pos[1] = (adc_fd[0].d_pos[1] + 1) % SAMPLE_CNT;
    }
}


