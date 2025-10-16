#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale;

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);

    Serial.println("HX711 초기화 중...");
    delay(1500);

    scale.set_scale();
    scale.tare();
    Serial.println("0점 보정 완료! (빈 상태)");
    delay(1000);
    Serial.println("이제 240g 물체를 올려주세요!");
}

void loop() {
    long raw = scale.read_average(10);
    float weight = -scale.get_units(10);  // ← 부호 반전!
    
    Serial.print("RAW: ");
    Serial.print(raw);
    Serial.print("  /  계산된 무게: ");
    Serial.print(weight, 3);
    Serial.println(" kg");

    delay(1000);
}