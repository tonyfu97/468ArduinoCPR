/*
 * BIOEN 468/568 Lab 5: Accelerometer-Based CPR Monitor
 * 
 * Use OLED to display the CPR rate and depth measured by ADXL335.
 */
#define DECAY_RATE 0.01

void setup() {
  Serial.begin(9600);
}

void loop() {
  int analogValue = analogRead(A0);

  // Static variables are initialized once.
  static float analogValueMax = 0;
  static float analogValueMin = 1024;

  // Keeping track of max and min readings.
  analogValueMax = (analogValue > analogValueMax) ? analogValue : analogValueMax;
  analogValueMin = (analogValue < analogValueMin) ? analogValue : analogValueMin;

  // Elastic envelope.
  float stepSize = (analogValueMax - analogValueMin) * DECAY_RATE;
  analogValueMax -= stepSize;
  analogValueMin += stepSize;

  // Define two thresholds (for hysteresis)
  float upperThreshold = analogValueMax * 0.7 + analogValueMin * 0.3;
  float lowerThreshold = analogValueMax * 0.3 + analogValueMin * 0.7;

  // Calculate CPR rate
  static long lastRisingEdgeTime = micros();
  static bool wasAboveUpperThreshold = false;
  static float cprRate = 0;
  if (analogValue > upperThreshold && (analogValueMax - analogValueMin) > 50) {
    if (!wasAboveUpperThreshold) {
      long currTime = micros();
      long timeDiff = currTime - lastRisingEdgeTime;  // in us
      lastRisingEdgeTime = currTime;
      cprRate = float(1e6)/timeDiff;  // in Hz
    }
    wasAboveUpperThreshold = true;
  } else if (analogValue < lowerThreshold) {
    wasAboveUpperThreshold = false;
  }

  // Calculate CPR depth
  float cprDepth = 0.7 * (analogValueMax - analogValueMin) / (cprRate * cprRate);  // in cm
}
