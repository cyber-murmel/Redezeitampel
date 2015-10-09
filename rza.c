#include <in430.h>
#include <msp430.h>
#include <stdnoreturn.h>

#ifndef __MSP430_HAS_ADC10__
# error This software needs an ADC10 to work
#endif

#define SECOND 1000000LU
#define DELAY  (5 * SECOND)
#define TICKSPERSEC ((int)(32768U/64))

/* definitions for the digital pins */
enum {
	P_GREEN	   = 0001,
	P_YELLOW   = 0002,
	P_RED	   = 0004,
	P_BELL	   = 0010,
	P_DURATION = 0020,	/* used from the ADC10 */
	P_RESET	   = 0040,
	P_STANDBY  = 0100,
	P_UNUSED   = 0200
};

/* pins that need to be pulled up or down */
#define P_UP (P_RESET | P_STANDBY)
#define P_DOWN P_UNUSED

/* the various aspects */
enum {
	ASP_OFF = P_UP,					/* no aspect */
	ASP_FULL = P_UP | P_GREEN,			/* most of the period remains */
	ASP_LOW = P_UP | P_YELLOW,			/* period is running out soon */
	ASP_ENDING = P_UP | P_RED | P_BELL,		/* period has just ended */
	ASP_END = P_UP | P_RED,				/* period has ended a while ago */
	ASP_RESETTING = P_UP | P_RED | P_YELLOW,	/* the reset button is depressed */
	ASP_ERROR = P_UP | P_RED | P_YELLOW | P_GREEN,	/* an error occured */
};

/* flags for the state machine */
enum {
	SFL_WAIT,	/* just wait for the time to elapse */
	SFL_TERMINAL,	/* this is the last state */
	SFL_RELEASE	/* wait for the reset button being released */
};

/*
 * The traffic light state machine. Each period consists of an aspect,
 * a set of flags and a time this state is held for. The time has the
 * form a + d * r where a is an absolute duration in seconds and r is
 * a relative duration multiplied with the duration setting.
 */
const struct state {
	unsigned char asp, a:2, r:2, flags:2;
} states[] = {
	{ ASP_RESETTING,	0, 0, SFL_RELEASE },
	{ ASP_FULL,		2, 0, SFL_WAIT },
	{ ASP_LOW, 		1, 0, SFL_WAIT },
	{ ASP_ENDING,		0, 3, SFL_WAIT },
	{ ASP_END,		0, 0, SFL_TERMINAL },
};

static void setup(void)
{
	/* disable watchdog */
	WDTCTL = WDTPW | WDTHOLD;

	/* set up clocks, do not set up BCSCTL1 as the defaults are ok */
	/* set internal capacitor to 12.5pF, ACLCK @ 4096 Hz */
	BCSCTL1 = XT2OFF | DIVA_3 | RSEL0 | RSEL1 | RSEL2;
	BCSCTL2 = SELS;
	BCSCTL3 = XCAP_3;

	__delay_cycles(255);	/* wait for oscilator to start */

	/* set up timer A to give us one interrupt per second */
	/* use ACLCK (512 ticks per second), up mode */
	TACTL = TASSEL_1 | ID_0 | MC_3 | TAIE;
	TACCR0 = 4096;		/* one interrupt per second */
	TACCTL0 = CCIE;		/* enable interrupts */

	/* set up IO ports as necessary */
	P1DIR = P_GREEN | P_YELLOW | P_RED | P_BELL;
	P1REN = P_UP | P_DOWN;
	P1OUT = P_UP;
	ADC10AE0 = P_DURATION;
}

/* get the value of the potentiometer */
static int sample(void)
{

	/* INCH = A4, ADC10SSEL = ACLCK */
	ADC10CTL1 = INCH2 | ADC10SSEL0;
	/* ADC10SR = 50kbps */
	ADC10CTL0 = ADC10SR | ADC10ON | ENC | ADC10SC;

	/* wait for data available */
	while (ADC10CTL1 & ADC10BUSY)
		;

	/* disable ADC10 */
	ADC10CTL0 &= ~ENC;

	return ADC10MEM;
}

/* general purpose handler: just wake up the CPU */
__attribute__((wakeup, interrupt(TIMER0_A0_VECTOR)))
extern void interrupt_handler(void)
{
	;
}

#if 0

/* indicate an error and then halt */
static noreturn void error(void)
{

	for (;;) {
		P1DIR = P_GREEN | P_YELLOW | P_RED | P_BELL;
		P1REN = P_UP | P_DOWN;
		P1OUT = ASP_ERROR;
		__low_power_mode_off_on_exit();
	}
}

#endif

extern int main()
{
	register int i, dur;

	setup();

	__eint();

	P1OUT = P_UP | P_GREEN | P_YELLOW;

	for (;;) {
		_low_power_mode_0();

		P1OUT ^= P_GREEN | P_YELLOW | P_RED | P_BELL;
	}
}
