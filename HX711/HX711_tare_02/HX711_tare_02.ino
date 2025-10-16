#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale;

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);

    Serial.println("HX711 초기화 중...");
    delay(1000);  // 안정화 대기

    scale.set_scale(-7050.0);   // 임시 보정값
    scale.tare();               // 0점 설정

    Serial.println("0점 보정 완료");
}

void loop() {
    float reading = scale.get_units(10);  // 10회 평균
    reading = reading * 1000;             // kg → g 변환
    Serial.print("현재 무게: ");
    Serial.print(reading, 1);
    Serial.println(" g");

    delay(1000);
}