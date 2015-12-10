/*
 * timer.cpp
 *
 *  Created on: Jun 8, 2015
 *      Author: gbumgard
 */

#include "timer.h"
#include <avr/interrupt.h>

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (F_CPU / 1000L) )

#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

SIGNAL(TIMER0_OVF_vect)
{
  unsigned long m = timer0_millis;
  unsigned char f = timer0_fract;

  m += MILLIS_INC;
  f += FRACT_INC;
  if (f >= FRACT_MAX) {
    f -= FRACT_MAX;
    m += 1;
  }

  timer0_fract = f;
  timer0_millis = m;
  timer0_overflow_count++;
}

void timer_init() {
  cli();
  TCCR0A  =  0;                         // reset register
  TCCR0B = _BV(CS01) | _BV(CS00);
  TCNT0 = 0;
  TIMSK0 |= _BV(TOIE0);
  sei();
}

unsigned long millis()
{
  unsigned long m;
  uint8_t oldSREG = SREG;

  cli();
  m = timer0_millis;
  SREG = oldSREG;
  sei();

  return m;
}

unsigned long micros() {
  unsigned long m;
  uint8_t oldSREG = SREG, t;

  cli();
  m = timer0_overflow_count;
  t = TCNT0;


  if ((TIFR0 & _BV(TOV0)) && (t < 255))
    m++;

  SREG = oldSREG;
  sei();

  return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}