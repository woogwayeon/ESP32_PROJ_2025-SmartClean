// 로드셀에 무게가 올라오면 딱 그 순간만 감지해서 1회 출력
// 시간관계상 일단 감지만 구현

// 센서가 죽은거 확인해보는 코드

#include <HX711.h>

#define DOUT 19
#define CLK 18

HX711 scale;

void setup()
{
    Serial.begin(9600);
    scale.begin(DOUT, CLK);
    Serial.println("HX711 raw data test start");
}

void loop()
{
    if (scale.is_ready())
    {
        long reading = scale.read();
        Serial.println(reading);
    }
    else
    {
        Serial.println("HX711 not ready");
    }
    delay(200);
}