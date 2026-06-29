#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

#include "PageIndex.h"

#define PulseSensor_PIN 36

const char* ssid = "V30+";
const char* password = "ddn12345";

// ========================== VARIABEL SENSOR ==========================
int PulseSensorSignal = 0;      // sinyal yang sudah difilter, dipakai untuk deteksi & dikirim ke web
int lastValidSignal   = 512;
const int glitchFloor = 120;    // di bawah ini dianggap glitch ADC, bukan sinyal asli

// --- Filter low-pass ringan: meredam noise tinggi tanpa menghilangkan bentuk pulsa ---
float filteredSignal = 512;
const float filterAlpha = 0.35; // 0..1 — makin kecil makin halus tapi makin lambat merespon puncak asli

int UpperThreshold = 520;
int LowerThreshold = 500;
boolean ThresholdStat = true;

// ========================== THRESHOLD ADAPTIF ==========================
float envelopeMax = 600;
float envelopeMin = 400;
const float decayMax = 0.3;
const float decayMin = 0.3;
const float upperThresholdRatio = 0.55;
const float lowerThresholdRatio = 0.45;
const float minEnvelopeRange = 40;
bool fingerDetected = false;

// --- Deteksi jari yang lebih akurat: pakai envelope, BUKAN nilai sinyal sesaat ---
// SESUAIKAN kedua nilai ini lewat Serial Monitor:
// - lihat berapa nilai envelopeMin saat jari benar-benar menempel vs tidak ada jari
// - lihat berapa "range" (envelopeMax - envelopeMin) saat ada detak jelas vs tidak ada jari
const float minBaselineForFinger = 150; // ambang dasar minimum (envelopeMin) saat jari menempel
const float minRangeForFinger    = minEnvelopeRange + 10; // amplitudo minimum dianggap ada detak

// ========================== VARIABEL BPM ==========================
int BPMval = 0;
int cntHB = 0;
bool get_BPM = false;
int timer_Get_BPM = 0;

// --- Refractory period: jeda minimum antar detak, supaya noise di sekitar threshold ---
// --- tidak ikut terhitung sebagai detak tambahan dalam waktu yang sangat singkat ---
unsigned long lastBeatMillis = 0;
const unsigned long minBeatInterval = 300; // ms, ~200 BPM adalah batas wajar tertinggi

byte tSecond = 0, tMinute = 0, tHour = 0;
char tTime[10];

// ========================== WEB SERVER ==========================
const char* PARAM_INPUT_1 = "BTN_Start_Get_BPM";
String BTN_Start_Get_BPM = "";
JSONVar JSON_All_Data;

AsyncWebServer server(80);
AsyncEventSource events("/events");
SemaphoreHandle_t gemBok;

void UpdateAdaptiveThreshold(int signalValue) {

  if (signalValue > envelopeMax)
    envelopeMax = signalValue;
  else
    envelopeMax -= decayMax;

  if (signalValue < envelopeMin)
    envelopeMin = signalValue;
  else
    envelopeMin += decayMin;

  if (envelopeMax < envelopeMin + minEnvelopeRange)
    envelopeMax = envelopeMin + minEnvelopeRange;

  float range = envelopeMax - envelopeMin;

  UpperThreshold = envelopeMin + (range * upperThresholdRatio);
  LowerThreshold = envelopeMin + (range * lowerThresholdRatio);
}

