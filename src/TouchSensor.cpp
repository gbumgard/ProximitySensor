/*
 * TSIChannel.cpp
 *
 *  Created on: Sep 29, 2014
 *      Author: gbumgard
 */

#include "TouchSensor.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

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

TouchSensor::TouchSensor(AdcPinInput* pReferencePin, AdcPinInput* pSensorPin )
: m_pReferencePin(pReferencePin)
, m_pSensorPin(pSensorPin)
, m_gain(0)
, m_scale(0)
, m_ltaAdaptationRate(DEFAULT_LTA_ADAPTATION_RATE)
, m_ltaReseedThreshold(DEFAULT_LTA_RESEED_THRESHOLD)
, m_idleStartTimeMs(millis())
, m_proximityThreshold(DEFAULT_PROXIMITY_THRESHOLD)
, m_proximityTimeoutMs(DEFAULT_PROXIMITY_TIMEOUT_MS)
, m_proximityStartTimeMs(0)
, m_touchThreshold(DEFAULT_TOUCH_THRESHOLD)
, m_touchTimeoutMs(DEFAULT_TOUCH_TIMEOUT_MS)
, m_touchStartTimeMs(0)
, m_releaseThreshold(DEFAULT_RELEASE_THRESHOLD)
, m_debounceDelayMs(DEFAULT_DEBOUNCE_DELAY_MS)
, m_debounceStartTimeMs(0)
, m_lta(0)
, m_state(IDLE)
, m_reseed(true)
, m_reseedSampleCount(0)
, m_onSampleCallbackData(0)
, m_onSampleCallback(0)
{
}

uint32_t TouchSensor::update() {

  uint8_t gain = m_gain;
  uint8_t scale = m_scale;

  size_t sampleCount = _BV(gain);

  uint32_t value = 0;

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

    value += (charged-discharged);

    if (m_onSampleCallback) (*m_onSampleCallback)(m_onSampleCallbackData);
  }

  return update((value >> scale) << (14 - gain + scale));
}


uint32_t TouchSensor::update(uint32_t sample) {

  if (m_reseed == true) {
    m_lta = sample;
    m_reseed = false;
    return sample;
  }

  uint32_t reseedThreshold = m_lta - getLtaFraction(m_ltaReseedThreshold);
  uint32_t proximityThreshold = m_lta + getLtaFraction(m_proximityThreshold);
  uint32_t touchThreshold = proximityThreshold + getLtaFraction(m_touchThreshold);
  uint32_t releaseThreshold = touchThreshold - getLtaFraction(m_releaseThreshold);

  if (m_state == IDLE) {
    if (sample > proximityThreshold) {
      if (m_debounceStartTimeMs == 0) {
        m_debounceStartTimeMs = millis();
      }
      else if (millis() - m_debounceStartTimeMs > m_debounceDelayMs) {
        m_state = PROXIMITY;
        m_debounceStartTimeMs = 0;
        m_proximityStartTimeMs = millis();
      }
    }
    else if (sample < reseedThreshold) {
      m_debounceStartTimeMs = 0;
      m_lta = sample;
    }
    else {
      m_debounceStartTimeMs = 0;
      updateLta(sample,m_ltaAdaptationRate);
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
      updateLta(sample,m_ltaAdaptationRate);
    }
    else if (m_proximityTimeoutMs > 0 && millis() - m_proximityStartTimeMs > m_proximityTimeoutMs) {
      m_state = IDLE;
      m_idleStartTimeMs = millis();
      m_lta = sample;
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
        updateLta(sample,m_ltaAdaptationRate);
      }
    }
    else if (m_touchTimeoutMs > 0 && millis() - m_touchStartTimeMs > m_touchTimeoutMs) {
      m_state = IDLE;
      m_idleStartTimeMs = millis();
      m_lta = sample;
    }
  }
  return sample;
}

