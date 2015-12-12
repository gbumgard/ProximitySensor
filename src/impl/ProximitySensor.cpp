/*
 * ProximitySensor.cpp
 *
 *  Created on: Sep 29, 2014
 *      Author: gbumgard
 */

#include <stdlib.h>
#include <avr/interrupt.h>
#include <ProximitySensor.h>
#include <util/delay.h>

#ifdef AVR_PROJECT_BUILD
#include "timer.h"
#else
#include <Arduino.h>
#endif

#define DEFAULT_FILTER_ADAPTATION_RATE 4
#define DEFAULT_FILTER_RESEED_THRESHOLD 32

#define DEFAULT_PROXIMITY_THRESHOLD 32
#define DEFAULT_TOUCH_THRESHOLD 32
#define DEFAULT_RELEASE_THRESHOLD 8

#define DEFAULT_PROXIMITY_TIMEOUT_MS 10000
#define DEFAULT_TOUCH_TIMEOUT_MS 10000
#define DEFAULT_DELAY_MS 20
#define DEFAULT_RESOLUTION 7

/**
 * Function that selects compile-time generated delays required for randomization of capacitive charging cycle.
 */
static void random_delay() {
  switch(rand() / (RAND_MAX / 7)) {
  case 0:
    _delay_us(16);
    break;
  case 1:
    _delay_us(17);
    break;
  case 2:
    _delay_us(18);
    break;
  case 3:
    _delay_us(19);
    break;
  case 4:
    _delay_us(20);
    break;
  case 5:
    _delay_us(21);
    break;
  case 6:
    _delay_us(22);
    break;
  case 7:
    _delay_us(23);
    break;
  default:
    _delay_us(24);
    break;
  }
}

ProximitySensor::ProximitySensor(AdcPinInput* pReferencePin, AdcPinInput* pSensorPin )
: m_pReferencePin(pReferencePin)
, m_pSensorPin(pSensorPin)
, m_resolution(DEFAULT_RESOLUTION)
, m_filterAdaptationRate(DEFAULT_FILTER_ADAPTATION_RATE)
, m_filterReseedThreshold(DEFAULT_FILTER_RESEED_THRESHOLD)
, m_idleStartTimeMs(millis())
, m_proximityThreshold(DEFAULT_PROXIMITY_THRESHOLD)
, m_proximityTimeoutMs(DEFAULT_PROXIMITY_TIMEOUT_MS)
, m_proximityStartTimeMs(0)
, m_touchThreshold(DEFAULT_TOUCH_THRESHOLD)
, m_touchTimeoutMs(DEFAULT_TOUCH_TIMEOUT_MS)
, m_touchStartTimeMs(0)
, m_releaseThreshold(DEFAULT_RELEASE_THRESHOLD)
, m_delayMs(DEFAULT_DELAY_MS)
, m_delayStartTimeMs(0)
, m_movingAverage(0)
, m_state(IDLE)
, m_reseed(true)
, m_reseedSampleCount(0)
, m_onSampleCallbackData(0)
, m_onSampleCallback(0)
{
}

void ProximitySensor::begin() {
  // Configure ADC as required for proximity sensing
  // Set AVCC reference to 5V
  ADMUX = (1<<REFS0);
  // Set Prescaler to 128 -> clock 16MHz/128 = 125kHz
  ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
  // Enable the ADC
  ADCSRA |= (1<<ADEN);
}

uint32_t ProximitySensor::update() {

  size_t sampleCount = _BV(m_resolution);

  uint32_t total = 0;

  for (size_t i=0; i < sampleCount; i++) {

    cli();

    // Ground Mux and discharge S&H cap
    ADMUX |= 0b11111;

    // This delay had to be increased after introducing LED animation.
    // The S&H cap did not appear to be fully discharging before a new sample was taken.
    _delay_us(30);

    // Connect reference pin to S&H cap
    m_pReferencePin->select();
    // Charge S&H cap
    m_pReferencePin->pin().startCharge();
    // Discharge sensor cap
    m_pSensorPin->pin().startDischarge();

    random_delay();

    // Let sensor pin float
    m_pSensorPin->pin().stopDischarge();

    m_pSensorPin->select();

    uint16_t discharged = getAdcSample();

    // Connect reference pin to S&H cap
    m_pReferencePin->select();
    // Discharge S&H cap
    m_pReferencePin->pin().startDischarge();
    // Charge sensor cap
    m_pSensorPin->pin().startCharge();

    random_delay();

    // Let sensor pin float
    m_pSensorPin->pin().stopCharge();

    // Connect sensor pin to S&H cap
    m_pSensorPin->select();

    uint16_t charged = getAdcSample();

    sei();

    total += (charged-discharged);

    if (m_onSampleCallback) (*m_onSampleCallback)(m_onSampleCallbackData);
  }

  return update((total / sampleCount) << 8) >> 8;
}

