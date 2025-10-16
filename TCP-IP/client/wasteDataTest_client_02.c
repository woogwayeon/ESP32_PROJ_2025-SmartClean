#include <WiFi.h>
#include <HX711.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#define TRIG 23
#define ECHO 22
#define DOUT 19
#define CLK 18

const char* ssid = "MY_SSID";
const char* password = "MY_PASS";
const char* host = "MY_IP";
const int port = 9190;

HX711 scale;

void setup() 
{
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    scale.begin(DOUT, CLK);
    scale.set_scale(-7050.0);
    scale.tare();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi 연결 완료");
}

void loop() 
{
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) 
    {
        Serial.println("서버 연결 실패");
        close(sock);
        delay(3000);
        return;
    }

    // 테스트용 데이터 전송
    char msg[64];
    sprintf(msg, "101-101@%.2f", 53.27);
    send(sock, msg, strlen(msg), 0);
    Serial.printf("전송: %s\n", msg);

    close(sock);
    delay(5000);
}