/**
    ******************************************************************************
    *     @file         BMIC_ProcessData.c
    *     @version   
    *     @author    
    *     @date      2017/09/12 
    *     @brief     BMIC 의 데이터를 관리하고, RBMS에 데이터를 전송 
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"
#include "BMS.h"
#include "BMIC_ProcessData.h"
#include "SubmicProcessData.h"

#define CVREF_H    EEPROMData.cal.CV_HVREF
#define CVREF_L    EEPROMData.cal.CV_LVREF
#define CV_LD       EEPROMData.cal.D_LV
#define CV_HD       EEPROMData.cal.D_HV

//float32 gSensingVoltageValue[SLAVEDEVICENUM*CELLNUMPERDEVICE];		// Sensing Voltage	Array
STR_TrayModel Tray[TBMS_BUF_SIZE];//Tray Modeling Variable

uint8_t tray_fault;
uint8_t tray_fault_num;
uint8_t tray_mod_fault_num;
uint32_t tray_comErrorBit[MAX_COMM_NUMS];

//jae.kim 
// 까먹었네. 
//여기서 비트 설정해야 읽혀짐. 
uint8_t CELLTABLE[4][BMIC_MAX_CH]=
{
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0
/*
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    
  */
};

/**
  * @brief Tray 관련 초기화 작업
  */
void Init_Tray()
{
    int i;
    
    for(i=0; i<BMIC_DEVICE_COUNT;i++)
    {
        uint16_t mask; 
        uint8_t j;
        mask = CELLMASK(i);
        for(j=0; j< BMIC_MAX_CH; j++){
            CELLTABLE[i][j] = ( (1<<j) & mask) ?  1 : 0;
        }
    }
    
    for(i=0;i<TBMS_BUF_SIZE;i++)
    {
    	Tray[i].MbmsID=0x0;
    	Tray[i].Mode.all=0;
    	Tray[i].MaxCell.CellNumber=0;
    	Tray[i].MaxCell.Voltage.all=INIT_CELLVOLTAGE;
    	Tray[i].MinCell.CellNumber=0;
    	Tray[i].MinCell.Voltage.all=INIT_CELLVOLTAGE;
    	Tray[i].VoltageMismatch.Fault=0;
    	Tray[i].VoltageMismatch.Value.all=0;
    	Init_Alert_Tray(i);
    	Init_Module(i);
    }

    
}


/**
  * @brief TRAY 내에 Module들의 정보를 초기화 한다.
  */
void Init_Module(uint8_t id)
{
	uint16_t i=0;
    uint16_t MaxUsedCnt=0;
    
	for(i=0; i< BMIC_DEVICE_COUNT; i++)
	{
		Tray[id].Module[i].DeviceID=0xFFFFFFFF;
		Tray[id].Module[i].MaxCell.CellNumber=0;
		Tray[id].Module[i].MaxCell.Voltage.all=0;
		Tray[id].Module[i].MinCell.CellNumber=0;
		Tray[id].Module[i].MinCell.Voltage.all=0;

		Tray[id].Module[i].VoltageMismatch.Fault=0;
		Tray[id].Module[i].VoltageMismatch.Value.all=0;

		Tray[id].Module[i].u.Register_17823.FMEA.Reg.all=0;
		Tray[id].Module[i].u.Register_17823.StatusReg.all=0;
		Tray[id].Module[i].u.Register_17823.WatchdogReg.all=0;
		Tray[id].Module[i].u.Register_17823.InternalREF.Reg.all=0;


		Tray[id].Module[i].ModuleVoltage.BlockByBMIC.all=0;
		Tray[id].Module[i].ModuleVoltage.TotalByBMIC.all=0;
		Tray[id].Module[i].ModuleVoltage.CellSumBySW.all=0;

		Tray[id].Module[i].HWDiag.ExtFetFault.all=0;
		Tray[id].Module[i].HWDiag.CnPinFault=0;
 //Aux 정보를 입력 
#if ( P100_8S ==1 )
        Tray[id].Module[i].aux[0].type = (EEPROMData.base.u.aux10.BmicAuxUse1[i]>>6) &0x3;
        Tray[id].Module[i].aux[1].type = (EEPROMData.base.u.aux10.BmicAuxUse1[i]>>4) &0x3;
        Tray[id].Module[i].aux[2].type = (EEPROMData.base.u.aux10.BmicAuxUse1[i]>>2) &0x3;
        Tray[id].Module[i].aux[3].type = (EEPROMData.base.u.aux10.BmicAuxUse1[i])    &0x3;
#endif     
		Init_Cells(id, i);
        MaxUsedCnt = (MaxUsedCnt <  Tray[id].Module[i].usedCellNum) ? Tray[id].Module[i].usedCellNum : MaxUsedCnt;
		Init_Alert_Module(id, i);
        Init_TempSensor(id);
	}
    Tray[id].maxUsedCnt = MaxUsedCnt;       // READALL Cell COunt 개수 
}

