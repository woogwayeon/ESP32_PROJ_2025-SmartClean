#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale;

// 수정된 보정값 (160g → 240g으로 미세보정)
//#define CALIB_FACTOR 422780.529
#define CALIB_FACTOR 410449.430

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);

    Serial.println("센서 안정화 중...");
    delay(3000); // 전원 안정화

    // HX711 노이즈 버림
    for (int i = 0; i < 10; i++) scale.read();

    // 영점 보정
    Serial.println("0점 설정 중...");
    scale.tare();
    Serial.println("0점 보정 완료!");
}

void loop() {
    float weight = scale.get_units(20);
    float grams = weight * 1000.0;

    Serial.print("현재 무게: ");
    Serial.print(grams, 1);
    Serial.println(" g");

    delay(1000);
}