#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "MY_SSID";      // 집 와이파이 이름
const char* password = "MY_PASS";       // 와이파이 비밀번호

void setup() {

  Serial.begin(9600);
  delay(1000);
  WiFi.begin(ssid, password);
  Serial.print("WiFi 연결 중");

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi 연결 완료!");
  Serial.print("ESP32 IP 주소: ");
  Serial.println(WiFi.localIP());

  HTTPClient http;
  String url = "http://192.168.nn.nn:8080/test.jsp"; // 서버 IP 넣기
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String postData = "value=ESP32 테스트중";
  int code = http.POST(postData);
  Serial.print("응답 코드: ");
  Serial.println(code);

  String response = http.getString();
  Serial.println("서버 응답: " + response);

  http.end();
}

void loop() {}