void Init_Temperature(uint8_t id)
{
    uint8_t i;

    for(i=0; i< TMP_BUF_SIZE ; i++){
        Tray[id].Module_TempSensor[i].Temperature.all = 0;
    }
    for(i=0; i< BDTMP_BUF_SIZE ; i++){
        Tray[id].Board_TempSensor[i].Temperature.all = 0;
    }
}

void Init_Cells(uint8_t id, uint16_t DeviceNum)
{
	uint16_t CellNum;

	for(CellNum=0; CellNum<BMIC_MAX_CH  ; CellNum++)
	{
		Tray[id].Module[DeviceNum].Cells[CellNum].CellVoltage.all=INIT_CELLVOLTAGE;
		Tray[id].Module[DeviceNum].Cells[CellNum].BufferCellVoltage.all=INIT_CELLVOLTAGE;

		Tray[id].Module[DeviceNum].Cells[CellNum].Status.Balancing=0;
		Tray[id].Module[DeviceNum].Cells[CellNum].Status.VoltageStatus.ByBMIC.all=0;
		Tray[id].Module[DeviceNum].Cells[CellNum].Status.VoltageStatus.BySW.all=0;
		Tray[id].Module[DeviceNum].Cells[CellNum].Status.VoltageStatus.Summary.all=0;
		Tray[id].Module[DeviceNum].Cells[CellNum].ZeroVoltFlag=0;
		Tray[id].Module[DeviceNum].Cells[CellNum].OnBalancing=0;

        if(CELLTABLE[DeviceNum][CellNum] == 0){
            Tray[id].Module[DeviceNum].Cells[CellNum].used = 0;
        }else{
            Tray[id].Module[DeviceNum].Cells[CellNum].used = 1;
            Tray[id].Module[DeviceNum].usedCellNum++;
        }

		Init_Alert_Cells(id, DeviceNum,CellNum);
	}

}

/**
  * @brief Tray의 Alarm 정보를 초기화 한다.
  */

void Init_Alert_Tray(uint8_t id)
{
	Tray[id].VoltageMismatch.Alert.Warning_MisMatch=0;
	Tray[id].VoltageMismatch.Alert.Fault_MisMatch=0;
	Tray[id].VoltageMismatch.Alert.Count.MisMatch_ClearCount=0;
	Tray[id].VoltageMismatch.Alert.Count.MisMatch_SetCount=0;
}

void Init_Alert_Module(uint8_t id, uint16_t DeviceNum)
{

}

void Init_Alert_Cells(uint8_t id, uint16_t DeviceNum, uint16_t CellNum)
{
	Tray[id].Module[DeviceNum].Cells[CellNum].Alert.Count.OV_ClearCount=0;
	Tray[id].Module[DeviceNum].Cells[CellNum].Alert.Count.OV_SetCount=0;
	Tray[id].Module[DeviceNum].Cells[CellNum].Alert.Count.UV_ClearCount=0;
	Tray[id].Module[DeviceNum].Cells[CellNum].Alert.Count.UV_SetCount=0;
	Tray[id].Module[DeviceNum].Cells[CellNum].Alert.Fault.all=0;
	Tray[id].Module[DeviceNum].Cells[CellNum].Alert.Warning.all=0;
	Tray[id].Module[DeviceNum].Cells[CellNum].Alert.VoltageStatus_Conversion=0;
}

void Init_TempSensor(uint8_t id)
{
	uint16_t i;;
	for(i=0; i < MODULE_TEMP_SENSORNUM; i++)
	{
		Tray[id].Module_TempSensor[i].Temperature.all=INIT_TEMPERATURE;
		Tray[id].Module_TempSensor[i].Status.BySW.all=0;
		Tray[id].Module_TempSensor[i].Status.SensorStatus.all=0;
		Tray[id].Module_TempSensor[i].Status.Summary.all=0;
    }

    for(i=0; i < MODULE_BDTEMP_SENSORNUM; i++)
	{
	    Tray[id].Board_TempSensor[i].Temperature.all=INIT_TEMPERATURE;
		Tray[id].Board_TempSensor[i].Status.BySW.all=0;
		Tray[id].Board_TempSensor[i].Status.SensorStatus.all=0;
		Tray[id].Board_TempSensor[i].Status.Summary.all=0;

	}

}


