/*
  OPEN 또는 CLOSE가 연속으로 찍히지 않게 (한 번만 출력)
  OPEN 상태가 1초 이상 유지될 때만 실제로 OPEN으로 간주
  > OPEN이 1초 이상 유지되면 출력

  두번째 열리면 무게측정 못하는 이슈 있음 (물건 감지)
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

long baseReading = 0;
long lastReading = 0;
String loadcellState = "idle";
long threshold = 10000;   // 변동 폭이 이 이상이면 감지로 판단

void setup()
{
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    scale.begin(DOUT, CLK);
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
        long current = scale.read();
        long diff = current - baseReading;

        if (diff > threshold && loadcellState == "idle")
        {
            Serial.println("물건 감지 ↑");
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
            // 문이 막 열렸을 때 기준값 다시 설정
            baseReading = scale.read();
            Serial.println("기준값 초기화 완료");
        }

        lastStatus = currentStatus;
    }

    // 문이 열린 상태에서만 감지 검사
    if (lastStatus == "OPEN")
    {
        checkLoadcell();
    }

    lastDistance = d;
    delay(300);
}