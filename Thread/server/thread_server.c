#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 100
#define PORT 9190

void* handle_clnt(void* arg);
void error_handling(char* message);

int main(void)
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
    { 
        error_handling("socket() error");
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
    {    
        error_handling("bind() error");
    }

    if (listen(serv_sock, 5) == -1)
    {
        error_handling("listen() error");
    }

    printf("멀티서버 실행중 (PORT %d)\n", PORT);

    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        if (clnt_sock == -1)
        {
            continue;
        }

        printf("클라이언트 연결: %s\n", inet_ntoa(clnt_adr.sin_addr));

        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock); // thread 만들기
        pthread_detach(t_id);  // 스레드 정리
    }

    close(serv_sock);
    return 0;
}

void* handle_clnt(void* arg)
{
    int clnt_sock = *((int*)arg);
    int str_len;
    char msg[BUF_SIZE];

    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
    {
        msg[str_len] = '\0';
        printf("%s과 연결되었습니당다다당\n", msg);
    }

    close(clnt_sock);
    printf("클라이언트 종료\n");
    return NULL;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}