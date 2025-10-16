#include <HX711.h> // 로드셀 라이브러리

#define calibration_factor -7050.0 // 로드셀 스케일 값
#define DOUT 3                     // 엠프 데이터 아웃 핀 넘버 선언
#define CLK 2                      // 엠프 클락 핀 넘버

HX711 scale(DOUT, CLK);            // 엠프 핀 선언

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 scale Test");
  scale.set_scale(calibration_factor);Í // 스케일 지정
  scale.tare();                         // 스케일 설정
  Serial.println("readings : ");
}

void loop() {
  Serial.print("reading : ");
  Serial.print(scale.get_units(), 1);  // 무제 출력
  Serial.print(" lbs");                // 단위
  Serial.println();
  delay(1000);
}
