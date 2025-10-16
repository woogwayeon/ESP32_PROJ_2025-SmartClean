#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale;

// 기준물 무게(g) 입력
#define REF_G 240.0

void wait_key() {
    while (Serial.available()) Serial.read();
    while (!Serial.available()) { delay(10); }
    while (Serial.available()) Serial.read();
}
 
void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);
    delay(1000);

    // 초기화
    scale.set_scale();      // 보정 제거
    scale.tare();           // 영점
    Serial.println("보정 시작: 빈 상태에서 엔터");
    wait_key();

    long r0 = scale.read_average(15);
    Serial.print("빈 RAW: "); Serial.println(r0);

    Serial.println("기준물(240g)을 올리고 엔터");
    wait_key();

    long r1 = scale.read_average(15);
    Serial.print("하중 RAW: "); Serial.println(r1);

    long d = r0 - r1;       // 변화량(부호 유지)
    Serial.print("RAW 변화량: "); Serial.println(d);

    // 보정계수 계산 (kg 단위 환산용)
    // (raw - offset) / scale = kg  =>  scale = d / (REF_G/1000.0)
    double scale_factor = (double)d / (REF_G / 1000.0);

    // 반전 보정: 기준물을 올렸을 때 r1이 r0보다 작으면 부호가 +
    // r1이 더 크면 = 반대 방향 => 부호 뒤집기
    if (scale_factor < 0) scale_factor = -scale_factor;

    Serial.print("계산된 보정계수: "); Serial.println(scale_factor, 3);

    scale.set_scale(scale_factor);
    scale.tare();

    Serial.println("보정 완료. 측정 시작.");
}

void loop() {
    // 필요시 부호 반전 (측정값이 음수면 - 붙이기)
    double kg = scale.get_units(10);
    double g = kg * 1000.0;
    Serial.print("현재 무게: ");
    Serial.print(g, 1);
    Serial.println(" g");
    delay(500);
}