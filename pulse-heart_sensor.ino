#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "";
const char* password = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.begin(ssid, password, 6);
  while(WiFi.status() != WL_CONNECTED){
    delay(300);
    Serial.print(".");
  }
  Serial.println("Menunggu Koneksi WiFi");
  Serial.print("Tersambung. Alamat IP: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // put your main code here, to run repeatedly:
  HTTPClient httpControl;
  httpControl.begin(url + "control");
  int httpRespCode = httpControl.GET();

  if(httpRespCode > 0){
      Serial.println(httpRespCode);
      String payload = httpControl.getString();
      Serial.println("Succes")
      Serial.println(payload);

  }else{
      Serial.println(payload)
      Serial.println("Error");
  }
      httpControl.end();


}
