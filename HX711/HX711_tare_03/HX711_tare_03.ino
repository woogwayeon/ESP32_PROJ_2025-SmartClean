#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale;

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);

    Serial.println("HX711 초기화 중...");
    delay(1500);

    // 임시 스케일 (보정 전)
    scale.set_scale();
    scale.tare();   // 0점 맞추기

    Serial.println("0점 보정 완료!");
}

void loop() {
    if (scale.is_ready()) {
        float reading = scale.get_units(10); // 평균값
        Serial.print("현재 무게: ");
        Serial.print(reading, 2);
        Serial.println(" 단위 없음(raw 기준)");
    } else {
        Serial.println("HX711 not ready");
    }
    delay(1000);
}