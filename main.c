#include "Adafruit_LEDBackpack.h"

Adafruit_7segment led_display1 = Adafruit_7segment();

const uint8_t TMP_PIN = A0;
const uint8_t POT_PIN = A1;

// Relay to enable/disable motor driver supply
const uint8_t RELAY_EN_PIN = 7;

// Motor driver (e.g. L298N) control pins
const uint8_t MOTOR_IN1 = 5;
const uint8_t MOTOR_IN2 = 4;
const uint8_t MOTOR_ENA = 2;  // PWM-capable

// ADC & TMP36 constants for Arduino Uno
const float ADC_REF_VOLTAGE = 5.0f;
const float ADC_MAX_COUNT   = 1023.0f;
const float TMP36_OFFSET    = 0.5f;
const float TMP36_SCALE     = 100.0f;
const int MAX_PWM_VAL       = 255;  // Max PWM value for analogWrite()

float previousTemp = 0;
bool fanOn = false;

const float TEMP_THRESHOLD = 23.0;   // change to any value you like

float read_temperatureC() {
  int adcValue = analogRead(TMP_PIN);
  float voltage = adcValue * (ADC_REF_VOLTAGE / ADC_MAX_COUNT);
  float tempC   = (voltage - TMP36_OFFSET) * TMP36_SCALE;
  return tempC;
}

void setup() {
  pinMode(TMP_PIN, INPUT);  // analog input
  pinMode(POT_PIN, INPUT);        // Potentiometer pin is an input (we read it)
  
  // Initialize the 4-digit display at address 112 (0x70)
  led_display1.begin(112);
  
  pinMode(RELAY_EN_PIN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);
  Serial.begin(9600);

  digitalWrite(RELAY_EN_PIN, LOW);  // Motor driver initially OFF
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_ENA, 0);
}

void loop() {
  //digitalWrite(RELAY_EN_PIN, LOW);
  float temperatureC = read_temperatureC();

  // Convert to integer for display (e.g. 25.3 -> 25)
  int tempInt = (int)round(temperatureC);

  Serial.print("Temp (C): ");
  Serial.println(tempInt);
  
  // 3) Update 4-digit display (with clamp if needed)
  if (tempInt < -999) tempInt = -999;
  if (tempInt > 9999) tempInt = 9999;

  // Show temperature on the 4-digit display
  led_display1.println(tempInt);   // send the number
  led_display1.writeDisplay();     // update display

  delay(500);
  
  
  // 1) Detect crossing of threshold
  if (!fanOn && temperatureC >= TEMP_THRESHOLD) {
      Serial.print("If statement block 1");
    // Rising through threshold → trigger safety/override
    fanOn = true;
    // e.g. turn relay ON, increase motor speed, light warning LED, etc.
    // 1) Turn ON relay → power to motor driver
    digitalWrite(RELAY_EN_PIN, HIGH);
    delay(100);  // small settling time

    // 2) Run motor forward at ~50% speed for 3s
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    analogWrite(MOTOR_ENA, 128);   // 0–255, ~50%
    delay(3000);
    }

    if (fanOn && temperatureC <= (TEMP_THRESHOLD - 2.0)) {
      Serial.print("If statement block 2");
      // Falling back below threshold with 2°C hysteresis
      fanOn = false;
      // e.g. turn relay OFF, slow fan, clear warning
      // 3) Stop motor
      analogWrite(MOTOR_ENA, 0);
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, LOW);
      delay(1000);

      // 4) Turn relay OFF → cut power to motor driver
      digitalWrite(RELAY_EN_PIN, LOW);
     // delay(3000);
    }

    // ================= PWM SPEED CONTROL (POTENTIOMETER) =================
    if (fanOn) {
      int potValue = analogRead(POT_PIN);                 // 0–1023
      int pwmValue = map(potValue, 0, 1023, 0, MAX_PWM_VAL);

      analogWrite(MOTOR_ENA, pwmValue);                   // Control fan speed

      Serial.print("Fan PWM: ");
      Serial.println(pwmValue);
    }

  previousTemp = temperatureC;
}
