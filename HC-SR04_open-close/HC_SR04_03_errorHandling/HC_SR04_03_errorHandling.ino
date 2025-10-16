/*
  OPEN 또는 CLOSE가 연속으로 찍히지 않게 (한 번만 출력)
  OPEN 상태가 1초 이상 유지될 때만 실제로 OPEN으로 간주
  전체 구조, 들여쓰기, 주석, 줄바꿈 그대로 유지

  > 기본 안정화 버전 (지금 상태 그대로 한 번만 출력)
  
*/

#define TRIG 23
#define ECHO 22

float lastDistance = 0;
String lastStatus = "CLOSE";

void setup() {
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
}

float getDistance() {
    long sum = 0;
    int validCount = 0;

    for (int i = 0; i < 5; i++) {  // 5회 측정 평균
        digitalWrite(TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);

        long t = pulseIn(ECHO, HIGH, 30000);  // 30ms 타임아웃
        float d = t * 0.034 / 2;

        if (d > 0.5 && d < 400) {  // 0.5~400cm 사이만 유효
            sum += d;
            validCount++;
        }
        delay(10);
    }

    if (validCount == 0) return lastDistance;
    return (float)sum / validCount;
}

void loop() {
    float d = getDistance();

    String currentStatus = (d >= 6) ? "OPEN" : "CLOSE";

    if (currentStatus != lastStatus) {  // 바뀔 때만 출력
        Serial.print("Distance: ");
        Serial.print(d);
        Serial.println(" cm");
        Serial.print("Status: ");
        Serial.println(currentStatus);

        lastStatus = currentStatus;
    }

    lastDistance = d;
    delay(300);
}


