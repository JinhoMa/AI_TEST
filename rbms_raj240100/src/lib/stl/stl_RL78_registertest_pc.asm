;*******************************************************************************
;* DISCLAIMER
;* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
;* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
;* applicable laws, including copyright laws. 
;* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
;* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
;* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
;* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
;* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
;* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
;* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
;* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
;* following link:
;* http://www.renesas.com/disclaimer
;* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
;*******************************************************************************
;*******************************************************************************
;* File Name    : stl_RL78_registertest_pc.asm
;* Version      : 3.00
;* Device(s)    : RL78 (Test device used RL78/G14)
;* Tool-Chain   : RL78 CC-RL V1.08.00
;* OS           : None
;* H/W Platform : RL78/G14 Target Board
;* Description  : RL78 IEC Self Test Library PC register test module
;                The data extension Register (PC) is tested and restored by this
;
;* Limitations : Any dependency of the user application in that it might be
;                necessary to disable interrupts while testing.
;*******************************************************************************
;* History :
;* 28.02.2019 V1.00 First Release
;* 27.09.2019 V3.00 Extened Version
;*******************************************************************************

    .PUBLIC  _stl_RL78_registertest_pc      ; Global Name Declaration

  	.DSEG SBSS
;******************************************************************************
; Private variables and functions
;******************************************************************************

        .CSEG   CONSTF

ADR_PC_CHK_2:
        .DB4     PC_CHK_2

        .CSEG

PC_CHK_1:
;Let the return address be the return value (L, DE)
        MOV     A,[SP+2]
        MOV     L,A
        MOVW    AX,[SP+0]
        MOVW    DE,AX
        RET

;******************************************************************************
;* Outline : stl_RL78_registertest_pc - RL78 IEC PC Register test module
;* Include : none
;* Declaration : stl_RL78_registertest_pc
;* Function Name: stl_RL78_registertest_pc
;* Description : RL78 IEC PC Register test code module
;* Argument : none
;* Return Value : Register A - Result status (Pass = 0x00, Fail = 0x01)
;* Calling Functions : main.c
;******************************************************************************
;* Start of stl_RL78_registertest_pc.asm module
_stl_RL78_registertest_pc:
        PUSH    DE
        PUSH    HL
        PUSH    AX

        CALL    !!PC_CHK_1
call_ret_1:
        MOVW    AX,DE
        CMPW    AX,#LOWW (call_ret_1)
        BNZ     $out_error
;
        MOV     A,L
        CMP     A,#LOW(HIGHW(call_ret_1))
        BNZ     $out_error
;
        MOV     ES,#LOW(HIGHW(ADR_PC_CHK_2))
        MOVW    AX,ES:!ADR_PC_CHK_2
        MOVW    DE,AX
        MOV     A,ES:!ADR_PC_CHK_2+2
        MOV     CS,A
        CALL    DE

call_ret_2:
        MOVW    AX,DE
        CMPW    AX,#LOWW (call_ret_2)
        BNZ     $out_error
;
        MOV     A,L
        CMP     A,#LOW(HIGHW(call_ret_2))
        BNZ     $out_error
;
        MOV     L, #0x00                        ; Return no error (0x00) to calling function
        BR      $test_finished
out_error:
        MOV     L, #0x01                        ; Return error (0x01) to calling function
test_finished:
        POP     AX
        MOV     A,L
        POP     HL
        POP     DE
        ret

PC_CHK_2:
;Let the return address be the return value (L, DE)
        MOV     A,[SP+2]
        MOV     L,A
        MOVW    AX,[SP+0]
        MOVW    DE,AX
        RET