// *************************************************************
// * 					Save the Read Data
// ************************************************************
#if ( P100_8S ==1 )    

void SaveCellVoltage(uint8_t ch, uint16_t CellNumber_id, uint16_t Storage_Location)
{

}


#else 
#endif

void SaveBlockVoltage(uint8_t ch)
{
#if ( P100_8S==1)    

    // Cell 읽을때 이미 Block 계산 해 버렸음.
    // BLOCK 의 모듈이 1개....

#endif
}

void SaveTotalVoltage(uint8_t ch)
{
#if ( P100_8S==1)    

#endif
}

void SaveModuleAuxData(uint8_t ch, uint8_t aux_id)
{
#if ( P100_8S == 1)    

   

#endif

}


void SaveModuleTemperature(uint8_t ch, uint16_t Varity)
{

}

void SaveDaisyChainDevAddress(uint8_t ch)
{
}
void SaveDaisyChainDevAddressPointer(uint8_t ch, uint8_t id, uint32_t chipid)
{
}

static uint16_t ConvertADtoCV( uint8_t cid )
{
    uint16_t tad;
    int16_t ttmp2, ttmp3;
    int32_t ttmp1;
    tad = ir.model.cell[cid];
// Voltage = (AD - VL) * (VrefH - VrefL) / (VH - VL) + VrefL
// *:Vx = Calibration AD value, Vrefx = Calibration reference voltage value
	ttmp1 = ((long)tad - CV_LD[cid] ) * ( CVREF_H - CVREF_L );
	ttmp2 = CV_HD[cid] - CV_LD[cid];
	ttmp3 = (uint16_t)( ttmp1 % ttmp2 );
	ttmp1 = ttmp1 / ttmp2;
	if( ttmp1 < 0 && ttmp3 != 0 )
	{
		ttmp1 -= 1;
	}
	return (ttmp1 + CVREF_L );
}

void Update_Voltage_MisMatch_Check(uint8_t id)
{
	uint16_t i=0;
	uint16_t j=0;
    uint16_t minVol;
    uint16_t minCellNumber;
    uint16_t maxVol;
    uint16_t maxCellNumber;
    uint16_t cellcnt;
    uint8_t set_max, set_min;
    
  	for(i=0 ; i <BMIC_DEVICE_COUNT ; i++)
	{
            //42S 는 Cell 0를 사용하지 않기 때문에 0값을 max, min 값으로 사용하면 잘못된 값은 가진다. 
            //따라서 Used 체크후 max,min 값을 가지도록 수정
		Tray[id].Module[i].MinCell.CellNumber=0;
		Tray[id].Module[i].MaxCell.CellNumber=0;
           set_max = 0; set_min = 0;
		for(j=0;j< BMIC_MAX_CH ;j++)
		{
		    if( Tray[id].Module[i].Cells[j].used == 0)
                continue;
               if(!set_max){
                   Tray[id].Module[i].MaxCell.Voltage.all=Tray[id].Module[i].Cells[j].CellVoltage.all;                    
                    set_max= 1;
               }
               if(!set_min){
                   Tray[id].Module[i].MinCell.Voltage.all=Tray[id].Module[i].Cells[j].CellVoltage.all;
                    set_min=1;
               }
			if(Tray[id].Module[i].Cells[j].CellVoltage.all >= Tray[id].Module[i].MaxCell.Voltage.all)
			{
				Tray[id].Module[i].MaxCell.Voltage.all=Tray[id].Module[i].Cells[j].CellVoltage.all;
				Tray[id].Module[i].MaxCell.CellNumber=j+1;
			}

			if(Tray[id].Module[i].Cells[j].CellVoltage.all <= Tray[id].Module[i].MinCell.Voltage.all)
			{
				Tray[id].Module[i].MinCell.Voltage.all=Tray[id].Module[i].Cells[j].CellVoltage.all;
				Tray[id].Module[i].MinCell.CellNumber=j+1;
			}
		}

		Tray[id].Module[i].VoltageMismatch.Value.all=Tray[id].Module[i].MaxCell.Voltage.all-Tray[id].Module[i].MinCell.Voltage.all;

		if( Tray[id].Module[i].VoltageMismatch.Value.all < CELLDEVIATION)
		{
			Tray[id].Module[i].VoltageMismatch.Fault=0;
		}else
		{
			Tray[id].Module[i].VoltageMismatch.Fault=1;
		}

	}

    minVol = Tray[id].Module[0].MinCell.Voltage.all;
    minCellNumber = Tray[id].Module[0].MinCell.CellNumber;
    maxVol = Tray[id].Module[0].MaxCell.Voltage.all;
    maxCellNumber= Tray[id].Module[0].MaxCell.CellNumber;
    cellcnt =0;
    for(i=1; i < BMIC_DEVICE_COUNT ; i++){
        cellcnt += Tray[id].Module[i-1].usedCellNum;
	    //--- Tray Min Cell Number &Voltage
    	if( minVol  > Tray[id].Module[i].MinCell.Voltage.all)
        {
            minCellNumber=Tray[id].Module[i].MinCell.CellNumber + cellcnt;
    		minVol=Tray[id].Module[i].MinCell.Voltage.all;
    	}
    	//--- Tray Max Cell Number &Voltage
    	if(maxVol  <= Tray[id].Module[i].MaxCell.Voltage.all)
    	{
    		maxCellNumber = Tray[id].Module[i].MaxCell.CellNumber + cellcnt;
    		maxVol=Tray[id].Module[i].MaxCell.Voltage.all;
    	}
    }
    Tray[id].MaxCell.Voltage.all = maxVol;

    Tray[id].MaxCell.CellNumber = maxCellNumber;
    Tray[id].MinCell.Voltage.all = minVol;
    Tray[id].MinCell.CellNumber = minCellNumber;
	Tray[id].VoltageMismatch.Value.all=Tray[id].MaxCell.Voltage.all-Tray[id].MinCell.Voltage.all;

	if(Tray[id].VoltageMismatch.Value.all > CELLDEVIATION ) //&& Balancing.BalancingStatus !=CELLBALANCING)
	{
		Tray[id].VoltageMismatch.Fault=1;
	}else
	{
		Tray[id].VoltageMismatch.Fault=0;
	}

}
void BMIC_Read_Block_V(uint8_t ch)
{
    SaveBlockVoltage(ch);
}

