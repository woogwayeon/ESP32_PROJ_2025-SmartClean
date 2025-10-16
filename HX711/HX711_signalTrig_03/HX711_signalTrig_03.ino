// 로드셀에 무게가 올라오면 딱 그 순간만 감지해서 1회 출력
// 시간관계상 일단 감지만 구현

// 변동폭 확인

#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale;

long lastReading = 0;
String lastState = "idle";
long threshold = 10000;   // 변동 폭이 이 이상이면 감지로 판단

void setup()
{
    Serial.begin(9600);
    scale.begin(DOUT, CLK);
    Serial.println("HX711 detect trigger test start");
}

void loop()
{
    if (scale.is_ready())
    {
        long current = scale.read();
        long diff = current - lastReading;

        // 물체 올라옴 (idle → detected)
        if (diff > threshold && lastState == "idle")
        {
            Serial.println("물건 감지 ↑");
            lastState = "detected";
        }

        // 물체 내려감 (detected → idle)
        if (diff < -threshold && lastState == "detected")
        {
            Serial.println("물체 내려감 ↓");
            lastState = "idle";
        }

        lastReading = current;
    }

    delay(200);
}