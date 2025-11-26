/*
  ESP32-WROOM-32D + KY-024 Linear Hall Module (magnet) + BLE Speed Telemetry
  - Counts pulses from KY-024 DO to compute RPM and linear speed
  - Reads KY-024 AO to expose magnetic field strength (for tuning)
  - Broadcasts over BLE (notify) every 100 ms using the user's UUIDs

  Wiring (KY-024 → ESP32):
    VCC → 3.3V

V3 (KY-024) → 3.3V (ESP32)
GND (KY-024) → GND (ESP32)
DO (KY-024) → GPIO 27 (ESP32)
AO (KY-024) → GPIO 34 (ESP32)

  Adjust these for your setup:
    WHEEL_CIRCUMFERENCE_M : wheel circumference in meters
    MAGNETS_PER_REV       : number of magnets on wheel
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// ==== BLE UUIDs (kept from your original sketch) ====
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// ==== Pins ====
constexpr int PIN_KY024_DO = 27;  // Digital output (LM393 comparator)
constexpr int PIN_KY024_AO = 34;  // Analog output (linear Hall), ADC1 only

// ==== Wheel / sensor config ====
constexpr float   WHEEL_CIRCUMFERENCE_M = 0.210f;  // <-- CHANGE: your wheel circumference in meters
constexpr uint8_t MAGNETS_PER_REV       = 1;       // <-- CHANGE: magnets per wheel revolution

// Minimum pulse spacing to de-bounce (us). 1500–3000 us is fine for most RC wheels
constexpr uint32_t MIN_PULSE_GAP_US = 2000;

// Timeout (ms) after last pulse to consider speed = 0
constexpr uint32_t NO_PULSE_TIMEOUT_MS = 600;

// BLE
BLECharacteristic *pCharacteristic = nullptr;

// ===== Telemetry packet (sent via BLE notify) =====
#pragma pack(push, 1)
struct Telemetry {
  float speed_mps;     // meters per second
  float speed_kph;     // km/h
  float rpm;           // wheel RPM
  uint16_t hall_raw;   // 0..4095 (ESP32 12-bit ADC)
  uint32_t millis_ts;  // timestamp for client-side sanity
};
#pragma pack(pop)

Telemetry tx{};

// ===== Pulse counting (ISR) =====
volatile uint32_t g_pulseCount = 0;
volatile uint32_t g_lastPulseMicros = 0;
volatile uint32_t g_lastIntervalMicros = 0;

void IRAM_ATTR isr_k y024() {
  uint32_t now = micros();
  uint32_t gap = now - g_lastPulseMicros;
  if (gap >= MIN_PULSE_GAP_US) {      // simple de-bounce / noise reject
    g_lastIntervalMicros = gap;
    g_lastPulseMicros = now;
    g_pulseCount++;
  }
}

// ===== Helpers =====
float rpmFromInterval(uint32_t intervalMicros, uint8_t magnetsPerRev) {
  if (intervalMicros == 0 || magnetsPerRev == 0) return 0.0f;
  // rev/s = 1 / (interval * magnetsPerRev)  where interval in seconds
  // rpm   = rev/s * 60
  const double rev_per_sec = 1e6 / (double(intervalMicros) * double(magnetsPerRev));
  return float(rev_per_sec * 60.0);
}

float speedMpsFromRPM(float rpm, float circumference_m) {
  // rev/min * circumference (m/rev) = m/min
  // m/min / 60 = m/s
  return (rpm * circumference_m) / 60.0f;
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);
  delay(300);

  // GPIO
  pinMode(PIN_KY024_DO, INPUT_PULLUP);  // DO is open-collector style from the module's LM393; pull-up ensures a clean edge
  pinMode(PIN_KY024_AO, INPUT);         // AO analog (ADC1)

  attachInterrupt(digitalPinToInterrupt(PIN_KY024_DO), isr_k y024, CHANGE);
  // NOTE: If you want fewer false edges, you can switch to FALLING or RISING after observing actual polarity.
  // Example: attachInterrupt(digitalPinToInterrupt(PIN_KY024_DO), isr_k y024, FALLING);

  // BLE init
  BLEDevice::init("RC-Speed-Tracker");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("RC Speed Tracker is ready. Open a BLE client and subscribe to notifications.");
}

// ===== Loop =====
void loop() {
  static uint32_t lastSendMs = 0;
  const uint32_t nowMs = millis();

  // Read analog magnetic level (0..4095)
  uint16_t hallRaw = analogRead(PIN_KY024_AO);

  // Derive RPM / speed from last interval if pulses are recent
  uint32_t lastPulseMicrosSnapshot;
  uint32_t lastIntervalMicrosSnapshot;
  uint32_t pulseCountSnapshot;

  noInterrupts();
  lastPulseMicrosSnapshot   = g_lastPulseMicros;
  lastIntervalMicrosSnapshot= g_lastIntervalMicros;
  pulseCountSnapshot        = g_pulseCount; // not used directly here, but handy for debugging
  interrupts();

  float rpm = 0.0f;
  float speed_mps = 0.0f;

  const uint32_t nowUs = micros();
  const uint32_t sinceLastPulseMs = (nowUs - lastPulseMicrosSnapshot) / 1000U;

  if (sinceLastPulseMs <= NO_PULSE_TIMEOUT_MS && lastIntervalMicrosSnapshot > 0) {
    rpm = rpmFromInterval(lastIntervalMicrosSnapshot, MAGNETS_PER_REV);
    speed_mps = speedMpsFromRPM(rpm, WHEEL_CIRCUMFERENCE_M);
  } else {
    rpm = 0.0f;
    speed_mps = 0.0f;
  }

  // Send over BLE every 100 ms
  if (nowMs - lastSendMs >= 100) {
    lastSendMs = nowMs;

    tx.speed_mps = speed_mps;
    tx.speed_kph = speed_mps * 3.6f;
    tx.rpm       = rpm;
    tx.hall_raw  = hallRaw;
    tx.millis_ts = nowMs;

    pCharacteristic->setValue((uint8_t*)&tx, sizeof(tx));
    pCharacteristic->notify();

    // Serial debug
    Serial.print("RPM: ");      Serial.print(tx.rpm, 1);
    Serial.print(" | m/s: ");   Serial.print(tx.speed_mps, 2);
    Serial.print(" | km/h: ");  Serial.print(tx.speed_kph, 2);
    Serial.print(" | hallRaw: "); Serial.print(tx.hall_raw);
    Serial.print(" | pulses: ");  Serial.println(pulseCountSnapshot);
  }

  // Small sleep to lower CPU usage
  delay(2);
}
