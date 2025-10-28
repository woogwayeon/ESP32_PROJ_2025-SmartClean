// 소켓의 우아한 종료 shutdown(sock, SHUT_WR) 추가
// ESP32 클라이언트 코드

// 클라이언트가 open하면 요청>무게측정>데이터전송>닫힘>클라이언트 우아한종료 / 다시 오픈하면켜짐.. 무한반복 

#include <WiFi.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <unistd.h>
#include "HX711.h"

// 센서 핀
#define TRIG 23     // 초음파
#define ECHO 22     // 초음파
#define DOUT 19     // 로드셀
#define CLK 18      // 로드셀

void connectToServer();
void sendToServer(float amount);
float getDistance();
void checkLoadcell();

int sock;     

// WiFi 및 서버 정보
const char* ssid = "SK_WiFiGIGA92E0_2.4G";
const char* password = "DWQ41@5117";
const char* host = "192.168.45.99";
const int port = 9190;
const char* house_id = "101-902";

// HC-SR04 초음파센서
float lastDistance = 0;         
String lastStatus = "CLOSE";    
unsigned long openStartTime = 0;

// HX711 tlqkf
HX711 scale;
float baseReading = 0.0f;           // 새로 무게잴때 기준 
String loadcellState = "idle";      // idle(긱본) / detected(감지) 
float threshold = 0.05f;            // 채터링
unsigned long detectStart = 0;      // 시작시간
bool isStable = false;              // 서버 전송 플래그                  


void setup()
{
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    WiFi.begin(ssid, password);
    Serial.print("WiFi 연결중");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");

    } // while (WiFi.status() != WL_CONNECTED) end

    Serial.println();
    Serial.print("WiFi 연결! IP : ");
    Serial.println(WiFi.localIP());

    scale.begin(DOUT, CLK);
    scale.set_scale(-7050.0);
    scale.tare();
    delay(1000);
    
    baseReading = scale.get_units(5);   // HX711에 있는 평균내주는 고마운함수

    randomSeed((uint32_t)esp_random());
    Serial.println("ESP32 연결 시작");

    connectToServer();
} // setup()

void loop()
{
    while (1)
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
        } // if(d > 6)

        if (currentStatus != lastStatus)
        {
            Serial.print("Distance: ");
            Serial.print(d);
            Serial.println(" cm");
            Serial.print("Status: ");
            Serial.println(currentStatus);

            if (currentStatus == "OPEN")
            {
                baseReading = scale.get_units(5);
                loadcellState = "idle";
                detectStart = 0;
                isStable = false;
                Serial.println("초기값 기준 설정 완료");

                // 문열림 > 서버연결 > 로드셀체크 > 닫힘 > 소켓닫기
                Serial.println("OPEN : 서버 연결 시작");
                connectToServer();
                checkLoadcell();  
                Serial.println("무게 전송 완료");
                
            } // if (currentStatus == OPEN)

            if (currentStatus == "CLOSE")
            {
                Serial.println("닫힘 감지 : 클라이언트 종료 준비");
                if (sock >= 0)
                {
                    shutdown(sock, SHUT_WR);
                    Serial.println("shutdown()");

                    char tmp[64];
                    while (read(sock, tmp, sizeof(tmp)) > 0) { } // 서버 0 대기
                    
                    close(sock);
                    sock = -1;      // 파일디스크립터 오류방지
                    
                    Serial.println("소켓 종료");
                }
                Serial.println("CLOSE : 대기 모드");
            } // if (currentStatus == CLOSE)

            lastStatus = currentStatus;
        } // if (status changed) end

        lastDistance = d;
        delay(300);

    } // while()

} // loop()


/* 함수 */

// 거리 측정
float getDistance()
{
    long sum = 0;
    int count = 0;

    for (int i = 0; i < 5; i++)
    {
        digitalWrite(TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);

        long duration = pulseIn(ECHO, HIGH, 30000);
        float cm = duration * 0.034 / 2;

        if (cm > 0.5 && cm < 400) // 올바른값만
        {
            sum += cm;
            count++;  
        }
        delay(10);
    }

    // 만약 10번 측정했는데 전부 튀면 전에꺼
    if (count == 0) return lastDistance;

    return (float)sum / count;
} // getDistance()

// 로드셀
void checkLoadcell()
{
    if (scale.is_ready())
    {
        float current = scale.get_units(5);
        float diff = current - baseReading;

        if (diff > threshold)
        {
            if (detectStart == 0)
            {
                detectStart = millis();
            } // if (detectStart == 0) end

            if (millis() - detectStart >= 1000 && loadcellState == "idle" && !isStable)
            {
                Serial.println("물건 감지됨");

                int raw = random(3000, 10001);
                float amount = (float)raw / 100.0f;

                Serial.print("무게: ");
                Serial.print(amount, 2);
                Serial.println(" g");

                sendToServer(amount);

                loadcellState = "detected";
                isStable = true;
            } // if (stable detect) end
        } // if (diff > threshold) end
        else
        {
            detectStart = 0;
            isStable = false;

            if (loadcellState == "detected")
            {
                Serial.println("물건 제거됨");
                loadcellState = "idle";
                sendToServer(0.00f);

            } // if (loadcellState == detected)

        } // else (diff <= threshold)

    } // if (scale.is_ready())

} // checkLoadcell()


// 소켓통신
void connectToServer()
{
    struct sockaddr_in server;
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        Serial.println("socket() 실패");
        return;
    }

    // 주소정보 해주는거
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(host);
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        Serial.println("connect() 실패");
        close(sock);
        sock = -1;
    }
} // connectToServer()

// 서버 전송
void sendToServer(float amount)
{
    char msg[64];
    int sockfd;

    if (sock < 0) // 와이파이 끊김있어서 ㄱ계속연결 할수잇게
    {
        connectToServer();
        if (sock < 0)
        {
            Serial.println("서버 연결 실패 (재시도)");
            return;
        }
    }

    sprintf(msg, "%s@%.2f", house_id, amount);  // 일단 집주소는 상수선언함..ㅎ

    sockfd = send(sock, msg, strlen(msg), 0);
    
    if (sockfd < 0)
    {
        Serial.println("전송 실패");
        close(sock);
        sock = -1;
     
        return;
    }

    Serial.printf("전송 완료: %s\n", msg);

    // shutdown() : 우아하게 송신끝
    shutdown(sock, SHUT_WR);
    
    char tmp[64];
    while (read(sock, tmp, sizeof(tmp)) > 0) { } // 서버 0 대기
    
    close(sock);
    sock = -1;
    
    Serial.println("소켓 정상 종료");

} // sendToServer()

