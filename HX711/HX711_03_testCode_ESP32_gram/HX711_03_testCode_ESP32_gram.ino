#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale(DOUT, CLK);

float calibration_factor = 7050.0;

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 2kg 로드셀 테스트");

  scale.begin(DOUT, CLK);
  scale.set_gain(128); // 128: default, 10Hz 안정적
  scale.set_scale(calibration_factor);
  scale.tare();

  Serial.println("----------");
  Serial.println("교정 안내:");
  Serial.println("1. 아무 것도 올리지 않고 0이 나오게 tare() 실행");
  Serial.println("2. 정확한 추(예: 1kg)를 올리고 raw 값 확인");
  Serial.println("3. calibration_factor 계산 후 코드에 적용");
  Serial.println("----------");
}

void loop() {
  long raw_value = scale.read();
  Serial.print("Raw value : ");
  Serial.println(raw_value);

  float weight_lbs = scale.get_units();
  float weight_g = weight_lbs * 453.59237;
  float weight_kg = weight_g / 1000.0;

  Serial.print("Weight : ");
  Serial.print(weight_g, 1);
  Serial.print(" g  /  ");
  Serial.print(weight_kg, 3);
  Serial.println(" kg");

  delay(1000);
}