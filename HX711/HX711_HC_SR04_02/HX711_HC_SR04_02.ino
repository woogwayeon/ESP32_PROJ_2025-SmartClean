// 로드셀이랑 초음파센서 버전 01
#include "HX711.h"

#define TRIG 23
#define ECHO 22
#define DOUT 19   // HX711 데이터 핀
#define CLK 18    // HX711 클럭 핀

HX711 scale;
String lastState = "close";  // 처음 상태

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // HX711 초기화
  scale.begin(DOUT, CLK);
  scale.set_scale(-7050.0); // 보정값은 직접 조정해야 함
  scale.tare();             // 0점 보정
  Serial.println("로드셀 준비완료");
}

void loop() {
  // HC-SR04 sensor
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // 초음파 반사 시간 읽기
  long t = pulseIn(ECHO, HIGH, 30000);  // 30ms 타임아웃
  
  // cm 단위 변환
  float d = t * 0.034 / 2;              

  // 0.00이나 너무 큰 값(노이즈) 무시
  if (d == 0 || d > 400) 
  {
    delay(300);
    return;
  }

  String currentState;

  if (d >= 2 && d <= 6) 
  {
    currentState = "close";
  } 
  else 
  {
    currentState = "open";
  }

  // 상태가 바뀌었을 때만 실행
  if (currentState != lastState) 
  {
    Serial.print("Distance: ");
    Serial.print(d);
    Serial.println(" cm");
    Serial.print("Status: ");
    Serial.println(currentState);

    // OPEN 상태일 때만 무게 측정
    if (currentState == "open") 
    {
      Serial.println("무게 측정 시작");
      float weight = scale.get_units(5); // 5회 평균값 측정
      if (weight < 0) weight = 0;        // 음수 방지
      Serial.print("측정된 무게: ");
      Serial.print(weight, 2);
      Serial.println(" kg");
    }

    lastState = currentState;  // 상태 갱신
  }

  delay(300);
}