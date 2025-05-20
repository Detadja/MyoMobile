#include <esp_now.h>
#include <WiFi.h>
#define LEFT 35 // Pin as seen on board: 
#define RIGHT 34 // Pin as seen on board: 
#define NUM_SAMPLES 10
#define THRESHOLD 500 // ADC threshold
#define SEND_INTERVAL 50 // ms (~10hz)
#define LED 13

// FUNCTIONS AND INITIALIZATIONS

const int muscle[4]  = {LEFT, RIGHT};
float     avg[2]     = {0, 0};
float     val[2]     = {0, 0};

// const float emg_min[2] = {0, 20}; // Min EMG value for EMG_1 and EMG_2
const float emg_max[2] = {3700, 3700}; // Max EMG value for EMG_1 and EMG_2
const float wheel_motor[2] = {0, 1}; // Min and Max value

uint8_t peerAddress[] = { 0x70, 0xB8, 0xF6, 0x72, 0x12, 0x9C };
bool    started       = false;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // optional: print send status
  Serial.print("Send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}


void setup() {
  Serial.begin(115200);

  while (!Serial) { /* wait for Serial */ }
  Serial.println("Type 'start' to begin.");

  // ADC setup
  analogReadResolution(12);
  analogSetPinAttenuation(muscle[0], ADC_11db);
  analogSetPinAttenuation(muscle[1], ADC_11db);

  // ESP-NOW init
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); 
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

void loop() {
  if (!started) {
    if (Serial.available()) {
      String cmd = Serial.readStringUntil('\n');
      cmd.trim();
      if (cmd.equalsIgnoreCase("start")) {
        started = true;
        Serial.println("Sending enabled.");
      }
    }
    return;
  }

  digitalWrite(LED, HIGH);
  float sum[2] = {0, 0};
  for (int i = 0; i < NUM_SAMPLES; i++)
  {
    sum[0] += analogRead(muscle[0]);
    sum[1] += analogRead(muscle[1]);
  }

  avg[0] = sum[0] / NUM_SAMPLES;
  avg[1] = sum[1] / NUM_SAMPLES;

  val[0] = ((avg[0] / emg_max[0]) > 1) ? 1 : avg[0] / emg_max[0];
  val[1] = ((avg[1] / emg_max[1]) > 1) ? 1 : avg[1] / emg_max[1];

  // decide payload
  float left  = (avg[0] > THRESHOLD) ? val[0] : 0;
  float right = (avg[1] > THRESHOLD) ? val[1] : 0;

  // build JSON
  char payload[32];
  snprintf(payload, 
          sizeof(payload), 
          "{\"left\":%.2f,\"right\":%.2f}", left, right);

  // send
  esp_err_t res = esp_now_send(peerAddress,
                               (uint8_t*)payload,
                               strlen(payload));

  // optional debug
  // Serial.print("ADC 1 = ");
  // Serial.print(avg[0]);
  // Serial.print(" → ");
  // Serial.print(",");
  // Serial.print(payload);
  // Serial.print("ADC 2 = ");
  // Serial.print(avg[2]);
  // Serial.print(" → ");
  Serial.print(payload);
  Serial.print(",");

  Serial.print(avg[0]);
  Serial.print(",");
  Serial.print(avg[1]);
  Serial.print(",");

  Serial.print(left);
  Serial.print(",");
  Serial.println(right);

  delay(SEND_INTERVAL);
}
