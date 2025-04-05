/*
 * File:   main.c
 * Author: Chiper
 *
 * Created on 22 марта 2025 г., 15:19
 */
#include "config.h"
#include "defined.h"
#include "epp.h"

#define _myDEBUG

void(* i2cStepExec)(void);
void(* lmStepExec)(void);

void i2cInit(void);
void i2cOpen(void);
void i2cClose(void);
void i2cIdleStep(void);

void idleLM75(void);
void startLM75(void);
void exitLM75(void);
void stepLM75(void);

void initChip(void);
void initOpenCAN(void);
void initCAN(void);
void startTimers(void);
void initINT(void);
void canReceived(void);
void timeCount(void);
void makeSYNC(void);
void canSend(void);
void i2cInit(void);


extern __eeprom ee_t _ee;
volatile sys_flag_t flags;
volatile timer_t timer;
volatile core_t core;

#ifdef _myDEBUG
void initDebug(void)
{
	TRISAbits.TRISA5 = 0;
	TRISAbits.TRISA4 = 0;
	TRISAbits.TRISA2 = 0;
	TRISAbits.TRISA1 = 0;
	TRISAbits.TRISA0 = 0;
	TRISCbits.TRISC2 = 0;
	TRISCbits.TRISC3 = 0;
	PORTAbits.RA0 = 0;
	PORTAbits.RA1 = 0;
	PORTAbits.RA2 = 0;
	PORTAbits.RA4 = 0;
	PORTAbits.RA5 = 0;
	PORTCbits.RC2 = 0;
	PORTCbits.RC3 = 0;
	PORTAbits.RA0 = PORTAbits.RA0^1;
	PORTAbits.RA1 = PORTAbits.RA1^1;
	PORTAbits.RA2 = PORTAbits.RA2^1;
	PORTAbits.RA4 = PORTAbits.RA4^1;
	PORTAbits.RA5 = PORTAbits.RA5^1;
	PORTCbits.RC2 = PORTCbits.RC3^1;
	PORTCbits.RC3 = PORTCbits.RC3^1;
}
#endif

void main(void)
{
	NOP();

	initChip();
#ifdef _myDEBUG
	initDebug();
#endif
	i2cInit();
	initOpenCAN();
	initCAN();
	startTimers();
	initINT();

	i2cOpen();
//	lmStepExec = idleLM75;
	i2cStepExec = i2cIdleStep;

	while (1) {
		canReceived();
//		lmStepExec();

		if (flags.sys) {
			flags.sys = 0; // сброс флаг 1мс
			timeCount();
			//makeSYNC();
		}

		if (flags.can) {
			flags.can = 0;
		}
		
		if (flags.sec) {
			flags.sec = 0;
			startLM75();
		}
		canSend();
		CLRWDT();
	}
	i2cClose();
	return;
}