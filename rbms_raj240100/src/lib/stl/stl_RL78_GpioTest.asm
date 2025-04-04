;*****************************************************************************
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
;/********************************************************************************/
;/* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.   */
;/********************************************************************************/
;/** File Name    : stl_RL78_GpioTest.h*/
;/** Version      : 3.00 */
;/** Device(s)    : RL78 (Test device used RL78/G14)*/
;/** Tool-Chain   : RL78 CC-RL V1.08.00
;/** OS           : None*/
;/** H/W Platform : RL78 Target Board*/
;/** Description  : RL78 IEC Self Test Library GPIO Test module */
;/** Limitations  : None*/
;/**********************************************************************************/
;/** History :*/
;* 30.09.2019 V3.00 Extened Version
;/********************************************************************************/

$include (stl_RL78_GpioTest.h)

	.PUBLIC		_stl_RL78_GpioTest

	.DSEG
TEST_DATA:   .DS   1

        .CSEG

;******************************************************************************

;* Outline : stl_RL78_GpioTest - RL78 IEC GPIO test module
;* Include : none
;* Declaration : stl_RL78_GpioTest
;* Function Name: stl_RL78_GpioTest
;* Description : RL78 IEC GPIO test code module
;* Argument : none
;* Return Value : Register A - Result status (Pass = 0x00, Fail = 0x01)
;* Calling Functions : main.c
;******************************************************************************
;* Start of stl_RL78_IoTest.asm module
_stl_RL78_GpioTest:
$IFDEF PMS
	CLR1		TEST_GPIO_MODE.TEST_GPIO_BIT	; Set test port to output

	MOVW            DE,#LOW(PMS)
	ONEB            A

	ADD             A,!TEST_DATA			; Update output value
	MOV             B,A
	MOV             !TEST_DATA,A

	MOV1            CY,A.0

	MOV             A,TEST_GPIO_PORT
	MOV1            A.TEST_GPIO_BIT,CY
	MOV             TEST_GPIO_PORT,A		; Output to GPIO
	

	MOV             A,[DE]				; Get the current value of the PMS register
	MOV             X,A
	MOV             [DE],#0x01			; Select the output level of the pin

	MOV             A,B
	XOR             A,TEST_GPIO_PORT		; Compare output value and pin level
	MOV1            CY,A.TEST_GPIO_BIT
	MOV		A,X
	MOV             [DE],A				; Restore PMS register settings
	BC    	        $_stl_RL78_IoTest_10		; Branches if the output value does not match the terminal level
;
	CLRB		A				; Return no error (0x00) to calling function
	BR		$_stl_RL78_IoTest_90
_stl_RL78_IoTest_10:
	ONEB		A				; Return error (0x01) to calling function
_stl_RL78_IoTest_90:
	RET
$ELSE
    CLRB		A				; Return no error (0x00) to calling function
	RET
$ENDIF