void BMIC_Read_Each_Voltage(uint8_t ch, uint16_t Value, uint16_t Storage_Location )
{
    int CellNumber_id;
	uint16_t tempData=0;
    uint16_t tempBlockVData = 0;
    uint16_t avg;
    uint8_t pos =0;
    for(CellNumber_id=0;CellNumber_id< TRAY_CELL_NUMS ; CellNumber_id++ ){
        tempData=ConvertADtoCV( CellNumber_id);
        pos = Tray[0].Module[0].Cells[CellNumber_id].samplePos ;
		Tray[0].Module[0].Cells[CellNumber_id].SampleVolt[pos].all =tempData;//Save the 
#if (SEN_ALG_3MEANS == 1)
        {
            uint16_t *temp;
		Tray[0].Module[0].Cells[CellNumber_id].samplePos = (++Tray[0].Module[0].Cells[CellNumber_id].samplePos)%3;
            temp = &Tray[0].Module[0].Cells[CellNumber_id].SampleVolt[0].all;
            if(*temp > *(temp+1) ){
                if(*temp > *(temp+2)){
                    //max = *temp;
                    if(*(temp+1) > *(temp+2)){
                        avg = *(temp+1);
                        //min = *(temp+2);
                    }else{
                        avg = *(temp+2);
                        //min = *(temp+1);
                    }
                }else{
                    //max = *(temp+2);
                    avg = *(temp);
                    //min = *(temp+1);
                }
            }else{
                if(*(temp+1) < *(temp+2)){
                    //max = *(temp+2);
                    //min= *temp;
                    avg = *(temp+1);
                }else{
                    //max = *(temp+1);
                    if(*(temp+2) > *temp){
                        avg = *(temp+2);
                        //min = *temp;
                    }else{
                        avg = *temp;
                        //min = *(temp+2);
                    }
                }
            }
        }
        Tray[0].Module[0].Cells[CellNumber_id].CellVoltage.all = avg;
        tempBlockVData += avg;

#endif        
    
   }
    // Moduel내에 BMIC 1개 라서...
    Tray[0].Module[0].ModuleVoltage.CellSumBySW.all=tempBlockVData;
    Tray[0].Module[0].ModuleVoltage.CellSumMV.all=tempBlockVData;    
    Tray[0].Module[0].ModuleVoltage.BlockByBMIC.all=tempBlockVData;        
    Tray[0].Module[0].ModuleVoltage.TotalByBMIC.all=tempBlockVData; 
    Tray[0].TrayVoltage.CellSumBySW.all=tempBlockVData;
    Tray[0].TrayVoltage.CellSumMV.all = tempBlockVData;       
}

