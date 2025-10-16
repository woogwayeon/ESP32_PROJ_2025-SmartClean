#include <WiFi.h>

const char* ssid = "MY_SSID";
const char* password = "MY_PASS";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("WiFi 연결 중");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n연결 완료!");
  Serial.print("ESP32 IP 주소: ");
  Serial.println(WiFi.localIP());
}

void loop() {}