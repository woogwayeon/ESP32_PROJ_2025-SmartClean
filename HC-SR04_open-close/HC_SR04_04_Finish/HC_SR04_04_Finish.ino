/*
  OPEN 또는 CLOSE가 연속으로 찍히지 않게 (한 번만 출력)
  OPEN 상태가 1초 이상 유지될 때만 실제로 OPEN으로 간주

  > OPEN이 1초 이상 유지되면 출력
  
*/

#define TRIG 23
#define ECHO 22

float lastDistance = 0;
String lastStatus = "CLOSE";
unsigned long openStartTime = 0;

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

        if (d > 0.5 && d < 400) {
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

    String currentStatus;

    if (d >= 6) {
        if (openStartTime == 0) {
            openStartTime = millis();
        }
        if (millis() - openStartTime >= 1000) {  // 1초 이상 유지
            currentStatus = "OPEN";
        } else {
            currentStatus = lastStatus;  // 아직 대기
        }
    } else {
        openStartTime = 0;
        currentStatus = "CLOSE";
    }

    if (currentStatus != lastStatus) {
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