#if 0
//---Voltage Fault Check By SW
void CheckCellStatusbySW()
{
	uint16_t i,j;
    uint8_t id;
    for(id =0; id< TRAY_NUMS; id++){
    	for(i=0; i <BMIC_DEVICE_COUNT; i++)
    	{
    		for(j=0; j <BMIC_MAX_CH; j++)
    		{
    		    if(Tray[id].Module[i].Cells[j].used == 0){
                    continue;
                }
    			CheckEachCellStausbySW(id, i,j);

    		}
    	}
    }

}
#endif

#define GE_RT_TABLE_LENGTH 126
#define LTR_RT_TABLE_LENGTH 161
#define NCT_SHORT_TEMPERATURE 200*100			//200 'C
#define NCT_OPEN_TEMPERATURE -50*100			//-50 'C

const float GE_Temperature_Table[GE_RT_TABLE_LENGTH][2]=
{
	{204.549,-40},{193.221,-39},{182.594,-38},{172.621,-37},{163.257,-36},{154.461,-35},{146.195,-34},{138.425,-33},{131.117,-32},{124.242,-31},
	{117.771,-30},{111.678,-29},{105.938,-28},{100.53,-27},{95.431,-26},{90.623,-25},{86.088,-24},{81.807,-23},{77.766,-22},{73.949,-21},
	{70.344,-20},{66.936,-19},{63.714,-18},{60.667,-17},{57.784,-16},{55.056,-15},{52.473,-14},{50.027,-13},{47.709,-12},{45.513,-11},
	{43.431,-10},{41.457,-9},{39.584,-8},{37.807,-7},{36.12,-6},{34.519,-5},{32.998,-4},{31.553,-3},{30.179,-2},{28.874,-1},
	{27.632,0},	{26.451,1},{25.328,2},{24.258,3},{23.24,	4},	{22.27,	5},{21.347,6},{20.467,7},{19.628,8},{18.829,9},
	{18.066,10},{17.339,11},{16.645,12},{15.983,13},{15.351,14},{14.747,15},{14.17,	16},{13.62,	17},{13.093,18},{12.59,	19},
	{12.109,20},{11.649,21},{11.209,22},{10.788,23},{10.386,24},{10,	25},{9.631,	26},{9.277,	27},{8.939,	28},{8.614,	29},
	{8.303,	30},{8.005,	31},{7.719,	32},{7.445,	33},{7.182,	34},{6.93,	35},{6.688,	36},{6.456,	37},{6.233,	38},{6.019,	39},
	{5.813,	40},{5.616,	41},{5.426,	42},{5.243,	43},{5.068,	44},{4.9,	45},{4.738,	46},{4.582,	47},{4.432,	48},{4.287,	49},
	{4.149,	50},{4.015,	51},{3.886,	52},{3.763,	53},{3.643,	54},{3.528,	55},{3.418,	56},{3.311,	57},{3.208,	58},{3.109,	59},
	{3.013,	60},{2.921,	61},{2.832,	62},{2.747,	63},{2.664,	64},{2.584,	65},{2.507,	66},{2.433,	67},{2.361,	68},{2.292,	69},
	{2.225,	70},{2.16,	71},{2.098,	72},{2.037,	73},{1.979,	74},{1.923,	75},{1.868,	76},{1.815,	77},{1.764,	78},{1.715,	79},
	{1.668,	80},{1.621,	81},{1.577,	82},{1.534,	83},{1.492,	84},{1.451,	85}
};


