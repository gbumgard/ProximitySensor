/*
 * AdcPintInputBase.cpp
 *
 *  Created on: Jun 3, 2015
 *      Author: gbumgard
 */

#include "AdcPinInput.h"

#include <avr/pgmspace.h>

void AdcPinInput::select() {
  ADMUX &= ~0b11111;
  uint8_t index = getMuxIndex();
  ADMUX |= (index & 0x1F);
  if (index & 0x20) {
    ADCSRB |= _BV(MUX5);
  }
  else {
    ADCSRB &= ~(_BV(MUX5));
  }
}

