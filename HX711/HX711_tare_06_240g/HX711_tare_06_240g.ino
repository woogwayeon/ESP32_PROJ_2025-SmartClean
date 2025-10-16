/*
    보정값 계산
    - 빈 상태 RAW ≈ 8,385,848
    - 240g 물체 올린 상태 RAW ≈ 116,874
    - 변화량(Δ) = 8,385,848 − 116,874 = 8,268,974

    8,268,974 (변화량) / 0.24 kg = 34,454,058.3 // 즉 보정값은 = 34454058.3

    // 뭘 찍어도 240g 나오는 이슈
*/

#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale;

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);

    Serial.println("HX711 초기화 중...");
    delay(1500);

    scale.set_scale(34454058.3);  // ← 보정값 입력!
    scale.tare();
    Serial.println("0점 보정 완료! (빈 상태)");
}

void loop() {
    float weight = -scale.get_units(10);  // 반전 유지
    Serial.print("현재 무게: ");
    Serial.print(weight * 1000, 1);  // g 단위로 출력
    Serial.println(" g");
    delay(1000);
}