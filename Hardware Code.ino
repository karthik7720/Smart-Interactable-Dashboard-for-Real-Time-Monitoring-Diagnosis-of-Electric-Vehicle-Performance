#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <time.h>

// === GPS ===
#define RXPin 16
#define TXPin 17
#define GPSBaud 9600
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);  // UART1

// === Pins ===
#define VOLTAGE_PIN 34
#define CURRENT_PIN 35
#define ENCODER_PIN_A 18
#define ENCODER_PIN_B 19
#define RELAY_PIN 

#define BUTTON_PIN 15
#define ONE_WIRE_BUS 4

// === Voltage Divider Resistors ===
float R1 = 30000.0;
float R2 = 7500.0;

// === Encoder Parameters ===
const int pulsesPerRevolution = 20;
const float wheelCircumference = 0.314;

// === Globals ===
volatile int pulseCount = 0;
unsigned long lastCheck = 0;
bool relayState = false;
bool lastButtonState = HIGH;

// === Dallas Temp Sensor ===
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

// === Encoder ISR ===
void IRAM_ATTR encoderISR() {
  pulseCount++;
}

// === Display GPS Info ===
void displayGPSInfo() {
  Serial.print(F("📍 Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" | 📆 Date/Time: "));
  if (gps.date.isValid()) {
    Serial.printf("%02d/%02d/%04d ", gps.date.month(), gps.date.day(), gps.date.year());
  } else {
    Serial.print("INVALID ");
  }

  if (gps.time.isValid()) {
    Serial.printf("%02d:%02d:%02d\n", gps.time.hour(), gps.time.minute(), gps.time.second());
  } else {
    Serial.println("INVALID");
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);

  // Encoder
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderISR, RISING);

  // Relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Temperature sensor
  sensors.begin();
  if (!sensors.getAddress(tempDeviceAddress, 0)) {
    Serial.println("❌ Dallas sensor not found.");
  }

  // Set system time (RTC)
  struct tm tm;
  tm.tm_year = 2025 - 1900;
  tm.tm_mon = 3;
  tm.tm_mday = 15;
  tm.tm_hour = 10;
  tm.tm_min = 30;
  tm.tm_sec = 0;
  time_t t = mktime(&tm);
  struct timeval now = { .tv_sec = t };
  settimeofday(&now, NULL);

  Serial.println("✅ System Initialized: Sensors + GPS Ready");
}

void loop() {
  // === GPS Data Reading ===
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // === Button toggle control ===
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    relayState = !relayState;
    digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
    Serial.println(relayState ? "🔁 Relay ON (button)" : "🛑 Relay OFF (button)");
    delay(200);
  }
  lastButtonState = currentButtonState;

  // === Serial control ===
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toUpperCase();
    if (input == "ON") {
      relayState = false;
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("🛑 Relay turned OFF (serial)");
    } else if (input == "OFF") {
      relayState = true;
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("🔁 Relay turned ON (serial)");
    }
  }

  // === Periodic Display ===
  unsigned long currentTime = millis();
  if (currentTime - lastCheck >= 1000) {
    noInterrupts();
    int count = pulseCount;
    pulseCount = 0;
    interrupts();

    float rpm = (count / (float)pulsesPerRevolution) * 60.0;
    float speed = ((rpm * wheelCircumference) / 60.0) * 3.6;

    // === Voltage Measurement ===
    int vRaw = analogRead(VOLTAGE_PIN);
    float vOUT = (vRaw * 3.3) / 4095.0;
    float voltage = vOUT / (R2 / (R1 + R2));

    // === Current Measurement ===
    float current = 0;
    for (int i = 0; i < 100; i++) {
      current += (0.0264 * analogRead(CURRENT_PIN) - 13.51);
      delay(1);
    }
    current /= 100.0;
    current /= 1000.0;

    float power = voltage * current;

    // === Temperature ===
    sensors.requestTemperatures();
    float temp1 = sensors.getTempC(tempDeviceAddress);

    // === RTC Time ===
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // === Output ===
    Serial.println("==============================");
    Serial.printf("🕒 Time: %04d-%02d-%02d %02d:%02d:%02d\n",
                  timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    Serial.print("🔁 RPM: "); Serial.print(rpm);
    Serial.print(" | Speed: "); Serial.print(speed); Serial.println(" km/h");

    Serial.print("🔋 Voltage: "); Serial.print(voltage); Serial.println(" V");
    Serial.print("⚡ Current: "); Serial.print(current, 3); Serial.println(" A");
    Serial.print("🔌 Power: "); Serial.print(power, 3); Serial.println(" W");

    Serial.print("🌡 Temp: "); Serial.print(temp1); Serial.println(" °C");

    // === GPS Info ===
    displayGPSInfo();

    lastCheck = currentTime;
  }

  // === GPS Connection Timeout Check ===
  if (millis() > 10000 && gps.charsProcessed() < 10) {
    Serial.println("❗ No GPS detected: check wiring.");
  }
}
