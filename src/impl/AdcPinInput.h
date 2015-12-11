#ifndef ADCPININPUTBASE_H_
#define ADCPININPUTBASE_H_

#include "AdcMuxInput.h"
#include "Pin.h"

class AdcPinInput : public AdcMuxInput {

public:

  virtual ~AdcPinInput() {}

  void select();

  virtual Pin& pin() = 0;

protected:

  AdcPinInput(uint8_t pinMuxIndex) : AdcMuxInput(pinMuxIndex) {}

};

#endif
