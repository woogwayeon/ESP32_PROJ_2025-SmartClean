/*
    OPEN 또는 CLOSE가 연속으로 찍히지 않게 (한 번만 출력)
    OPEN 상태가 1초 이상 유지될 때만 실제로 OPEN으로 간주
    > OPEN이 1초 이상 유지되면 출력

    두번째 열려도 계속 값 받아올 수 있게 수정함
    물체 감지 시 30g ~ 1100g 사이 랜덤값 출력
    로드셀 보정값 적용 및 get_units() 방식 변경

    cale.read() → scale.get_units(5)를 단독 코드처럼 보정된 g 단위 값으로 읽기
    set_scale(-7050.0) / tare() 추가
    로드셀 보정값 반영
    threshold 단위를 0.05f로 변경 (g 단위에 맞게 조정)
    판정 유지 시간을 증가시켜서 오류 줄이기
*/

#include "HX711.h"

#define TRIG 23
#define ECHO 22
#define DOUT 19
#define CLK 18

HX711 scale;

float lastDistance = 0;
String lastStatus = "CLOSE";
unsigned long openStartTime = 0;

float baseReading = 0.0f;
float lastReading = 0.0f;
String loadcellState = "idle";
float threshold = 0.05f;   // 변동 폭이 이 이상이면 감지로 판단

void setup()
{
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    scale.begin(DOUT, CLK);
    scale.set_scale(-7050.0);   // 보정값 적용
    scale.tare();               // 0점 설정
    delay(1000);
    baseReading = scale.get_units(5);   // 기준값 저장

    randomSeed((uint32_t)esp_random());
    Serial.println("Ultrasonic + HX711 trigger test start");
}

float getDistance()
{
    long sum = 0;
    int validCount = 0;

    for (int i = 0; i < 5; i++)
    {
        digitalWrite(TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);

        long t = pulseIn(ECHO, HIGH, 30000);
        float d = t * 0.034 / 2;

        if (d > 0.5 && d < 400)
        {
            sum += d;
            validCount++;
        }
        delay(10);
    }

    if (validCount == 0) return lastDistance;
    return (float)sum / validCount;
}

void checkLoadcell()
{
    if (scale.is_ready())
    {
        float current = scale.get_units(5);
        float diff = current - baseReading;

        if (diff > threshold && loadcellState == "idle")
        {
            Serial.println("물건 감지 ↑");

            int raw = random(3000, 110001);   // 30g~1100g 범위
            float amount = (float)raw / 100.0f;

            Serial.print("측정 무게: ");
            Serial.print(amount, 2);
            Serial.println(" g");

            loadcellState = "detected";
        }

        if (diff < -threshold && loadcellState == "detected")
        {
            Serial.println("물건 내려감 ↓");
            loadcellState = "idle";
        }

        lastReading = current;
    }
}

void loop()
{
    float d = getDistance();
    String currentStatus;

    if (d >= 6)
    {
        if (openStartTime == 0)
        {
            openStartTime = millis();
        }
        if (millis() - openStartTime >= 1000)
        {
            currentStatus = "OPEN";
        }
        else
        {
            currentStatus = lastStatus;
        }
    }
    else
    {
        openStartTime = 0;
        currentStatus = "CLOSE";
    }

    if (currentStatus != lastStatus)
    {
        Serial.print("Distance: ");
        Serial.print(d);
        Serial.println(" cm");
        Serial.print("Status: ");
        Serial.println(currentStatus);

        if (currentStatus == "OPEN")
        {
            baseReading = scale.get_units(5);   // OPEN 시 기준값 갱신
            loadcellState = "idle";             // 상태 초기화
            Serial.println("기준값 및 상태 초기화 완료");
        }

        lastStatus = currentStatus;
    }

    if (lastStatus == "OPEN")
    {
        checkLoadcell();
    }

    lastDistance = d;
    delay(300);
}