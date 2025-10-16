#define TRIG 23
#define ECHO 22

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  // 초음파 신호 발사
  digitalWrite(TRIG, LOW);
  digitalWrite(TRIG, HIGH);
  digitalWrite(TRIG, LOW);

  // 반사 시간 측정
  long t = pulseIn(ECHO, HIGH);
  float d = t * 0.034 / 2;   // 거리 계산 (cm)

  // 거리 출력
  Serial.print("Distance: ");
  Serial.print(d);
  Serial.println(" cm");

  // 문 열림 / 닫힘 조건
  if (d >= 6) 
  {
    Serial.println("Status: OPEN");
  } 
  else 
  {
    Serial.println("Status: CLOSE");
  }

  delay(500);
}