#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale;

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);

    Serial.println("HX711 초기화 중...");
    delay(1500); // 안정화 대기

    // HX711 기본 세팅
    scale.set_scale();  // 보정값 제거
    scale.tare();       // 현재 상태를 0으로 보정
    Serial.println("0점 보정 완료! (빈 상태 기준)");

    delay(1000);
}

void loop() {
    long raw = scale.read_average(10);
    float weight = scale.get_units(10); // 기본값은 아직 보정X

    Serial.print("RAW: ");
    Serial.print(raw);
    Serial.print("  /  현재 무게: ");
    Serial.print(weight, 3);  // kg 단위로 출력됨 (아직 정확하진 않음)
    Serial.println(" kg");

    delay(1000);
}