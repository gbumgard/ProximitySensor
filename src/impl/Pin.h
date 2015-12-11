/*
 * Pin.h
 *
 *  Created on: Jun 3, 2015
 *      Author: gbumgard
 */

#ifndef PIN_H_
#define PIN_H_

class Pin {
public:
  virtual ~Pin() {}
  virtual void startCharge() = 0;
  virtual void stopCharge() = 0;
  virtual void startDischarge() = 0;
  virtual void stopDischarge() = 0;
};

template<typename TPort,int PIN> class TPin : public Pin {
public:
  virtual ~TPin() {}
  void startCharge();
  void stopCharge();
  void startDischarge();
  void stopDischarge();
};

template<typename TPort, int PIN> void TPin<TPort,PIN>::startCharge() {
  TPort::PORT |= (1 << PIN); // set on or connect pull-up).
  TPort::DDR |= (1 << PIN);  // select output mode (drive-high)
}

template<typename TPort, int PIN> void TPin<TPort,PIN>::stopCharge() {
  TPort::DDR &= ~(1 << PIN);  // select input mode
  TPort::PORT &= ~(1 << PIN); // disconnect pull-up (tri-state)
}

template<typename TPort, int PIN> void TPin<TPort,PIN>::startDischarge() {
  TPort::PORT &= ~(1 << PIN); // set off or disconnect pull-up
  TPort::DDR |= (1 << PIN);   // select output mode (drive-low)
}

template<typename TPort, int PIN> void TPin<TPort,PIN>::stopDischarge() {
  TPort::PORT &= ~(1 << PIN); // disconnect pull-up (tri-state)
  TPort::DDR &= ~(1 << PIN);  // select input mode
}


#endif /* PIN_H_ */
