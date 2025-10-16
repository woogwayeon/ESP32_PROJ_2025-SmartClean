#include <HX711.h>

#define DOUT 19
#define CLK 18

HX711 scale;

float calibration_factor = -410449.430; // 초기 보정값 (240g 기준 실험값 근처)

// 필요 시, 아래처럼 미세조정 가능 (예: 값이 너무 작으면 *10, 너무 크면 /10)
// float calibration_factor = -410449.430 * 10;

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);
    scale.set_scale(calibration_factor);

    // HX711 안정화 루틴
    delay(3000);  // 전원 인가 후 워밍업
    for (int i = 0; i < 10; i++) scale.read();  // 초기 더미 데이터 버림

    while (!scale.is_ready()) {
        Serial.println("Waiting for HX711...");
        delay(100);
    }

    scale.tare();  // 0점 보정
    delay(3000);
    Serial.println("Tare complete");
}

void loop() {
    if (!scale.is_ready()) {
        Serial.println("HX711 not ready");
        delay(500);
        return;
    }

    // 10회 평균값으로 측정 안정화
    float grams = scale.get_units(10);
    float kg = grams / 1000.0;

    Serial.print("현재 무게 : ");
    Serial.print(grams, 2);
    Serial.print(" g  (");
    Serial.print(kg, 3);
    Serial.println(" kg)");

    delay(500);
}