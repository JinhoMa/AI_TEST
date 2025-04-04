/**
 ******************************************************************************
 *     @file         hvp_bsp.c
 *     @version   
 *     @author    
 *     @date      2024/10/17 
 *     @brief     
 ******************************************************************************
*/
#include "define.h"														// union/define definition
#include "afe.h"														// AFE window 0
#include "afe1.h"														// AFE window 1


BYTE hvp_window;
BYTE out_reg;
void AFE_HVP_INIT(void)
{
    AFE_RD(AFE_WINCHG, hvp_window);
    AFE_WR(AFE_PMHV,0x07);                                 // All Input Mode
    AFE_WR(AFE_POMHV,0x30);                               // Output Configuration
    AFE_WR(AFE_PHV,0x00);                                    //All Output low
    AFE_WR(AFE_PMHV,0x00);                                 //Output Mode
    out_reg = 0; 
}

void AFE_HVP0_Hi(void)
{
    AFE_RD(AFE_WINCHG, hvp_window);
    out_reg |=0x1;
    AFE_WR(AFE_PHV,out_reg);                                    // HVP0 Output High
}

void AFE_HVP0_Lo(void)
{
    AFE_RD(AFE_WINCHG, hvp_window);
    out_reg &=~0x1;
    AFE_WR(AFE_PHV,out_reg);                                    // HVP0 Output low
}

void AFE_HVP1_Hi(void)
{
    AFE_RD(AFE_WINCHG, hvp_window);
    out_reg |=0x2;
    AFE_WR(AFE_PHV,out_reg);                                    // HVP0 Output High
}

void AFE_HVP1_Lo(void)
{
    AFE_RD(AFE_WINCHG, hvp_window);
    out_reg &=~0x2;
    AFE_WR(AFE_PHV,out_reg);                                    // HVP0 Output low
}

