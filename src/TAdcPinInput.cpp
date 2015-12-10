/*
 * AdcPinInput.cpp
 *
 *  Created on: Jun 3, 2015
 *      Author: gbumgard
 */

#include "TAdcPinInput.h"

volatile uint8_t& PortB::PORT = PORTB;
volatile uint8_t& PortB::PIN = PINB;
volatile uint8_t& PortB::DDR = DDRB;

volatile uint8_t& PortC::PORT = PORTC;
volatile uint8_t& PortC::PIN = PINC;
volatile uint8_t& PortC::DDR = DDRC;

volatile uint8_t& PortD::PORT = PORTD;
volatile uint8_t& PortD::PIN = PIND;
volatile uint8_t& PortD::DDR = DDRD;

volatile uint8_t& PortE::PORT = PORTE;
volatile uint8_t& PortE::PIN = PINE;
volatile uint8_t& PortE::DDR = DDRE;

volatile uint8_t& PortF::PORT = PORTF;
volatile uint8_t& PortF::PIN = PINF;
volatile uint8_t& PortF::DDR = DDRF;


TAdcPinInput<0> TAdcPinInput<0>::s_singleton;
TAdcPinInput<1> TAdcPinInput<1>::s_singleton;
TAdcPinInput<6> TAdcPinInput<6>::s_singleton;
TAdcPinInput<7> TAdcPinInput<7>::s_singleton;
TAdcPinInput<8> TAdcPinInput<8>::s_singleton;
TAdcPinInput<9> TAdcPinInput<9>::s_singleton;
TAdcPinInput<10> TAdcPinInput<10>::s_singleton;
TAdcPinInput<11> TAdcPinInput<11>::s_singleton;
TAdcPinInput<12> TAdcPinInput<12>::s_singleton;
TAdcPinInput<13> TAdcPinInput<13>::s_singleton;