void TaskBacaSensor(void *parameter) {
  unsigned long waktuBPMTerakhir = millis();

  while (true) {
    // 1. Baca sinyal mentah dari sensor
    int rawSignal = analogRead(PulseSensor_PIN);

    // 2. Cek glitch (kalau sinyal jatuh drastis, pakai nilai terakhir yang valid)
    if (rawSignal < glitchFloor) {
      rawSignal = lastValidSignal;
    } else {
      lastValidSignal = rawSignal;
    }

    // 3. Low-pass filter ringan sebelum dipakai untuk deteksi
    //    Ini meredam noise tinggi yang bisa memicu deteksi detak palsu.
    filteredSignal += filterAlpha * (rawSignal - filteredSignal);
    PulseSensorSignal = (int)filteredSignal;

    // 4. Update threshold adaptif berdasarkan sinyal yang sudah difilter
    UpdateAdaptiveThreshold(PulseSensorSignal);

    // ----- KUNCI GEMBOK sebelum ubah variabel BPM -----
    xSemaphoreTake(gemBok, portMAX_DELAY);

      // Deteksi jari berbasis envelope (baseline + amplitudo), bukan nilai sesaat.
      // Lebih stabil karena tidak ikut "kedip" saat sinyal naik-turun karena detak.
      float range = envelopeMax - envelopeMin;
      fingerDetected = (envelopeMin > minBaselineForFinger) && (range > minRangeForFinger);

      if (PulseSensorSignal > UpperThreshold && ThresholdStat == true) {
        unsigned long now = millis();
        // Hitung detak HANYA kalau sudah lewat jeda minimum dari detak sebelumnya.
        // Ini mencegah 1 detak asli terhitung berkali-kali akibat noise di sekitar threshold.
        if (get_BPM && fingerDetected && (now - lastBeatMillis) >= minBeatInterval) {
          cntHB++;
          lastBeatMillis = now;
        }
        ThresholdStat = false;
      }
      if (PulseSensorSignal < LowerThreshold) {
        ThresholdStat = true;
      }

    // ----- BUKA GEMBOK setelah selesai -----
    xSemaphoreGive(gemBok);

    // 5. Hitung BPM setiap 10 detik
    if (millis() - waktuBPMTerakhir >= 1000) {
      waktuBPMTerakhir = millis();

      xSemaphoreTake(gemBok, portMAX_DELAY);

        if (get_BPM) {
          timer_Get_BPM++;

          if (timer_Get_BPM >= 10) {
            timer_Get_BPM = 0;

            tSecond += 10;
            if (tSecond >= 60) { tSecond = 0; tMinute++; }
            if (tMinute >= 60) { tMinute = 0; tHour++; }
            sprintf(tTime, "%02d:%02d:%02d", tHour, tMinute, tSecond);

            BPMval = cntHB * 6;
            cntHB = 0;

            Serial.print("BPM : ");
            Serial.println(BPMval);
          }
        }

      xSemaphoreGive(gemBok);
    }

    // 6. Istirahat 10ms sebelum baca lagi (versi RTOS dari delay)
    vTaskDelay(10 / portTICK_PERIOD_MS);

    // --- Debug opsional: aktifkan kalau perlu kalibrasi ulang nilai minBaselineForFinger / minRangeForFinger ---
    // static unsigned long lastPrint = 0;
    // if (millis() - lastPrint >= 500) {
    //   lastPrint = millis();
    //   Serial.printf("Signal:%d Min:%.1f Max:%.1f UpperTh:%d LowerTh:%d Finger:%s\n",
    //     PulseSensorSignal, envelopeMin, envelopeMax, UpperThreshold, LowerThreshold,
    //     fingerDetected ? "YES" : "NO");
    // }
  }
}

// ==========================
// TASK 2 : KIRIM DATA KE WEB
// ===========================
void TaskKirimData(void *parameter) {

  while (true) {

    // 1. Cek apakah ada tombol Start/Stop ditekan dari web
    if (BTN_Start_Get_BPM == "START" || BTN_Start_Get_BPM == "STOP") {
      BTN_Start_Get_BPM = "";

      xSemaphoreTake(gemBok, portMAX_DELAY);

        cntHB = 0;
        BPMval = 0;
        timer_Get_BPM = 0;
        lastBeatMillis = 0;
        tSecond = 0; tMinute = 0; tHour = 0;
        sprintf(tTime, "%02d:%02d:%02d", tHour, tMinute, tSecond);

        get_BPM = !get_BPM;

        if (get_BPM == true) {
          Serial.println("Start Getting BPM");
        } else {
          Serial.println("STOP");
        }
      xSemaphoreGive(gemBok);
    }

    xSemaphoreTake(gemBok, portMAX_DELAY);

      JSON_All_Data["heartbeat_Signal"] = PulseSensorSignal;
      JSON_All_Data["BPM_TimeStamp"]    = tTime;
      JSON_All_Data["BPM_Val"]          = BPMval;
      JSON_All_Data["BPM_State"]        = get_BPM;
      JSON_All_Data["Finger_Detected"]  = fingerDetected;

    xSemaphoreGive(gemBok);

    String dataKirim = JSON.stringify(JSON_All_Data);
    events.send(dataKirim.c_str(), "allDataJSON", millis());

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  analogReadResolution(10);
  sprintf(tTime, "%02d:%02d:%02d", tHour, tMinute, tSecond);

  // ---------- Koneksi WiFi ----------
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");

  int timeout = 40;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    timeout--;
    if (timeout <= 0) ESP.restart();
  }
  Serial.println("\nWiFi Terhubung!");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());

  // ---------- Halaman Web ----------
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", MAIN_page);
  });

  events.onConnect([](AsyncEventSourceClient * client) {
    client->send("hello!", NULL, millis(), 10000);
  });

  server.on("/BTN_Comd", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam(PARAM_INPUT_1)) {
      BTN_Start_Get_BPM = request->getParam(PARAM_INPUT_1)->value();
    } else {
      BTN_Start_Get_BPM = "No message";
    }
    request->send(200, "text/plain", "OK");
  });

  server.addHandler(&events);
  server.begin();

  // ---------- Buat Mutex ----------
  gemBok = xSemaphoreCreateMutex();

  // ---------- Buat 2 Task ----------
  xTaskCreatePinnedToCore(
    TaskBacaSensor,
    "BacaSensor",
    4096,
    NULL,
    2,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    TaskKirimData,
    "KirimData",
    4096,
    NULL,
    1,
    NULL,
    0
  );

  Serial.println("Kedua task RTOS sudah berjalan!");
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}