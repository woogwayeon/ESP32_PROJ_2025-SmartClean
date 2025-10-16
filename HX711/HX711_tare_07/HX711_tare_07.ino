/*
	뭘 올려도 240g이 찍히는건 보정계수(set_scale)가 너무 커서 모든 RAW 차이를 240g 근처로 잘라먹는 상태라는 것 > 캘리브레이션 계산이 한 번 틀렸던 것이지 센서나 배선 문제는 아니니까 삽질하지 말자

	-	“RAW 변화량 → g 변환 비율”(set_scale)이 실제보다 100배 이상 과대 설정되어 있는걸로 추정된다
	-	작은 RAW 변화도 곱셈 뒤에는 “무조건 수백 g 이상”으로 출력
	-	반응은 정상인데 스케일이 완전히 틀려먹음

	원인 추정

	HX711 라이브러리는 내부적으로 get_units() 계산 시 (raw - offset) / scale 을 수행한다!
	
	우리가 scale.set_scale(34454058.3)을 준 순간, RAW가 약간만 변해도 g 계산 결과가 0~수백으로 뻥튀기된다~

	RAW차이 / scale = weight
	8268974 / 34454058.3 ≈ 0.24  → 여기까진 OK (보정 맞음)
	
	하지만 실시간 RAW 변동폭이 ±10만만 돼도
	100000 / 34454058 ≈ 2.9g // 문제 없어야 하는데, 음수 반전 + 평균 범위 문제 + 샘플링 횟수 혼동 때문에 평균 잡힌 raw 차가 고정 → 무조건 240 근처 고정된다..

	그리고 아직도 로드셀 구조 바꾸는중
*/

#include "HX711.h"

#define DOUT 19
#define CLK 18

HX711 scale;

// 기준 물체 무게 (240ml 물 = 약 240g = 0.24kg)
#define CAL_WEIGHT 0.24

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT, CLK);

    Serial.println("HX711 초기화 중...");
    delay(1500);

    // 1. 영점 보정
    scale.set_scale();      // 초기 보정값 제거
    scale.tare();           // 현재 상태를 0으로 설정
    Serial.println("영점 보정 완료 (빈 상태)");
    delay(1000);

    // 2. 빈 상태 RAW 값 읽기
    long raw0 = scale.read_average(10);
    Serial.print("빈 RAW: ");
    Serial.println(raw0);

    Serial.println("기준 물체(240g)를 올려주세요...");
    delay(5000);  // 물체 올릴 시간 대기

    // 3. 하중 상태 RAW 값 읽기
    long raw1 = scale.read_average(10);
    Serial.print("하중 RAW: ");
    Serial.println(raw1);

    // 4. 보정 계수 계산
    float calibration_factor = (float)(raw0 - raw1) / CAL_WEIGHT;
    Serial.print("계산된 보정 계수: ");
    Serial.println(calibration_factor, 3);

    // 5. 보정 계수 적용
    scale.set_scale(calibration_factor);
    scale.tare();
    Serial.println("보정 적용 완료!");
    Serial.println();
    delay(1000);
}

void loop() {
    // 6. 측정 모드
    float weight = -scale.get_units(10);   // 반전 유지
    Serial.print("현재 무게: ");
    Serial.print(weight * 1000, 1);        // g 단위 출력
    Serial.println(" g");
    delay(1000);
}