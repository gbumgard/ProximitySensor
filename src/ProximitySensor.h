/*
 * ProximitySensor.h
 *
 *  Created on: Sep 29, 2014
 *      Author: gbumgard
 */

#ifndef PROXIMITYSENSOR_H_
#define PROXIMITYSENSOR_H_

#include <stdint.h>
#include <TAdcPinInput.h>

/**
 * 
 */
class ProxmitySensor {

public:

  enum State { IDLE, PROXIMITY, TOUCH };

  ProxmitySensor(AdcPinInput* pReferencePin, AdcPinInput* pSensorPin);

  /**
   * Configures and enables ADC.
   */
  static void begin();

  /**
   * The on-sample callback function signature.
   */
  typedef void (*OnSampleCallback)(void*);

  /**
   * Registers an optional callback function that will be invoked
   * after each ADC sample is read. The callback may be used to
   * perform other operations while samples are averaged within
   * the update() call.
   */
  void setOnSampleCallback(OnSampleCallback cb, void* data) {
    m_onSampleCallback = cb;
    m_onSampleCallbackData = data;
  }

  /**
   * Sets the approximate number of bits of resolution desired for
   * samples returned by the update() call. Resolution is increased by
   * capturing a number of intermediate ADC samples and then averaging those
   * to produce a final sample result. Increasing the resolution will reduce
   * noise but increase sample times. The resolution parameter specifies
   * the number of bits of resolution required. Passing value of zero will result
   * in no averaging of ADC samples. Each increment above zero increases the
   * number of averaged samples by a power of 2, up to a limit of 10-bits or
   * 1024 samples, matching that ideal resolution that might be produced by
   * the 10-bit ADC.
   */
  uint8_t setResolution(const uint8_t resolution) {
    m_reseed = true;
    return m_resolution = resolution > 10 ? 10 : resolution;
  }

  /**
   * Gets the current resolution setting.
   * @see setResolution(const uint8_t resolution)
   */
  uint8_t getResolution() const {
    return m_resolution;
  }

  /**
   * Sets the moving average adaptation rate. This value is used
   * as a coefficient in the infinite impulse response (IIR) filter
   * that provides a moving average over incoming sample values.
   * The adaptation rate parameter can be viewed as the enumerator
   * of a fractional coefficient. The denominator is internally fixed
   * at a value of 256. Smaller values will result in slower
   * filter response (minimum is 1). The filter coefficient must
   * be adjusted to minimize response to transient events but
   * must not be set such that the filter cannot respond to
   * slowly changing environmental conditions.
   */
  uint8_t setMovingAverageAdaptationRate(const uint8_t adaptationRate) {
    return m_movingAverageAdaptationRate = adaptationRate;
  }

  /**
   * Gets the moving average adaptation rate setting.
   */
  uint8_t getMovingAverageAdaptationRate() const {
    return m_movingAverageAdaptationRate;
  }

  /**
   * Sets the moving average reseed threshold. This
   * value defines a threshold below the current moving average
   * at which a sample will cause the moving average to be
   * immediately changed to match the value of that sample.
   * The threshold parameter is treated as the enumerator in
   * a fractional value with a fixed denominator of 256 - a value of
   * 1 specifies a threshold of 1/256 (.0039 or 0.39%).
   */
  uint8_t setMovingAverageReseedThreshold(const uint8_t threshold) {
    return m_movingAverageReseedThreshold = threshold;
  }

  /**
   * Gets the moving average reseed threshold setting.
   */
  uint8_t getMovingAverageReseedThreshold() const {
    return m_movingAverageReseedThreshold;
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
    return m_delayMs = milliseconds;
  }

  uint32_t getDelayMs() const {
    return m_delayMs;
  }

  uint32_t getDelayStartTimeMs() const {
    return m_delayStartTimeMs;
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

  uint32_t getIdleDurationMs() const;

  uint32_t getProximityDurationMs() const;

  uint32_t getTouchDurationMs() const;

  void reseed() {
    m_reseed = true;
  }

  uint32_t update();

protected:

  uint32_t update(uint32_t sample);

  uint32_t updateLta(uint32_t sample);

  static uint16_t getAdcSample();

private:

  AdcPinInput* m_pReferencePin;
  AdcPinInput* m_pSensorPin;

  uint8_t m_resolution;

  uint8_t m_movingAverageAdaptationRate;

  uint8_t m_movingAverageReseedThreshold;

  uint32_t m_idleStartTimeMs;

  uint8_t m_proximityThreshold;

  uint32_t m_proximityTimeoutMs;
  uint32_t m_proximityStartTimeMs;

  uint8_t m_touchThreshold;

  uint32_t m_touchTimeoutMs;
  uint32_t m_touchStartTimeMs;

  uint8_t m_releaseThreshold;

  uint32_t m_delayMs;
  uint32_t m_delayStartTimeMs;

  uint32_t m_lta;

  State m_state;

  bool m_reseed;

  size_t m_reseedSampleCount;

  void* m_onSampleCallbackData;
  OnSampleCallback m_onSampleCallback;

};


#endif /* PROXIMITYSENSOR_H_ */
