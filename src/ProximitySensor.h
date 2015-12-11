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
 * A class representing a single capacitive proximity sensor.
 * Each sensor requires two dedicated ADC inputs for operation.
 * One pin is attached to an electrode or antenna that will act as
 * one plate of a virtual capacitor. The second pin is used as a
 * reference and must be left unconnected.
 */
class ProximitySensor {

public:

  enum State { IDLE, PROXIMITY, TOUCH };

  /**
   * Constructs a ProximitySensor instance. The constructor accepts
   * two parameters that describe the pins that will be used to
   * provide the sensing functionality (see class description).
   * Pin-specific specializations of the TAdcPinInput template
   * class are used for these parameters, e.g.:
   *
   *   ProximitySensor(
   *       &TAdcPinInput<11>::instance(),
   *       &TAdcPinInput<12>::instance()
   *   )
   */
  ProximitySensor(AdcPinInput* pReferencePin, AdcPinInput* pSensorPin);

  /**
   * Configures and enables ADC.
   */
  static void begin();

  /**
   * Updates the current sensor state. This method is called to
   * capture samples, update the moving average and trigger
   * state transitions. Typically called from the main application loop.
   */
  uint32_t update();

  /**
   * The on-sample callback function signature.
   */
  typedef void (*OnSampleCallback)(void*);

  /**
   * Registers an optional callback function that will be invoked
   * after each ADC sample is read. The callback may be used to
   * perform other operations while samples are averaged within
   * the main update() call.
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
  uint8_t setFilterAdaptationRate(const uint8_t adaptationRate) {
    return m_filterAdaptationRate = adaptationRate;
  }

  /**
   * Gets the moving average adaptation rate setting.
   */
  uint8_t getFilterAdaptationRate() const {
    return m_filterAdaptationRate;
  }

  /**
   * Sets the moving average filter reseed threshold. This
   * value defines a threshold relative to the current moving average
   * value at which a sample will cause the moving average to be
   * immediately changed to match the value of that sample.
   * The threshold parameter is treated as the enumerator in
   * a fractional value with a fixed denominator of 256 - a value of
   * 1 specifies a threshold of 1/256 (.0039 or 0.39%).
   */
  uint8_t setFilterReseedThreshold(const uint8_t threshold) {
    return m_filterReseedThreshold = threshold;
  }

  /**
   * Gets the moving average filter reseed threshold setting.
   */
  uint8_t getFilterReseedThreshold() const {
    return m_filterReseedThreshold;
  }

  /**
   * Sets the threshold at which the sensor will enter the PROXIMITY
   * state. The threshold is defined as a percentage of the current
   * moving average. The parameter is treated as the enumerator in
   * a fractional value with a fixed denominator of 256 - a value of
   * 1 specifies a threshold of 1/256 (.0039 or 0.39%). The first sample
   * with a value greater than (moving_average + moving_average*threshold)
   * will trigger entry into the Proximity state.
   */
  uint8_t setProximityThreshold(const uint8_t& threshold) {
    return m_proximityThreshold = threshold;
  }

  /**
   * Gets the current proximity threshold setting.
   */
  uint8_t getProximityThreshold() const {
    return m_proximityThreshold;
  }

  /**
   * Sets the threshold at which the sensor will enter the TOUCH
   * state. The threshold is defined as a percentage of the value at
   * which the sensor entered the Proximity state.
   * The parameter is treated as the enumerator in a fractional value
   * with a fixed denominator of 256 - a value of 1 specifies a
   * threshold of 1/256 (.0039 or 0.39%). The first sample with a value
   * greater than  (proximity_value + proximity_value*threshold)
   * will trigger entry into the Touch state.
   */
  uint8_t setTouchThreshold(const uint8_t& threshold) {
    return m_touchThreshold = threshold;
  }

  /**
   * Gets the current touch threshold setting.
   */
  uint8_t getTouchThreshold() const {
    return m_touchThreshold;
  }

