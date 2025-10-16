// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <mysql/mysql.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// LwIP 네트워크 스택용 헤더 : ESP32용 TCP 클라이언트가 따로있음;;
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <unistd.h>

// 우리집 와이파이..ㅎㅋㅎ
const char* ssid = "MY_SSID";
const char* password = "MY_PASS";

// server IP
const char* host = "192.168.45.99";
const int port = 9190;

void setup() 
{
    Serial.begin(9600);
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
    int sock;
    int len;
    struct sockaddr_in server;
    char msg[64];
    char recv_buf[64];

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (sock < 0) 
    {
        Serial.println("socket() 실패");
        delay(2000);
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
        delay(2000);
        return;
    }

    // send() - id랑 측정량 전송
    sprintf(msg, "101-902@%.2f", 123.45);  // test
    send(sock, msg, strlen(msg), 0);
    Serial.printf("전송된거: %s\n", msg);

    // recv()
    len = recv(sock, recv_buf, sizeof(recv_buf) - 1, 0); // 0이 기본 받기모드
    
    if (len > 0) 
    {
        recv_buf[len] = '\0';
        Serial.printf("서버 응답: %s\n", recv_buf);
    }

    close(sock);
    delay(5000);
}

