#include <ProximitySensor.h>
#include <TAdcPinInput.h>

// Construct one sensor instance.
// This sensor instance uses PB4/ADC11/A8 as a reference pin and PB5/ADC12/A9 as the input pin.
ProximitySensor sensor(&TAdcPinInput<11>::instance(),&TAdcPinInput<12>::instance());

void setup() {

  Serial.begin(9600);

  // Called once in during setup. Configures ADC.
  ProximitySensor::begin();

  // Set filter coefficient to 2/256 to provide very slow moving average response.
  sensor.setFilterAdaptationRate(2);

  // Set resolution to 8-bits (out of 10).
  // Smaller values will increase sample rate but also increase noise.
  sensor.setResolution(8);

  // Set proximity "debounce" delay to 100 milliseconds.
  sensor.setDelayMs(100);

  // Force exit from PROXIMITY state and reset moving average after 5 seconds.
  sensor.setProximityTimeoutMs(5000);

  // Force exit from TOUCH state and reset moving average after 5 seconds.
  sensor.setTouchTimeoutMs(5000);

  // Set proximity threshold to (moving_average + 5/256 * moving_average).
  sensor.setProximityThreshold(5);

  // Set touch threshold to (proximity_threshold + 15/256 * proximity_threshold).
  sensor.setTouchThreshold(15);

}

void loop() {

  // Sample the ADC and update state. Returns new sample value.
  uint32_t sample = sensor.update();

  // Retrieve the current sensor state
  ProximitySensor::State state = sensor.getState();

  // Retrieve the moving average to compare with samples
  uint32_t average = sensor.getMovingAverage();

  // Remove the average from the samples to see relative values.
  int32_t difference = (int32_t)sample-(int32_t)average;
  difference = difference < 0 ? 0 : difference;

  Serial.print(sample);
  Serial.print(" ");
  Serial.print(average);
  Serial.print(" ");
  Serial.print(difference);
  Serial.print(" ");
  Serial.print(state == ProximitySensor::IDLE ? "I" : (state == ProximitySensor::PROXIMITY) ? "P" : "T");
  Serial.print(" ");
  Serial.println(state == ProximitySensor::IDLE
                 ? 
                   sensor.getIdleDurationMs()
                   :
                   state == ProximitySensor::PROXIMITY
                     ?
                     sensor.getProximityDurationMs()
                     :
                     sensor.getTouchDurationMs());

}

