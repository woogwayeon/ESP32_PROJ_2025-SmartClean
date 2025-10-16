#define TRIG 23
#define ECHO 22

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long t = pulseIn(ECHO, HIGH);
  float d = t * 0.034 / 2;   // 거리 계산 (cm)

  Serial.print("Distance: ");
  Serial.print(d);
  Serial.println(" cm");

  delay(500);
}