const float LTR_Temperature_Table[LTR_RT_TABLE_LENGTH][2]=
{
	{200.82,-40},{190.05,-39},{179.91,-38},{170.36,-37},{161.36,-36},{152.88,-35},{144.89,-34},{137.35,-33},{130.25,-32},{123.54,-31},
	{117.21,-30},{111.24,-29},{105.61,-28},{100.29,-27},{95.26,-26},{90.51,-25},{86.02,-24},{81.78,-23},{77.77,-22},{73.98,-21},
	{70.4,-20},{67,-19},{63.79,-18},{60.75,-17},{57.87,-16},{55.14,-15},{52.56,-14},{50.11,-13},{47.79,-12},{45.59,-11},
	{43.51,-10},{41.53,-9},{39.65,-8},{37.87,-7},{36.18,-6},{34.57,-5},{33.04,-4},{31.59,-3},{30.21,-2},{28.9,-1},
	{27.66,0},{26.47,1},{25.35,2},{24.27,3},{23.25,4},{22.28,5},{21.35,6},{20.47,7},{19.63,8},{18.83,9},
	{18.07,10},{17.34,11},{16.64,12},{15.98,13},{15.35,14},{14.74,15},{14.167,16},{13.616,17},{13.09,18},{12.587,19},
	{12.106,20},{11.647,21},{11.207,22},{10.787,23},{10.385,24},{10,25},{9.632,26},{9.279,27},{8.941,28},{8.617,29},
	{8.307,30},{8.01,31},{7.725,32},{7.452,33},{7.19,34},{6.938,35},{6.697,36},{6.465,37},{6.243,38},{6.029,39},
	{5.824,40},{5.628,41},{5.438,42},{5.256,43},{5.081,44},{4.913,45},{4.752,46},{4.596,47},{4.446,48},{4.302,49},
	{4.164,50},{4.03,51},{3.902,52},{3.778,53},{3.658,54},{3.543,55},{3.433,56},{3.326,57},{3.223,58},{3.123,59},
	{3.028,60},{2.935,61},{2.846,62},{2.76,63},{2.677,64},{2.597,65},{2.519,66},{2.444,67},{2.372,68},{2.302,69},
	{2.235,70},{2.17,71},{2.107,72},{2.046,73},{1.987,74},{1.93,75},{1.875,76},	{1.821,77},{1.77,78},{1.72,79},
	{1.671,80},{1.625,81},{1.579,82},{1.535,83},{1.493,84},{1.452,85},{1.412,86},{1.373,87},{1.336,88},{1.299,89},
	{1.264,90},{1.23,91},{1.197,92},{1.165,93},	{1.134,94},	{1.104,95},	{1.075,96},	{1.046,97},	{1.019,98},	{0.992,99},
	{0.966,100},{0.941,101},{0.917,102},{0.893,103},{0.87,104},{0.848,105},{0.826,106},{0.805,107},{0.785,108},{0.765,109},
	{0.746,110},{0.727,111},{0.709,112},{0.691,113},{0.674,114},{0.657,115},{0.641,116},{0.626,117},{0.61,118},{0.596,119},
	{0.581,120}


};
struct NTC{
    signed short temp;
    unsigned short mV;
} ;

const struct NTC temp_table[]={
    {-40, 1708  },
    {-35, 1682  },
    {-30, 1651  },
    {-25, 1613 },
    {-20, 1568 },
    {-15, 1516, },
    {-10, 1456 },
    { -5, 1390 },
    {  0, 1317 },
    {  5, 1238 },
    { 10, 1156 },
    { 15, 1071 },
    { 20,  985 },
    { 25,  900 },
    { 30,  817 },
    { 35,  737 },
    { 40,  625 },
    { 45,  592 },
    { 50,  528 },
    { 55,  470 },
    { 60,  417 },
    { 65,  370 },
    { 70,  327 },
    { 75,  290 },
    { 80,  257 },            
};

#define ARY(paddr,a,b)  *(paddr+((a)*2)+(b))
float ResisToTemperature(float NTCResistance ,float * paddr , uint16_t Table_Length)
{
    uint16_t j=0;
	float DeltaRper1C=0;
	float DeltaRperThisPos=0;
	float ReturnTemperature=0;
	float ReturnValue=0;

	float StartTable_Resistance=0;
	float FinalTable_Resistance=0;
	float CompareTable_Resistance=0;
	float LowTable_Resistance=0;
	float CompareTable_Temperature=0;



	StartTable_Resistance =ARY(paddr,0,0);
	FinalTable_Resistance =ARY(paddr,Table_Length-1,0);

	//------------------------------------------- Normal Routine ----------------------------------------------------
	if( NTCResistance > StartTable_Resistance)
	{//--- NCT is Open
		ReturnValue=NCT_OPEN_TEMPERATURE;
	}else if( NTCResistance < FinalTable_Resistance )
	{//--- NCT is Short
		ReturnValue=NCT_SHORT_TEMPERATURE;
	}else
	{//--- NCT is Normal
		for(j=1 ; j < Table_Length ;j++ )
		{
			CompareTable_Resistance =ARY(paddr,j,0);
			if(NTCResistance > CompareTable_Resistance)
			{
				LowTable_Resistance=ARY(paddr,j-1,0);
				CompareTable_Temperature=ARY(paddr,j,1);
				DeltaRper1C=LowTable_Resistance-CompareTable_Resistance;
				DeltaRperThisPos=NTCResistance-CompareTable_Resistance;
				ReturnTemperature=(CompareTable_Temperature-(DeltaRperThisPos/DeltaRper1C))*100;
				ReturnValue= ReturnTemperature;
				break;
			}
		}
	}
	return ReturnValue;
}


