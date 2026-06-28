#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

#include "PageIndex.h"

#define PulseSensor_PIN 36

const char* ssid = "V30+";
const char* password = "ddn12345";

// ---------------------------- TIMING ----------------------------
unsigned long previousMillisGetHB     = 0;  
unsigned long previousMillisSendEvent = 0;  
unsigned long previousMillisResultHB  = 0;  

const long intervalGetHB     = 10;   
const long intervalSendEvent = 100;  
const long intervalResultHB  = 1000; 

int timer_Get_BPM = 0;

int PulseSensorSignal;
int lastValidSignal = 512; 

int UpperThreshold = 520;
int LowerThreshold = 500;

int cntHB = 0;
boolean ThresholdStat = true;
int BPMval = 0;

bool get_BPM = false;

byte tSecond = 0;
byte tMinute = 0;
byte tHour   = 0;
char tTime[10];

const char* PARAM_INPUT_1 = "BTN_Start_Get_BPM";
String BTN_Start_Get_BPM = "";

JSONVar JSON_All_Data;

AsyncWebServer server(80);
AsyncEventSource events("/events");

float envelopeMax = 600;
float envelopeMin = 400;

const float decayMax = 0.3;
const float decayMin = 0.3;

const float upperThresholdRatio = 0.55;
const float lowerThresholdRatio = 0.45;

unsigned long lastDebugPrint = 0;

const float minEnvelopeRange = 40;

bool fingerDetected = false;

const int glitchFloor = 50;

void UpdateAdaptiveThreshold(int signalValue) {
  if (signalValue > envelopeMax) {
    envelopeMax = signalValue;
  } else {
    envelopeMax -= decayMax;
  }

  if (signalValue < envelopeMin) {
    envelopeMin = signalValue;
  } else {
    envelopeMin += decayMin;
  }

  if (envelopeMax < envelopeMin + minEnvelopeRange) {
    envelopeMax = envelopeMin + minEnvelopeRange;
  }

  float range = envelopeMax - envelopeMin;

  fingerDetected = (range > minEnvelopeRange + 5);

  UpperThreshold = envelopeMin + (range * upperThresholdRatio);
  LowerThreshold = envelopeMin + (range * lowerThresholdRatio);
}

void GetHeartRate() {
  unsigned long currentMillis = millis();

  // ---------------- Sampling sensor (cepat, tiap 10ms) ----------------
  if (currentMillis - previousMillisGetHB >= intervalGetHB) {
    previousMillisGetHB = currentMillis;

    int rawSignal = analogRead(PulseSensor_PIN);

    if (rawSignal < glitchFloor) {
      PulseSensorSignal = lastValidSignal;
    } else {
      PulseSensorSignal = rawSignal;
      lastValidSignal = rawSignal;
    }

    UpdateAdaptiveThreshold(PulseSensorSignal);

    if (PulseSensorSignal > UpperThreshold && ThresholdStat == true) {
      if (get_BPM == true && fingerDetected) cntHB++;
      ThresholdStat = false;
    }

    if (PulseSensorSignal < LowerThreshold) {
      ThresholdStat = true;
    }

    if (millis() - lastDebugPrint >= 1000) {
      lastDebugPrint = millis();
      Serial.print("Signal: "); Serial.print(PulseSensorSignal);
      Serial.print(" | Min: "); Serial.print(envelopeMin);
      Serial.print(" | Max: "); Serial.print(envelopeMax);
      Serial.print(" | UpperTh: "); Serial.print(UpperThreshold);
      Serial.print(" | LowerTh: "); Serial.print(LowerThreshold);
      Serial.print(" | Finger: "); Serial.print(fingerDetected ? "YES" : "NO");
      Serial.print(" | FreeHeap: "); Serial.println(ESP.getFreeHeap());
    }
  }

  if (currentMillis - previousMillisSendEvent >= intervalSendEvent) {
    previousMillisSendEvent = currentMillis;

    JSON_All_Data["heartbeat_Signal"] = PulseSensorSignal;
    JSON_All_Data["BPM_TimeStamp"] = tTime;
    JSON_All_Data["BPM_Val"] = BPMval;
    JSON_All_Data["BPM_State"] = get_BPM;
    JSON_All_Data["Finger_Detected"] = fingerDetected;

    String JSON_All_Data_Send = JSON.stringify(JSON_All_Data);
    events.send(JSON_All_Data_Send.c_str(), "allDataJSON", millis());
  }

  // ---------------- Perhitungan BPM tiap siklus 10 detik ----------------
  unsigned long currentMillisResultHB = millis();

  if (currentMillisResultHB - previousMillisResultHB >= intervalResultHB) {
    previousMillisResultHB = currentMillisResultHB;

    if (get_BPM == true) {
      timer_Get_BPM++;

      if (timer_Get_BPM >= 10) {
        timer_Get_BPM = 0;

        tSecond += 10;
        if (tSecond >= 60) { tSecond = 0; tMinute += 1; }
        if (tMinute >= 60) { tMinute = 0; tHour += 1; }

        sprintf(tTime, "%02d:%02d:%02d", tHour, tMinute, tSecond);

        BPMval = cntHB * 6;
        Serial.print("BPM : ");
        Serial.println(BPMval);

        cntHB = 0;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  analogReadResolution(10);

  sprintf(tTime, "%02d:%02d:%02d",  tHour, tMinute, tSecond);

  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.println("-------------");

  delay(100);

  Serial.println("------------");
  Serial.println("WIFI STA");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int connecting_process_timed_out = 20;
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if (connecting_process_timed_out > 0) connecting_process_timed_out--;
    if (connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("------------");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", MAIN_page);
  });

  events.onConnect([](AsyncEventSourceClient * client) {
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });

  server.on("/BTN_Comd", HTTP_GET, [] (AsyncWebServerRequest * request) {
    if (request->hasParam(PARAM_INPUT_1)) {
      BTN_Start_Get_BPM = request->getParam(PARAM_INPUT_1)->value();
      Serial.println();
      Serial.print("BTN_Start_Get_BPM : ");
      Serial.println(BTN_Start_Get_BPM);
    }
    else {
      BTN_Start_Get_BPM = "No message";
    }
    request->send(200, "text/plain", "OK");
  });

  server.addHandler(&events);
  server.begin();

  Serial.println();
  Serial.println("------------");
  Serial.print("ESP32 IP address : ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println("Visit the IP Address above in your browser to open the main page.");
  Serial.println("------------");
  Serial.println();
}

void loop() {

  if (BTN_Start_Get_BPM == "START" || BTN_Start_Get_BPM == "STOP") {
    delay(100);

    BTN_Start_Get_BPM = "";
    cntHB = 0;
    BPMval = 0;
    timer_Get_BPM = 0;

    tSecond = 0;
    tMinute = 0;
    tHour   = 0;

    sprintf(tTime, "%02d:%02d:%02d", tHour, tMinute, tSecond);

    get_BPM = !get_BPM;

    if (get_BPM == true) {
      Serial.println("Start Getting BPM");
    } else {
      Serial.println("STOP");
    }
  }

  GetHeartRate();
}
