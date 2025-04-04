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
;* File Name    : stl_RL78_RamTest_Stacks_x.asm
;* Version      : 3.00
;* Device(s)    : RL78 (Test device used RL78/G14)
;* Tool-Chain   : RL78 CC-RL V1.08.00
;* OS           : None
;* H/W Platform : RL78/G14 Target Board
;* Description  : RL78 IEC Self Test Library Stack test by March X assembler module
;                The "Worklength" byte in the currently used stack area is saved to 
;                "pWork" Perform memory test of stack area.
;                During the test, use the area specified by "pNewSp" as the stack area.
;      Register "AX" contains the address of the stack test structure
;
;     Register "C" returns 0x01 if a memory fault was detected, otherwise
;     0x00 is returned

;* Limitations : Any dependency of the user application in that it might be
;                necessary to disable interrupts while testing.
;*******************************************************************************
;* History :
;* 28.01.2019 V1.00 First Release
;* 27.09.2019 V3.00 Extened Version
;*******************************************************************************

.EXTERN  _stl_RL78_march_x
$IF (__RENESAS_VERSION__ >= 0x01010000)
$ELSE	; for CC-RL V1.00
.EXTRN	_stacktop
$ENDIF

     .PUBLIC  _stl_RL78_RamTest_Stacks_x            ; Global Name Declaration

	.CSEG


;******************************************************************************
;* Outline : stl_RL78_RamTest_Stacks_c - RL78 IEC strack area test module
;* Include : none
;* Declaration : stl_RL78_RamTest_Stacks_c:
;* Function Name: stl_RL78_RamTest_Stacks_c
;* Description : strack area test code module
;* Argument: Register "AX" contains address of the stack test structure
;*           Registers AX, BC, DE, H of Bank1 are used to buffer 
;*           Registers AX ,BC, D  of Bank2 are used to buffer 
;* Return Value : Register C - Result status (Pass = 0x00, Fail = 0x01)
;* Calling Functions : main.c
;******************************************************************************
;* Start of stl_RL78_RamTest_Stacks_x.asm code
_stl_RL78_RamTest_Stacks_x:

;* store input parameter
    PUSH    BC                      ; save BC on stack
    PUSH    AX                      ; save AX on stack
    PUSH    DE                      ; save DE on stack
    PUSH    HL                      ; save HL on stack
;
    MOVW    HL,AX                   ; HL address of the stack test structure
;
    MOVW    AX,SP
    MOVW    0xFFEE8,AX              ; save SP on BANK(2) AX
;
    MOVW    AX,[HL+6]               ; load new sp
    MOVW    SP,AX
;
    MOVW    AX,[HL+2]               ; Test size
    MOVW    BC,AX
    MOVW    AX,[HL]                 ; The address of the area for saving the contents of the stack
    CALL    !!_stl_RL78_march_x     ; Test of stack evacuation area
    MOV     C,A
    CMP     A,#0
    BNZ     $test_finished          ; retuen error
;
$IF (__RENESAS_VERSION__ >= 0x01010000)
	MOVW	AX,#LOWW(__STACK_ADDR_START)
$ELSE	; for CC-RL V1.00
	MOVW	AX,#LOWW(_stacktop)
$ENDIF
    MOVW    BC,AX
    MOVW    AX,[HL+4]               ; stack offset
    XCHW    AX,BC
    SUBW    AX,BC
    MOVW    BC,AX
    MOVW    AX,[HL+2]               ; test size
    XCHW    AX,BC
    SUBW    AX,BC
    MOVW    0xFFEEA,AX              ; save stack test start address on BANK(2) BC
    MOVW    DE,AX
    PUSH    HL
    MOVW    AX,[HL]                 ; save area
    MOVW    HL,AX
    MOVW    AX,BC
    ADDW    AX,HL
    MOVW    BC,AX
;
loop_1:                            ; Evacuation of stack area
    MOV     A,[DE]
    MOV     [HL],A
;
    INCW    DE
    INCW    HL
    MOVW    AX,BC
    CMPW    AX,HL
    BNZ     $loop_1
;
    POP     HL
;
    MOVW    AX,[HL+2]               ; AX Stack test area size
    MOVW    BC,AX
    MOVW    AX,0xFFEEA              ; AX Stack start address
    CALL    !!_stl_RL78_march_x     ; Test of stack area
    MOV     0xFFEED,A               ; retun value on BANK(2) D
;
    MOVW    AX,0xFFEEA              ; AX Stack start address
    MOVW    DE,AX
    PUSH    HL
    MOVW    AX,[HL]                 ; save area
    MOVW    HL,AX
    MOVW    AX,BC
    ADDW    AX,HL
    MOVW    BC,AX
;
loop_2:                            ; Returning the stack area
    MOV     A,[HL]
    MOV     [DE],A
;
    INCW    DE
    INCW    HL
    MOVW    AX,BC
    CMPW    AX,HL
    BNZ     $loop_2
;
    POP     HL
;
    MOVW    AX,0xFFEE8
    MOVW    SP,AX                   ; Restore SP from BANK(2) AX
;
    MOV     A,0xFFEED               ; Restore return value from BANK(2) D
    MOV     C,A
test_finished:
    POP     HL                      ; Restore HL from stack
    POP     DE                      ; Restore DE from stack
    POP     AX                      ; Restore AX
    MOV     A,C
    POP     BC                      ; Restore BC from stack
    ret
