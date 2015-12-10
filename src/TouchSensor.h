/*
 * TSIChannel.h
 *
 *  Created on: Sep 29, 2014
 *      Author: gbumgard
 */

#ifndef TOUCHSENSOR_H_
#define TOUCHSENSOR_H_

#include <stdint.h>
#include <util/delay.h>
#include "AdcPinInput.h"
#include "Fraction.h"
#include "timer.h"

#define DEFAULT_LTA_ADAPTATION_RATE 4

#define DEFAULT_LTA_RESEED_THRESHOLD 32

#define DEFAULT_DEBOUNCE_DELAY_MS 250

#define DEFAULT_PROXIMITY_THRESHOLD 8

#define DEFAULT_PROXIMITY_TIMEOUT_MS 20000

#define DEFAULT_TOUCH_THRESHOLD 40

#define DEFAULT_TOUCH_TIMEOUT_MS 20000

#define DEFAULT_RELEASE_THRESHOLD 8

class TouchSensor {

public:

  enum State { IDLE, PROXIMITY, TOUCH };

  typedef void (*OnSampleCallback)(void*);

protected:

  AdcPinInput* m_pReferencePin;
  AdcPinInput* m_pSensorPin;

  uint8_t m_gain;
  uint8_t m_scale;

  uint8_t m_ltaAdaptationRate;

  uint8_t m_ltaReseedThreshold;

  uint32_t m_idleStartTimeMs;

  uint8_t m_proximityThreshold;

  uint32_t m_proximityTimeoutMs;
  uint32_t m_proximityStartTimeMs;

  uint8_t m_touchThreshold;

  uint32_t m_touchTimeoutMs;
  uint32_t m_touchStartTimeMs;

  uint8_t m_releaseThreshold;

  uint32_t m_debounceDelayMs;
  uint32_t m_debounceStartTimeMs;

  uint32_t m_lta;

  State m_state;

  bool m_reseed;

  size_t m_reseedSampleCount;

  void* m_onSampleCallbackData;
  OnSampleCallback m_onSampleCallback;

public:

  void setOnSampleCallback(OnSampleCallback cb, void* data) {
    m_onSampleCallback = cb;
    m_onSampleCallbackData = data;
  }

  uint8_t setGain(const uint8_t& gain) {
    m_reseed = true;
    return m_gain = gain > 14 ? 14 : gain;
  }

  uint8_t getGain() const {
    return m_gain;
  }

  uint8_t setScale(const uint8_t& scale) {
    m_reseed = true;
    return m_scale = scale > 14 ? 14 : scale;
  }

  uint8_t getScale() const {
    return m_scale;
  }

  uint8_t setLtaAdaptationRate(const uint8_t& adaptationRate) {
    return m_ltaAdaptationRate = adaptationRate;
  }

  uint8_t getLtaAdaptationRate() const {
    return m_ltaAdaptationRate;
  }

  uint8_t setLtaReseedThreshold(const uint8_t& threshold) {
    return m_ltaReseedThreshold = threshold;
  }

  uint8_t getLtaReseedThreshold() const {
    return m_ltaReseedThreshold;
  }

  uint8_t setProximityThreshold(const uint8_t& threshold) {
    return m_proximityThreshold = threshold;
  }

  uint8_t getProximityThreshold() const {
    return m_proximityThreshold;
  }

  uint8_t setTouchThreshold(const uint8_t& threshold) {
    return m_touchThreshold = threshold;
  }

  uint8_t getTouchThreshold() const {
    return m_touchThreshold;
  }

  uint8_t setReleaseThreshold(const uint8_t& threshold) {
    return m_releaseThreshold = threshold;
  }

  uint8_t getReleaseThreshold() const {
    return m_releaseThreshold;
  }

  uint32_t setDebounceDelayMs(const uint32_t milliseconds) {
    return m_debounceDelayMs = milliseconds;
  }

  uint32_t getDebounceDelayMs() const {
    return m_debounceDelayMs;
  }

  uint32_t getDebounceStartTimeMs() const {
    return m_debounceStartTimeMs;
  }

  uint32_t setProximityTimeoutMs(const uint32_t& milliseconds) {
    return m_proximityTimeoutMs = milliseconds;
  }

  uint32_t getProximityTimeoutMs() const {
    return m_proximityTimeoutMs;
  }

  uint32_t setTouchTimeoutMs(const uint32_t& milliseconds) {
    return m_touchTimeoutMs = milliseconds;
  }

  uint32_t getTouchTimeoutMs() const {
    return m_touchTimeoutMs;
  }

  uint32_t getLta() const {
    return m_lta;
  }

  State getState() const {
    return m_state;
  }

  bool inProximity() const {
    return m_state == PROXIMITY || m_state == TOUCH;
  }

  bool inTouch() const {
    return m_state == TOUCH;
  }

  uint32_t getIdleDurationMs() const {
    return m_state == TOUCH || m_state == PROXIMITY ? 0 : millis() - m_idleStartTimeMs;
  }

  uint32_t getProximityDurationMs() const {
    return m_state == TOUCH || m_state == PROXIMITY ? millis() - m_proximityStartTimeMs : 0;
  }

  uint32_t getTouchDurationMs() const {
    return m_state == TOUCH ? millis() - m_touchStartTimeMs : 0;
  }

  void reseed() {
    m_reseed = true;
  }

  uint32_t update();

  TouchSensor(AdcPinInput* pReferencePin, AdcPinInput* pSensorPin);

protected:

  uint32_t update(uint32_t sample);

  uint32_t getLtaFraction(const uint8_t& enumerator) {
    return ((enumerator * m_lta) >> 8);
  }

  uint32_t updateLta(uint32_t sample, const uint8_t& adaptationRate) {
    return m_lta = (int32_t)m_lta + (((int32_t)adaptationRate*((int32_t)sample - (int32_t)m_lta)) >> 8);
  }

  static uint16_t getAdcSample() {

    // Start conversion
    ADCSRA |= (1<<ADSC);

    // Wait for conversion to finish
    while(!(ADCSRA & _BV(ADIF)));

    // Reset the conversion complete flag
    ADCSRA |= (1<<ADIF);

    //return (ADCH << 8) | ADCL;
    return ADC;
  }
};


#endif /* TOUCHSENSOR_H_ */
