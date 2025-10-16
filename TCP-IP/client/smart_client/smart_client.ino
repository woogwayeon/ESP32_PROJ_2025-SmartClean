// 소켓의 우아한 종료 shutdown(sock, SHUT_WR) 추가
// ESP32 클라이언트 코드

// 클라이언트가 open하면 요청>무게측정>데이터전송>닫힘>클라이언트 우아한종료 / 다시 오픈하면켜짐.. 무한반복 

#include <WiFi.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <unistd.h>
#include "HX711.h"

// 센서 핀
#define TRIG 23
#define ECHO 22
#define DOUT 19
#define CLK 18

HX711 scale;

// WiFi 및 서버 정보
const char* ssid = "MY_SSID";
const char* password = "MY_PASS";
const char* host = "MY_IP";
const int port = 9190;
const char* house_id = "101-902";

float lastDistance = 0;         
String lastStatus = "CLOSE";    
unsigned long openStartTime = 0;

float baseReading = 0.0f;       
String loadcellState = "idle";  
float threshold = 0.05f;        
unsigned long detectStart = 0;  
bool isStable = false;          

int sock;                       


// WiFi 및 로드셀 초기화
void setup()
{
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    WiFi.begin(ssid, password);
    Serial.print("WiFi 연결 중");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");

    } // while (WiFi.status() != WL_CONNECTED) end

    Serial.println();
    Serial.print("WiFi 연결 완료, IP: ");
    Serial.println(WiFi.localIP());

    scale.begin(DOUT, CLK);
    scale.set_scale(-7050.0);
    scale.tare();
    delay(1000);
    
    baseReading = scale.get_units(5);

    randomSeed((uint32_t)esp_random());
    Serial.println("ESP32 연결 시작");

    connectToServer();
} // setup end

// TCP 서버 연결
void connectToServer()
{
    struct sockaddr_in server;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0)
    {
        Serial.println("socket() 실패");
        return;
    } // if (sock < 0) end

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(host);
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        Serial.println("connect() 실패");
        close(sock);
        sock = -1;
    } // if (connect < 0) end
    else
    {
        Serial.println("서버 연결 완료");
    } // else (connect ok) end
} // connectToServer end

// 초음파 센서 거리 측정
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

        long t = pulseIn(ECHO, HIGH, 30000);
        float d = t * 0.034 / 2;

        if (d > 0.5 && d < 400)
        {
            sum += d;
            count++;
        } // if (valid d) end
        delay(10);
    } // for (i) end

    if (count == 0) return lastDistance; // if (count == 0) end
    return (float)sum / count;
} // getDistance end

// 서버로 전송
void sendToServer(float amount)
{
    if (sock < 0)
    {
        connectToServer();
        if (sock < 0)
        {
            Serial.println("서버 연결 실패 (재시도)");
            return;
        } // if (sock < 0) after reconnect end
    } // if (sock < 0) end

    char msg[64];
    sprintf(msg, "%s@%.2f", house_id, amount);

    int sent = send(sock, msg, strlen(msg), 0);
    if (sent < 0)
    {
        Serial.println("전송 실패");
        close(sock);
        sock = -1;
        return;
    } // if (sent < 0) end

    Serial.printf("전송 완료: %s\n", msg);

    // shutdown() : 송신끝
    shutdown(sock, SHUT_WR);
    Serial.println("송신 종료 (shutdown)");
    char tmp[64];
    while (read(sock, tmp, sizeof(tmp)) > 0) { } // 서버 0 대기
    close(sock);
    sock = -1;
    Serial.println("소켓 정상 종료");

} // sendToServer end

// 로드셀 무게 감지 및 전송
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
            } // if (loadcellState == detected) end
        } // else (diff <= threshold) end
    } // if (scale.is_ready()) end
} // checkLoadcell end

// 문 열림 감지 및 로드셀 제어
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
            } // if (openStartTime == 0) end
            if (millis() - openStartTime >= 1000)
            {
                currentStatus = "OPEN";
            } // if (1s passed) end
            else
            {
                currentStatus = lastStatus;
            } // else (keep lastStatus) end
        } // if (d >= 6) end
        else
        {
            openStartTime = 0;
            currentStatus = "CLOSE";
        } // else (d < 6) end

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
                Serial.println("기준값 초기화 완료");

                // 로직추가
                Serial.println("OPEN : 서버 연결 시작");
                connectToServer();
                checkLoadcell();  
                Serial.println("무게 전송 완료");
                
            } // if (currentStatus == OPEN) end

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
                    sock = -1;
                    Serial.println("소켓 정상 종료");
                }
                Serial.println("CLOSE : 대기 상태 복귀");
            } // if (currentStatus == CLOSE) end

            lastStatus = currentStatus;
        } // if (status changed) end

        lastDistance = d;
        delay(300);
    } // while (1) end
} // loop end