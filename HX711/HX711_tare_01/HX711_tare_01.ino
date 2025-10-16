#include "HX711.h"

#define DOUT 19
#define CLK 18
#define TARE_BTN 25    // 수동 0점 버튼

HX711 scale;

void setup() {
  Serial.begin(9600);
  pinMode(TARE_BTN, INPUT_PULLUP);

  scale.begin(DOUT, CLK);

  Serial.println("HX711 초기화 중...");
  delay(1000);  // 안정화 대기

  scale.set_scale(-7050.0);   // 임시 보정값
  scale.tare();               // 0점 설정

  Serial.println("0점 보정 완료");
}

void loop() {
  // 수동 0점 버튼 누르면 다시 영점 보정
  if (digitalRead(TARE_BTN) == LOW) {
    Serial.println("0점 다시 보정중...");
    scale.tare();
    delay(500);
    Serial.println("0점 보정 완료");
  }

  // 현재 측정값 출력
  float reading = scale.get_units(10);  // 10회 평균
  Serial.print("현재 무게: ");
  Serial.print(reading, 3);
  Serial.println(" kg");

  delay(1000);
}