uint32_t ProximitySensor::updateMovingAverage(uint32_t sample) {
  return m_movingAverage = (int32_t)m_movingAverage + (((int32_t)m_filterAdaptationRate*((int32_t)sample - (int32_t)m_movingAverage)) >> 8);
}

uint32_t ProximitySensor::getIdleDurationMs() const {
  return m_state == TOUCH || m_state == PROXIMITY ? 0 : millis() - m_idleStartTimeMs;
}

uint32_t ProximitySensor::getProximityDurationMs() const {
  return m_state == TOUCH || m_state == PROXIMITY ? millis() - m_proximityStartTimeMs : 0;
}

uint32_t ProximitySensor::getTouchDurationMs() const {
  return m_state == TOUCH ? millis() - m_touchStartTimeMs : 0;
}

uint32_t ProximitySensor::update(uint32_t sample) {

  if (m_reseed == true) {
    m_movingAverage = sample;
    m_reseed = false;
    return sample;
  }

  uint32_t reseedThreshold = m_movingAverage - ((m_filterReseedThreshold * m_movingAverage) >> 8);
  uint32_t proximityThreshold = m_movingAverage + ((m_proximityThreshold * m_movingAverage) >> 8);
  uint32_t touchThreshold = proximityThreshold + ((m_touchThreshold * m_movingAverage) >> 8);
  uint32_t releaseThreshold = touchThreshold - ((m_releaseThreshold * m_movingAverage) >> 8);

  if (m_state == IDLE) {
    if (sample > proximityThreshold) {
      if (m_delayStartTimeMs == 0) {
        m_delayStartTimeMs = millis();
      }
      else if (millis() - m_delayStartTimeMs > m_delayMs) {
        m_state = PROXIMITY;
        m_delayStartTimeMs = 0;
        m_proximityStartTimeMs = millis();
      }
    }
    else if (sample < reseedThreshold) {
      m_delayStartTimeMs = 0;
      m_movingAverage = sample;
    }
    else {
      m_delayStartTimeMs = 0;
      updateMovingAverage(sample);
    }
  }

  if (m_state == PROXIMITY) {
    if (sample >= touchThreshold) {
      m_state = TOUCH;
      m_touchStartTimeMs = millis();
    }
    else if (sample < proximityThreshold) {
      m_state = IDLE;
      m_idleStartTimeMs = millis();
      updateMovingAverage(sample);
    }
    else if ((m_proximityTimeoutMs > 0 && millis() - m_proximityStartTimeMs) > m_proximityTimeoutMs) {
      m_state = IDLE;
      m_idleStartTimeMs = millis();
      m_movingAverage = sample;
    }
  }

  if (m_state == TOUCH) {
    if (sample < releaseThreshold) {
      if (sample >= proximityThreshold) {
        m_state = PROXIMITY;
        m_proximityStartTimeMs = millis();
      }
      else {
        m_state = IDLE;
        m_idleStartTimeMs = millis();
        updateMovingAverage(sample);
      }
    }
    else if (m_touchTimeoutMs > 0 && millis() - m_touchStartTimeMs > m_touchTimeoutMs) {
      m_state = IDLE;
      m_idleStartTimeMs = millis();
      m_movingAverage = sample;
    }
  }

  return sample;
}

uint16_t ProximitySensor::getAdcSample() {

  // Start conversion
  ADCSRA |= (1<<ADSC);

  // Wait for conversion to finish
  while(!(ADCSRA & _BV(ADIF)));

  // Reset the conversion complete flag
  ADCSRA |= (1<<ADIF);

  //return (ADCH << 8) | ADCL;
  return ADC;
}


