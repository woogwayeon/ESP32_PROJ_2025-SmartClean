// 서버와 통신 성공 코드
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "MY_SSID";
const char* password = "MY_PASS";

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

  if (WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http;
    String url = "http://192.168.nn.nn:8080/test.jsp";
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "value=ESP32테스트";
    int httpCode = http.POST(postData);
   
    Serial.print("응답 코드: ");
    Serial.println(httpCode);

    if (httpCode > 0) 
    {
      String response = http.getString();
      Serial.println("서버 응답: " + response);
    } 
    else 
    {
      Serial.println("서버 연결 실패");
    }

    http.end();
  } 
  else 
  {
    Serial.println("WiFi 연결 실패...");
  }
}

void loop() {}