  /**
   * Sets the threshold at which the sensor will exit the TOUCH
   * state. The threshold is defined as a percentage relative to
   * the value at which the sensor entered the touch state.
   * The parameter is treated as the enumerator in a fractional value
   * with a fixed denominator of 256 - a value of 1 specifies a
   * threshold of 1/256 (.0039 or 0.39%). The first sample with a
   * value less than (touch_value - threshold_value * touch) will trigger an
   * exit from the Touch state.
   */
  uint8_t setReleaseThreshold(const uint8_t& threshold) {
    return m_releaseThreshold = threshold;
  }

  /**
   * Gets the current release threshold setting.
   */
  uint8_t getReleaseThreshold() const {
    return m_releaseThreshold;
  }

  /**
   * Sets a time that must pass with after the sensor begins
   * receiving samples greater than the proximity threshold before
   * entering the PROXIMITY state. This delay is used to "debounce"
   * the incoming samples to prevent several state changes as samples
   * near the proximity threshold.
   */
  uint32_t setDelayMs(const uint32_t milliseconds) {
    return m_delayMs = milliseconds;
  }

  /**
   * Gets the current delay time setting.
   */
  uint32_t getDelayMs() const {
    return m_delayMs;
  }

  /**
   * Returns the time since the sensor entered into the
   * temporary delay state.
   */
  uint32_t getDelayStartTimeMs() const {
    return m_delayStartTimeMs;
  }

  /**
   * Sets the maximum amount of time that the sensor will be
   * allowed reside in the PROXIMITY state. A timeout should be used
   * if significant, persistent changes in the capacitive field
   * environment may occur while the sensor is active.
   */
  uint32_t setProximityTimeoutMs(const uint32_t& milliseconds) {
    return m_proximityTimeoutMs = milliseconds;
  }

  /**
   * Gets the current proximity timeout setting.
   */
  uint32_t getProximityTimeoutMs() const {
    return m_proximityTimeoutMs;
  }

  /**
   * Sets the maximum amount of time that the sensor will be
   * allowed reside in the TOUCH state. A timeout should be used
   * if significant, persistent changes in the capacitive field
   * environment may occur while the sensor is active.
   */
 uint32_t setTouchTimeoutMs(const uint32_t& milliseconds) {
    return m_touchTimeoutMs = milliseconds;
  }

 /**
  * Gets the current touch timeout setting.
  */
  uint32_t getTouchTimeoutMs() const {
    return m_touchTimeoutMs;
  }

  /**
   * Gets the moving average of incoming samples produced by
   * the internal IIR filter.
   */
  uint32_t getMovingAverage() const {
    return m_movingAverage;
  }

  /**
   * Gets the current state of the sensor.
   */
  State getState() const {
    return m_state;
  }

  /**
   * Indicates whether the sensor is in the PROXIMITY or TOUCH state.
   */
  bool inProximity() const {
    return m_state == PROXIMITY || m_state == TOUCH;
  }

  /**
   * Indicates whether the sensor is in the TOUCH state.
   */
  bool inTouch() const {
    return m_state == TOUCH;
  }

  /**
   * Returns the number of milliseconds during which the sensor
   * has currently resided in the IDLE state.
   */
  uint32_t getIdleDurationMs() const;

  /**
   * Returns the number of milliseconds during which the sensor
   * has currently resided in the PROXIMITY state.
   */
  uint32_t getProximityDurationMs() const;

  /**
   * Returns the number of milliseconds during which the sensor
   * has currently resided in the TOUCH state.
   */
  uint32_t getTouchDurationMs() const;

  /**
   * Forces a reset of the internal IIR moving average filter.
   */
  void reseed() {
    m_reseed = true;
  }

protected:

  uint32_t update(uint32_t sample);

  uint32_t updateMovingAverage(uint32_t sample);

  static uint16_t getAdcSample();

private:

  AdcPinInput* m_pReferencePin;
  AdcPinInput* m_pSensorPin;

  uint8_t m_resolution;

  uint8_t m_filterAdaptationRate;

  uint8_t m_filterReseedThreshold;

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

  uint32_t m_movingAverage;

  State m_state;

  bool m_reseed;

  size_t m_reseedSampleCount;

  void* m_onSampleCallbackData;
  OnSampleCallback m_onSampleCallback;

};


#endif /* PROXIMITYSENSOR_H_ */
