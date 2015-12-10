/*
 * Fraction.h
 *
 *  Created on: Sep 25, 2015
 *      Author: gbumgard
 */

#ifndef FRACTION_H_
#define FRACTION_H_

struct Fraction {

  uint32_t m_multiplier;
  uint32_t m_divisor;

  Fraction()
  : m_multiplier(0)
  , m_divisor(1)
  {

  }

  Fraction(uint32_t multiplier, uint32_t divisor)
  : m_multiplier(multiplier)
  , m_divisor(divisor)
  {
  }

  Fraction(const Fraction& fraction)
  : m_multiplier(fraction.m_multiplier)
  , m_divisor(fraction.m_divisor)
  {
  }

  Fraction& operator=(const Fraction& fraction) {
    m_multiplier = fraction.m_multiplier;
    m_divisor = fraction.m_divisor;
    return *this;
  }
} __attribute__ ((packed));

#endif /* FRACTION_H_ */
