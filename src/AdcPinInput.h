#ifndef ADCPININPUTBASE_H_
#define ADCPININPUTBASE_H_

#include "AdcMuxInput.h"
#include <avr/pgmspace.h>
#include "Pin.h"

class AdcPinInput : public AdcMuxInput {

public:

  virtual ~AdcPinInput() {}

  void select() {
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

  virtual Pin& pin() = 0;


protected:

  AdcPinInput(uint8_t pinMuxIndex) : AdcMuxInput(pinMuxIndex) {}

};

#endif
