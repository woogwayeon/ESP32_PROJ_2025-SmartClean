#define TRIG 23
#define ECHO 22

String lastState = "close";  // 처음 상태

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  
  // 초음파 신호 전송
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // 초음파 반사 시간 읽기
  long t = pulseIn(ECHO, HIGH, 30000);  // 30ms 타임아웃
  
  // cm 단위변환
  float d = t * 0.034 / 2;              

  // 0.00 오류값 잡아줌
  if (d == 0 || d > 400) {
    // Serial.println("무시무시무시");
    delay(300);
    return;
  }
  String currentState;

  if (d >= 2 && d <= 6) 
  {
    currentState = "close";
  } 
  else 
  {
    currentState = "open";
  }

  // 상태 변화가 있을 때만 출력
  if (currentState != lastState) {
    Serial.print("Distance: ");
    Serial.print(d);
    Serial.println(" cm");
    Serial.print("Status: ");
    Serial.println(currentState);

    lastState = currentState;
  }

  delay(300);
}