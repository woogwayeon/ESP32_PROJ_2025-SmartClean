#include <HX711.h>

#define DOUT 19
#define CLK 18

HX711 scale;

float calibration_factor = -410449.430 / 240.0; // 기존: -410449.430
// 위 줄은 "카운트/그램"으로 환산한 값. 240 g 기준 raw 차이(-410449.430)를 240으로 나눔.

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);
    scale.set_scale(calibration_factor);

    // HX711 안정화 루틴
    delay(3000);  // 전원 인가 후 워밍업
    for(int i=0; i<10; i++) scale.read();  // 초기 더미 데이터 버림

    // HX711 준비 상태 확인 후 tare 수행
    while(!scale.is_ready()) {
        Serial.println("Waiting for HX711...");
        delay(100);
    }
    scale.tare();
    delay(3000);
    Serial.println("Tare complete");
}

void loop() {
    if(!scale.is_ready()) {
        Serial.println("HX711 not ready");
        delay(500);
        return;
    }

    float grams = scale.get_units(10); // 평균값
    Serial.print("현재 무게 : ");
    Serial.println(grams, 2);
    delay(500);
}