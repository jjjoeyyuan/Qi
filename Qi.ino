#include <Adafruit_CircuitPlayground.h>
#include <math.h>

// Sensitivity and mapping parameters
float sensitivity = 0.5;     // Lower value = more sensitive to movement
float maxAcc = 12.0;         // Maximum acceleration for brightness mapping

// Smoothing variables
float smoothAcc = 0;         // Smoothed acceleration value
float smoothing = 0.2;       // Smoothing factor (smaller = smoother but slower)

// Brightness control
float currentBrightness = 0; // Current LED brightness
float targetBrightness = 0;  // Target brightness based on motion

float decaySpeed = 0.95;     // Decay rate when motion stops (closer to 1 = slower fade)

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
}

void loop() {
  // Read acceleration from 3 axes
  float x = CircuitPlayground.motionX();
  float y = CircuitPlayground.motionY();
  float z = CircuitPlayground.motionZ();

  // Calculate total acceleration minus gravity
  float totalAcc = sqrt(x*x + y*y + z*z) - 9.8;
  if (totalAcc < 0) totalAcc = 0;

  // Smooth the acceleration (low-pass filter)
  smoothAcc = smoothAcc + (totalAcc - smoothAcc) * smoothing;

  Serial.print("Smooth Acc: ");
  Serial.println(smoothAcc);

  // Calculate target brightness based on motion intensity
  if (smoothAcc > sensitivity) {
    targetBrightness = 255 * (smoothAcc - sensitivity) / (maxAcc - sensitivity);
    if (targetBrightness > 255) targetBrightness = 255;
  } else {
    // Gradual decay when no motion (creates a lingering effect)
    targetBrightness *= decaySpeed;
  }

  // Smooth brightness transition (ease effect)
  currentBrightness += (targetBrightness - currentBrightness) * 0.2;

  // Clamp brightness range
  if (currentBrightness < 0) currentBrightness = 0;
  if (currentBrightness > 255) currentBrightness = 255;

  // Output to all 10 LEDs (white light)
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, currentBrightness, currentBrightness, currentBrightness);
  }

  delay(20); // Faster refresh for smoother visual response
}