// gcc testworld_server.c -o t_server
// sudo ./t_server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char* message);

#define PORT 9190
#define BUF_SIZE 1024

int main() 
{
    // 소켓 변수
    int serv_sock;
    int clnt_sock;
    int str_len;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char buffer[BUF_SIZE];

    // 네트워크프로토콜 : @ 로 정함
    char house_id[30];
    float amount = 0.0;
    char *token;

    // TCP socket
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) 
    {
        error_handling("socket() 에러");
        return 1;
    }

    // bind error 때문에 추가함 -> 포트 계속 사용중인 이슈 새로 쓸 수 있게 해결해줌
    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) 
    {
        error_handling("bind() 에러");
        close(serv_sock);
        return 1;
    }

    if (listen(serv_sock, 5) == -1) 
    {
        error_handling("listen() 에러");
        close(serv_sock);
        return 1;
    }

    printf("서버 시작 : PORT  %d\n", PORT);

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    if (clnt_sock == -1) 
    {
        error_handling("accept() 에러");
        close(serv_sock);
        return 1;
    }

    printf("클라이언트 연결: %s\n", inet_ntoa(clnt_addr.sin_addr));

    // 받기
    while (1) 
    {
        memset(buffer, 0, BUF_SIZE);
        
        str_len = read(clnt_sock, buffer, BUF_SIZE - 1);
        if (str_len <= 0)
        { 
            break;
        }

        buffer[str_len] = '\0';
        printf("데이터: %s\n", buffer);

        // 간단 응답 추가
        char response[] = "OK";
        write(clnt_sock, response, strlen(response));
    }

    // 회선 닫기
    close(clnt_sock);
    close(serv_sock);
    printf("서버 종료\n");

    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}