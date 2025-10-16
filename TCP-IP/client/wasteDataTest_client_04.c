// 물건 감지 시점에 데이터 전송 테스트
#include <WiFi.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <unistd.h>
#include "HX711.h"

#define TRIG 23
#define ECHO 22
#define DOUT 19
#define CLK 18

HX711 scale;

// Wi-Fi 설정
const char* ssid = "MY_SSID";
const char* password = "MY_PASS";

// 서버 정보
const char* host = "MY_IP";
const int port = 9190;

// 하우스 아이디
const char* house_id = "101-902";

float lastDistance = 0;
String lastStatus = "CLOSE";
unsigned long openStartTime = 0;

float baseReading = 0.0f;
float lastReading = 0.0f;
String loadcellState = "idle";
float threshold = 0.05f;
unsigned long detectStart = 0;
bool isStable = false;

void setup()
{
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    // WiFi 연결
    WiFi.begin(ssid, password);
    Serial.print("WiFi 연결 중");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("WiFi 연결 완료, IP: ");
    Serial.println(WiFi.localIP());

    // 로드셀 초기화
    scale.begin(DOUT, CLK);
    scale.set_scale(-7050.0);
    scale.tare();
    delay(1000);
    baseReading = scale.get_units(5);

    randomSeed((uint32_t)esp_random());
    Serial.println("Ultrasonic + HX711 + TCP test start");
}

float getDistance()
{
    long sum = 0;
    int validCount = 0;

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
            validCount++;
        }
        delay(10);
    }

    if (validCount == 0) return lastDistance;
    return (float)sum / validCount;
}

void sendToServer(float amount)
{
    int sock;
    struct sockaddr_in server;
    char msg[64];
    char recv_buf[64];

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0)
    {
        Serial.println("socket() 실패");
        return;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(host);
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        Serial.println("connect() 실패");
        close(sock);
        return;
    }

    sprintf(msg, "%s@%.2f", house_id, amount);
    send(sock, msg, strlen(msg), 0);
    Serial.printf("전송된 데이터: %s\n", msg);

    int len = recv(sock, recv_buf, sizeof(recv_buf) - 1, 0);
    if (len > 0)
    {
        recv_buf[len] = '\0';
        Serial.printf("서버 응답: %s\n", recv_buf);
    }

    close(sock);
}

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
            }

            if (millis() - detectStart >= 1000 && loadcellState == "idle" && !isStable)
            {
                Serial.println("물건 감지 ↑");

                int raw = random(3000, 10001);   // 30~100g
                float amount = (float)raw / 100.0f;

                Serial.print("측정 무게: ");
                Serial.print(amount, 2);
                Serial.println(" g");

                sendToServer(amount);

                loadcellState = "detected";
                isStable = true;
            }
        }
        else
        {
            detectStart = 0;
            isStable = false;

            if (loadcellState == "detected")
            {
                Serial.println("물건 내려감 ↓");
                loadcellState = "idle";
                sendToServer(0.00f);
            }
        }

        lastReading = current;
    }
}

void loop()
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
    }

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
            Serial.println("기준값 및 상태 초기화 완료");
        }

        lastStatus = currentStatus;
    }

    if (lastStatus == "OPEN")
    {
        checkLoadcell();
    }

    lastDistance = d;
    delay(300);
}