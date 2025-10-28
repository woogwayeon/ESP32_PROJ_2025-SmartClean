#include <WiFi.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <unistd.h>

#define LED_PIN 2

const char* ssid = "fusion01";
const char* password = "123456789";
const char* host = "192.168.0.13";  // 서버 IP
const int port = 9190;

const char* house_id = "1번"; 

void setup()
{
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.print("WiFi 연결중");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi 연결 완료");
    Serial.println(WiFi.localIP());
}

void loop()
{
    int sock;
    struct sockaddr_in server;
    char msg[64];

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0)
    {
        Serial.println("소켓 생성 실패");
        delay(2000);
        return;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        Serial.println("서버 연결 실패");
        close(sock);
        delay(2000);
        return;
    }

    // 서버 연결 성공 시 LED ON
    digitalWrite(LED_PIN, HIGH);
    Serial.println("서버 연결 성공 (LED ON)");
    delay(2000);

    sprintf(msg, "%s", house_id);
    send(sock, msg, strlen(msg), 0);
    Serial.printf("보냄: %s\n", msg);

    // 통신 종료
    close(sock);
    Serial.println("소켓 종료");

    // LED OFF
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF (다음 통신 대기)");

    delay(5000);
}