#include <HX711.h>

#define DOUT 19
#define CLK 18

HX711 scale;

float calibration_factor = -410449.430; // 나중에 실험으로 갱신할 값

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);
    scale.set_scale(1.0);  // 일단 raw값을 보기 위해 1.0으로 설정
    delay(3000);
    for (int i = 0; i < 10; i++) scale.read();

    while (!scale.is_ready()) {
        Serial.println("Waiting for HX711...");
        delay(100);
    }

    scale.tare();
    delay(3000);
    Serial.println("Tare complete");
    Serial.println("240g을 올리고 raw값을 확인하세요.");
}

void loop() {
    if (!scale.is_ready()) {
        Serial.println("HX711 not ready");
        delay(500);
        return;
    }

    long raw = scale.get_value(10);
    Serial.print("raw: ");
    Serial.println(raw);
    delay(500);
}