// 거의 다 왔는데 240g 기준 물체 160g +- 정도 뜸 값조정 더하면 될듯

#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale;

// 240g 기준으로 보정된 값 (10월 12일 실측 결과)
#define CALIB_FACTOR 49593023.9

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);

    Serial.println("HX711 무게 측정 테스트 시작...");
    delay(1000);

    // 보정값 및 0점 보정
    scale.set_scale(-634170.793);
    scale.tare();

    Serial.println("0점 보정 완료! (빈 상태)");
    Serial.println("이제 다양한 무게를 올려보세요.\n");
}

void loop() {
    // 평균값 계산 (20회 측정)
    float weight = scale.get_units(20);  

    // kg → g 단위 변환
    float grams = weight * 1000.0;

    Serial.print("현재 무게: ");
    Serial.print(grams, 1);
    Serial.println(" g");

    delay(1000);  // 1초 간격 측정
}