int32_t Round(float VoltageOrTemper)
{
	int32_t int32TempData;
	int32_t ReturnData;
	float Delta;


	int32TempData=(int32_t)(VoltageOrTemper);
	Delta=VoltageOrTemper-int32TempData;

	if(Delta >= 0)
	{
		if(Delta >= 0.5f) ReturnData=int32TempData+1;
		else ReturnData= int32TempData;
	}else
	{
		if(Delta < -0.5f) ReturnData=int32TempData-1;
		else ReturnData=int32TempData;
	}

	return ReturnData;
}

static uint8_t aux_tmp_count = 0;

PUBLIC void BMIC_Read_Temperature(uint8_t ch)
{
    float x1,x2,y1,y2,x0;
    int acc;
    uint8_t n,tmpcnt, pos;
    uint16_t tempTemperature;
    int mvol;
    int i;    
    Tray[0].Module[0].aux[0].data = ir.model.temp1;
    Tray[0].Module[0].aux[1].data = ir.model.temp2;
    Tray[0].Module[0].aux[2].data = ir.model.temp3;
    

    for(tmpcnt = 0;tmpcnt< EEPROMData.base.TrTmpCnt  ;tmpcnt++)
    {
        mvol = 900*(Tray[0].Module[0].aux[tmpcnt].data - 22 )/(4730 - 22);
        for(i=0; i< (sizeof(temp_table)/sizeof (struct NTC)-1); i++){
            if( mvol >= temp_table[0].mV){
                break;
            }
            if( mvol < temp_table[i].mV && mvol >= temp_table[i+1].mV){
                break;
            }
        }

        switch(i){
            case 0:
               tempTemperature= (uint16_t)-4000;
                break;
            case sizeof(temp_table)/sizeof (struct NTC)-1:
               tempTemperature = 8000;
                break;
            default:
                x2 = temp_table[i+1].temp; x1 = temp_table[i].temp;
                y2 = temp_table[i+1].mV ; y1 = temp_table[i].mV;
                x0 = x2 - (mvol-y2)*(x2-x1)/(y1-y2);
                tempTemperature  = (uint16_t )(x0*100);
                break;
        }
       
        pos = Tray[0].Module_TempSensor[tmpcnt].samplePos;
        Tray[0].Module_TempSensor[tmpcnt].SampleTemp[pos] = tempTemperature;
        Tray[0].Module_TempSensor[tmpcnt].samplePos = (++ Tray[0].Module_TempSensor[tmpcnt].samplePos) % TEMP_SAMPLE_COUNT;
        acc = 0;
        for( n=0; n<TEMP_SAMPLE_COUNT; n++)
        {
            acc += Tray[0].Module_TempSensor[tmpcnt].SampleTemp[n] ;
        }

        if( aux_tmp_count >= TEMP_SAMPLE_COUNT){
            aux_tmp_count = TEMP_SAMPLE_COUNT;
        }else{
            aux_tmp_count++;
        }
        Tray[0].Module_TempSensor[tmpcnt].Temperature.all =(int16_t) (acc/aux_tmp_count);

    }
#if 0    
    for(i=0;i<TEMP_SAMPLE_COUNT;i++)
    {
        tempData = Tray[0].Module[0].aux[i].data;
        //STEP1. AD 데이터를 전압으로 치환
        tempVoltage= tempData/ 1000.0f;		                    //<===== 변경
        //STEP2. 전압을 다시 저항값으로 변경
        tempResistance = (tempVoltage*10.0f)/(3.0f -tempVoltage);  // <===== 변경
        //STEP3. Table에서 온도값 찾기
        FloaTemperature=	ResisToTemperature(tempResistance,( float*)LTR_Temperature_Table,LTR_RT_TABLE_LENGTH);  // Convert Resistance to Module Temperature
        tempTemperature=(int16_t)Round(FloaTemperature);
        
        pos = Tray[0].Module_TempSensor[tmpcnt].samplePos;
        Tray[0].Module_TempSensor[tmpcnt].SampleTemp[pos] = tempTemperature;
        Tray[0].Module_TempSensor[tmpcnt].samplePos = (++ Tray[0].Module_TempSensor[tmpcnt].samplePos) % TEMP_SAMPLE_COUNT;
        acc = 0;
        for( n=0; n<TEMP_SAMPLE_COUNT; n++)
        {
            acc += Tray[0].Module_TempSensor[tmpcnt].SampleTemp[n] ;
        }

        if( aux_tmp_count >= TEMP_SAMPLE_COUNT){
            aux_tmp_count = TEMP_SAMPLE_COUNT;
        }else{
            aux_tmp_count++;
        }
        Tray[0].Module_TempSensor[tmpcnt++].Temperature.all =(int16_t) (acc/aux_tmp_count);
    }
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//              RBMS intercommucate Data of TrayBMS
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _RBMS

void BMIC_ReadDeviceIDFromRBMS(void)
{
    int n;
    for(n=0; n <TRAY_NUMS; n++)
    {
       if( Tray[n].Module[0].DeviceID ==0xFF) continue;
       Tray[n].MbmsID = 100+ n;
       RbmsComTray[n].id = Tray[n].MbmsID ;
    }

}

void BMIC_ReadCellVolFromRBMS(void)
{
    int n,j,i;
    int cellcnt=0;
    for(n=0; n <TRAY_NUMS; n++)
    {
        cellcnt =0;
        for(i=0;i<BMIC_DEVICE_COUNT;i++)
        {
            for(j=0;j< CELL_ENABLE_COUNT;j++){
                if(Tray[n].Module[i].Cells[j].used){
#if 0
                    if( on_balancing && !on_bgbalancing && Balancing.BalancingStatus == CELLBALANCING ){
                        if(Tray[n].Module[i].Cells[j].CellVoltage.all < UNDER_CELL_V_FAULT_DETECT){
                            RbmsComTray[n].cellvol[cellcnt++] = UNDER_CELL_V_FAULT_DETECT;
                        }else
                        if( Tray[n].Module[i].Cells[j].CellVoltage.all > OVER_CELL_V_FAULT_DETECT){
                            RbmsComTray[n].cellvol[cellcnt++] = OVER_CELL_V_FAULT_DETECT;
                        }
                    }
                    else
                    {
                        RbmsComTray[n].cellvol[cellcnt++]= Tray[n].Module[i].Cells[j].CellVoltage.all;
                    }
#else
                        RbmsComTray[n].cellvol[cellcnt++]= Tray[n].Module[i].Cells[j].CellVoltage.all;
#endif
                }
            }
        }
        RbmsComTray[n].sum_sw_vol = Tray[n].TrayVoltage.CellSumBySW.all;
        RbmsComTray[n].sum_hw_vol = Tray[n].TrayVoltage.BlockByBMIC.all;
        RbmsComTray[n].maxCell.Vol = Tray[n].MaxCell.Voltage.all;
        RbmsComTray[n].maxCell.num = Tray[n].MaxCell.CellNumber;
        RbmsComTray[n].minCell.Vol = Tray[n].MinCell.Voltage.all;
        RbmsComTray[n].minCell.num = Tray[n].MinCell.CellNumber;
       // assert_param( cellcnt == TRAY_CELL_NUMS);
        cellcnt=0;
    }
}

void BMIC_ReadModTmpFromRBMS(void)
{
    int t,m;
    for(t=0; t <TRAY_NUMS; t++)
    {
        for( m =0; m < MODULE_TEMP_SENSORNUM;m++)
        {
            RbmsComTray[t].tmp[m]=Tray[t].Module_TempSensor[m].Temperature.all;
        }
        for( m=0; m < MODULE_BDTEMP_SENSORNUM; m++)
        {
            RbmsComTray[t].brdtmp[m]=Tray[t].Board_TempSensor[m].Temperature.all;
        }
    }
}

void BMIC_ReadTrayInterLockFromRBMS(void)
{
    int t;
    for(t=0; t <TRAY_NUMS; t++)
    {
        if(Tray[t].unlock.data  < 3000 && Tray[t].unlock.data != 0 ){ 
            RbmsComTray[t].unlock = 1;
        }else{
            RbmsComTray[t].unlock = 0;
        }
            
    }

}

void BMIC_ReadCellBalancingFromRBMS(void)
{
    int n,j,i;
    int cellcnt=0;
    for(n=0; n <TRAY_NUMS; n++)
    {
        for( i =0; i < BMIC_DEVICE_COUNT; i++)
        {
            for(j =0; j < CELL_ENABLE_COUNT  ; j++){
                if(Tray[n].Module[i].Cells[j].used){
                    RbmsComTray[n].cellbal[cellcnt++] = Tray[n].Module[i].Cells[j].OnBalancing;
                }
            }
        }
        //assert_param( cellcnt == TRAY_CELL_NUMS);
        cellcnt=0;
    }

}

void BMIC_ReadTrayFaultFromRBMS(void)
{
    int t;
    for(t=0; t <TRAY_NUMS; t++)
    {
        RbmsComTray[t].fault = Tray[t].bmic_fault;
    }

}
#endif
