#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>

#define PORT 9190
#define BUF_SIZE 1024

void error_handling(char* message);

char *host = "localhost";
char *user = "MY_DBID";
char *pass = "MY_DBPW";        
char *dbname = "iotdb";

int main() 
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char buffer[BUF_SIZE];
    int res;

    MYSQL *conn;
    conn = mysql_init(NULL);
    if (conn == NULL) 
    {
        fprintf(stderr, "mysql_init() 실패\n");
        exit(1);
    }

    if (!mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)) 
    {
        fprintf(stderr, "DB 연결 실패: %s\n", mysql_error(conn));
        exit(1);
    }
    printf("DB 연결 성공\n");

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() 에러");

    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() 에러");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() 에러");

    printf("서버 대기중... PORT %d\n", PORT);

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
        error_handling("accept() 에러");

    printf("클라이언트 연결: %s\n", inet_ntoa(clnt_addr.sin_addr));

    while (1)
    {
        memset(buffer, 0, BUF_SIZE);
        int str_len = read(clnt_sock, buffer, BUF_SIZE - 1);
        if (str_len <= 0) break;

        buffer[str_len] = '\0';
        printf("수신 데이터: %s\n", buffer);

        char house_id[30];
        float amount = 0.0;
        char *token;

        token = strtok(buffer, "@");
        if (token != NULL)
        {
            strcpy(house_id, token);
            token = strtok(NULL, "@");

            if (token != NULL)
            {
                amount = atof(token);

                char query[256];
                sprintf(query,
                        "INSERT INTO WasteRecord(House_ID, Amount, Record_DATE, Record_TIME, IP_addr, MAC_addr) "
                        "VALUES ((SELECT House_ID FROM House WHERE User_ID='%s'), %.2f, CURDATE(), CURTIME(), '192.168.45.99', 'AA:BB:CC:DD:EE:FF')",
                        house_id, amount);

                res = mysql_query(conn, query);
                if (!res)
                    printf("DB 저장 성공: %s → %.2fg\n", house_id, amount);
                else
                    fprintf(stderr, "DB 저장 실패: %s\n", mysql_error(conn));
            }
        }
    }

    close(clnt_sock);
    close(serv_sock);
    mysql_close(conn);
    printf("서버 종료\n");
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}