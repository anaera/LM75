
//PAPA----------------------------------------------------------
/**
 * \file
 * \brief Обработчик прерываний
 * 
 * Таймер T0
 * формирование интервалапрерываний каждые 50мкс,
 * счетчик интервала 1мс,
 * формирование межпакетного интервала неактивности шины,
 * переход в режим передачи при неактивности шины.
 *
 * Приемник USART
 * прием байта в прерывании,
 * последующий вывод байта в режиме передачи.
 *
 * Контроль активности шины по прерыванию INT0
 * обнаружение падения уровня на шине (активности шины).
 *
 */

#include "defined.h"
#include "ucan.h"

void(* i2cStepExec)(void);
void timeCtlEngine(void);
void setStateReceive(void);
void packActEngine(void);

extern volatile uint8_t divCnt;
extern tConst CONST;
extern volatile sys_flag_t flags;

void __interrupt() fullINT(void)
{

	if (PIR1bits.TMR1IF && PIE1bits.TMR1IE) {
		PIR1bits.TMR1IF = 0;
		TMR1H = _TMR1H;
		TMR1L = _TMR1L;
		flags.sys = 1; // взводим флаг 1мс
	}

	if (INTCONbits.TMR0IF && INTCONbits.TMR0IE) {
		INTCONbits.TMR0IF = 0;
		//        TMR0 = 0xFF;
		TMR0 = _TMR0;
		divCnt--;
		if (!divCnt) {
			divCnt = CONST.gapMult;
			timeCtlEngine(); //каждый интервал GAP = 100мс*gap.Mult вызываем обработчик
		}
	}

	//	if (INTCONbits.INTE && INTCONbits.INTF) {
	//		setStateReceive(); //переход в режим приема
	//	}
	if (INTCONbits.IOCIE && INTCONbits.IOCIF) {
		if (IOCAFbits.IOCAF3) setStateReceive(); //переход в режим приема
	}

	if (PIE1bits.RCIE && PIR1bits.RCIF) {
		PIR1bits.RCIF = 0;
		packActEngine();
	}
	if (INTCONbits.PEIE == 1) {
		if (PIE2bits.BCL1IE == 1 && PIR2bits.BCL1IF == 1) {
			PIR2bits.BCL1IF = 0;
		}
		if (PIE1bits.SSP1IE == 1 && PIR1bits.SSP1IF == 1) {
			PIR1bits.SSP1IF = 0;

			i2cStepExec();
		}

	}

}

//----------------------------------------------------------------------

