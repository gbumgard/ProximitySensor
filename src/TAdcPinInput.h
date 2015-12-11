/*
 * AdcPinInput.h
 *
 *  Created on: Jun 3, 2015
 *      Author: gbumgard
 */

#ifndef ADCPININPUT_H_
#define ADCPININPUT_H_

#include <stdint.h>
#include <avr/pgmspace.h>
#include <impl/AdcPinInput.h>
#include <impl/Pin.h>

struct PortB {
  static volatile uint8_t& PORT;
  static volatile uint8_t& PIN;
  static volatile uint8_t& DDR;
};

struct PortC {
  static volatile uint8_t& PORT;
  static volatile uint8_t& PIN;
  static volatile uint8_t& DDR;
};

struct PortD {
  static volatile uint8_t& PORT;
  static volatile uint8_t& PIN;
  static volatile uint8_t& DDR;
};

struct PortE {
  static volatile uint8_t& PORT;
  static volatile uint8_t& PIN;
  static volatile uint8_t& DDR;
};

struct PortF {
  static volatile uint8_t& PORT;
  static volatile uint8_t& PIN;
  static volatile uint8_t& DDR;
};

/**
 * Extends AdcPinInput class to provide a base template declaration for
 * pin-specific template specialization.
 * The various specializations can be treated as singletons and
 * accessed via the static instance() method defined by each.
 */
template<int MUX_INDEX> class TAdcPinInput : public AdcPinInput {
};

/**
 * Template specialization for pin 0.
 */
template<> class TAdcPinInput<0> : public AdcPinInput {
public:

  typedef TPin<PortF, PF0> PinType;

  TAdcPinInput()
  : AdcPinInput(0) {
    DIDR0 |= _BV(PF0);
  }

  virtual ~TAdcPinInput() {}

  Pin& pin() { return m_pin; }

  /**
   * Returns singleton instance of this specialization of the TAdcPinInput template.
   */
  static TAdcPinInput& instance() {
    return s_singleton;
  }

protected:
  PinType m_pin;
  static TAdcPinInput s_singleton;
};

template<> class TAdcPinInput<1> : public AdcPinInput {
public:

  typedef TPin<PortF,PF1> PinType;

  TAdcPinInput()
  : AdcPinInput(1) {
    DIDR0 |= _BV(PF1);
  }

  virtual ~TAdcPinInput() {}

  Pin& pin() { return m_pin; }

  /**
   * Returns singleton instance of this specialization of the TAdcPinInput template.
   */
  static TAdcPinInput& instance() {
    return s_singleton;
  }

protected:
  PinType m_pin;
  static TAdcPinInput s_singleton;
};

template<> class TAdcPinInput<6> : public AdcPinInput {
public:

  typedef TPin<PortF,PF6> PinType;

  TAdcPinInput()
  : AdcPinInput(6) {
    DIDR0 |= _BV(PF6);
  }

  virtual ~TAdcPinInput() {}

  Pin& pin() { return m_pin; }

  /**
   * Returns singleton instance of this specialization of the TAdcPinInput template.
   */
  static TAdcPinInput& instance() {
    return s_singleton;
  }

protected:
  PinType m_pin;
  static TAdcPinInput s_singleton;
};

template<> class TAdcPinInput<7> : public AdcPinInput {
public:
  typedef TPin<PortF,PF7> PinType;

  TAdcPinInput()
  : AdcPinInput(7) {
    DIDR0 |= _BV(PF7);
  }

  virtual ~TAdcPinInput() {}

  Pin& pin() { return m_pin; }

  /**
   * Returns singleton instance of this specialization of the TAdcPinInput template.
   */
  static TAdcPinInput& instance() {
    return s_singleton;
  }

protected:
  PinType m_pin;
  static TAdcPinInput s_singleton;
};


template<> class TAdcPinInput<8> : public AdcPinInput {
public:
  typedef TPin<PortD,PD4> PinType;

  TAdcPinInput()
  : AdcPinInput(0b100000) {
    DDRD &= ~(_BV(PD4));
    DIDR2 |= _BV(0);
  }

  virtual ~TAdcPinInput() {}

  Pin& pin() { return m_pin; }

  /**
   * Returns singleton instance of this specialization of the TAdcPinInput template.
   */
  static TAdcPinInput& instance() {
    return s_singleton;
  }

protected:
  PinType m_pin;
  static TAdcPinInput s_singleton;
};

template<> class TAdcPinInput<9> : public AdcPinInput {
public:
  typedef TPin<PortD,PD6> PinType;

  TAdcPinInput()
  : AdcPinInput(0b100001) {
    DDRD &= ~(_BV(PD6));
    DIDR2 |= _BV(1);
  }

  virtual ~TAdcPinInput() {}

  Pin& pin() { return m_pin; }

  /**
   * Returns singleton instance of this specialization of the TAdcPinInput template.
   */
  static TAdcPinInput& instance() {
    return s_singleton;
  }

protected:
  PinType m_pin;
  static TAdcPinInput s_singleton;
};

template<> class TAdcPinInput<10> : public AdcPinInput {
public:
  typedef TPin<PortD,PD7> PinType;

  TAdcPinInput()
  : AdcPinInput(0b100010) {
    DDRD &= ~(_BV(PD7));
    DIDR2 |= _BV(2);
  }

  virtual ~TAdcPinInput() {}

  Pin& pin() { return m_pin; }

  /**
   * Returns singleton instance of this specialization of the TAdcPinInput template.
   */
  static TAdcPinInput& instance() {
    return s_singleton;
  }

protected:
  PinType m_pin;
  static TAdcPinInput s_singleton;
};

template<> class TAdcPinInput<11> : public AdcPinInput {
public:
  typedef TPin<PortB,PB4> PinType;

  TAdcPinInput()
  : AdcPinInput(0b100011) {
    DDRB &= ~(_BV(PB4));
    DIDR2 |= _BV(3);
  }

  virtual ~TAdcPinInput() {}

  Pin& pin() { return m_pin; }

  /**
   * Returns singleton instance of this specialization of the TAdcPinInput template.
   */
  static TAdcPinInput& instance() {
    return s_singleton;
  }

protected:
  PinType m_pin;
  static TAdcPinInput s_singleton;
};

template<> class TAdcPinInput<12> : public AdcPinInput {
public:
  typedef TPin<PortB,PB5> PinType;

  TAdcPinInput()
  : AdcPinInput(0b100100) {
    DDRB &= ~(_BV(PB5));
    DIDR2 |= _BV(4);
  }

  virtual ~TAdcPinInput() {}

  Pin& pin() { return m_pin; }

  /**
   * Returns singleton instance of this specialization of the TAdcPinInput template.
   */
  static TAdcPinInput& instance() {
    return s_singleton;
  }

protected:
  PinType m_pin;
  static TAdcPinInput s_singleton;
};

template<> class TAdcPinInput<13> : public AdcPinInput {
public:
  typedef TPin<PortB,PB6> PinType;

  TAdcPinInput()
  : AdcPinInput(0b100101) {
    DDRB &= ~(_BV(PB5));
    DIDR2 |= _BV(5);
  }

  virtual ~TAdcPinInput() {}

  Pin& pin() { return m_pin; }

  /**
   * Returns singleton instance of this specialization of the TAdcPinInput template.
   */
  static TAdcPinInput& instance() {
    return s_singleton;
  }

protected:
  PinType m_pin;
  static TAdcPinInput s_singleton;
};

#endif /* ADCPININPUT_H_ */
