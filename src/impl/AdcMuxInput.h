#ifndef ADCMUXINPUT_H_
#define ADCMUXINPUT_H_

#include <stdint.h>

class AdcMuxInput {

private:

  uint8_t m_muxIndex;

public:

  AdcMuxInput(uint8_t muxIndex) : m_muxIndex(muxIndex) {}

  uint8_t getMuxIndex() const { return m_muxIndex; }

};

#endif
