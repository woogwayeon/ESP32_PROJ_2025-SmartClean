#include <WiFi.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <unistd.h>

// Wi-Fi 설정
const char* ssid = "MY_SSID";
const char* password = "MY_PASS";

// 서버 정보
const char* host = "MY_IP";
const int port = 9190;

// 하우스 아이디 (JSP에서 사용할 사용자 구분용)
const char* house_id = "101-902";

// 전송 주기 설정 (밀리초)
unsigned long lastSendTime = 0;
unsigned long sendInterval = 5000;  // 5초마다 전송

void setup()
{
    Serial.begin(9600);
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
}

void loop()
{
    if (millis() - lastSendTime >= sendInterval)
    {
        sendToServer();
        lastSendTime = millis();
    }
}

void sendToServer()
{
    int sock;
    struct sockaddr_in server;
    char msg[64];
    char recv_buf[64];

    // 소켓 생성
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0)
    {
        Serial.println("socket() 실패");
        return;
    }

    // 서버 주소 설정
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(host);
    server.sin_port = htons(port);

    // 서버 연결
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        Serial.println("connect() 실패");
        close(sock);
        return;
    }

    // 테스트용 무게값 (30~100g)
    int raw = random(3000, 10001);
    float weight = (float)raw / 100.0f;

    sprintf(msg, "%s@%.2f", house_id, weight);

    // 데이터 전송
    send(sock, msg, strlen(msg), 0);
    Serial.printf("전송된 데이터: %s\n", msg);

    // 서버 응답 수신
    int len = recv(sock, recv_buf, sizeof(recv_buf) - 1, 0);
    if (len > 0)
    {
        recv_buf[len] = '\0';
        Serial.printf("서버 응답: %s\n", recv_buf);
    }

    // 소켓 닫기
    close(sock);
}