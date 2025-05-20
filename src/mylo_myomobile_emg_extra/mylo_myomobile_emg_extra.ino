#include <esp_now.h>
#include <WiFi.h>
#define LEFT 35 // Pin as seen on board: D35
#define RIGHT 34 // Pin as seen on board: D34
#define LED_1 27 // Pin as seen on board: D27
#define LED_2 26 // Pin as seen on board: D26
#define NUM_SAMPLES 10
#define THRESHOLD 500 // ADC threshold
#define SEND_INTERVAL 50 // ms (~10hz)

// FUNCTIONS AND INITIALIZATIONS

const int muscle[4]  = {LEFT, RIGHT};
float     emg_avg[2] = {0, 0};
float     val[2]     = {0, 0};

float emg_val[2] = {0, 0};
// float emg_min[2] = {0, 20}; // Min EMG value for EMG_1 and EMG_2
float emg_max[2] = {3900, 3500}; // Max EMG value for EMG_1 and EMG_2
const float wheel_motor[2] = {0, 1}; // Min and Max value

uint8_t peerAddress[] = { 0x70, 0xB8, 0xF6, 0x72, 0x12, 0x9C };
bool    started       = false;

int       count_main     = 0;
// int       count_baseline = 0;
int       count_max      = 0;
// bool      baseline_read  = false;
bool      max_read       = false;
// const int baseline_size  = 2000 / SEND_INTERVAL;
const int max_size       = 5000 / SEND_INTERVAL;

// uint32_t current_time = 0;

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

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
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

  // current_time = milis();
  emg_val[0] = analogRead(muscle[0]);
  emg_val[1] = analogRead(muscle[1]);

  if (max_read)
  {
    count_main = (count_main >= NUM_SAMPLES) ? 0 : count_main;

    emg_avg[0] += emg_val[0];
    emg_avg[1] += emg_val[1];
    count_main++;

    if (count_main == NUM_SAMPLES)
    {
      emg_avg[0] /= NUM_SAMPLES;
      emg_avg[1] /= NUM_SAMPLES;

      val[0] = ((emg_avg[0] / emg_max[0]) > 1) ? 1 : emg_avg[0] / emg_max[0];
      val[1] = ((emg_avg[1] / emg_max[1]) > 1) ? 1 : emg_avg[1] / emg_max[1];

      // decide payload
      float left  = (emg_avg[0] > THRESHOLD) ? val[0] : 0;
      float right = (emg_avg[1] > THRESHOLD) ? val[1] : 0;

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
      // Serial.print(emg_avg[0]);
      // Serial.print(" → ");
      // Serial.print(",");
      // Serial.print(payload);
      // Serial.print("ADC 2 = ");
      // Serial.print(emg_avg[2]);
      // Serial.print(" → ");

      // Serial.print(payload);
      // Serial.print(",");

      // Serial.print(emg_avg[0]);
      // Serial.print(",");
      // Serial.print(emg_avg[1]);
      // Serial.print(",");

      // Serial.print(left);
      // Serial.print(",");
      // Serial.println(right);

      Serial.print(emg_max[0]);
      Serial.print(',');
      Serial.println(emg_max[1]);
    }
  }
  else // Get max EMG for 5 seconds
  {
    digitalWrite(LED_1, HIGH); 
    emg_max[0] = (emg_max[0] <= emg_val[0]) ? emg_val[0] : emg_max[0];
    emg_max[1] = (emg_max[1] <= emg_val[1]) ? emg_val[1] : emg_max[1];

    count_max++;
    max_read = (count_max == max_size) ? true : false;

    (count_max == max_size) ? digitalWrite(LED_2, HIGH) : digitalWrite(LED_2, LOW);
  }
  // else if (!baseline_read) // Get average baseline for 2 seconds
  // {
  //   emg_min[0] += emg_val[0];
  //   emg_min[1] += emg_val[1];
  //   count_baseline++;
  //   baseline_read = (count_baseline == baseline_size) ? true : false;

  //   if (count_baseline == baseline_size)
  //   {
  //     emg_min[0] = int(round(emg_min[0] /= baseline_size));
  //     emg_min[1] = int(round(emg_min[1] /= baseline_size));
  //     digitalWrite(LED_1, HIGH);
  //   }
  // }

  delay(SEND_INTERVAL);
}
