// 우아한 종료 서버 
// 클라이언트 0 수신 > shutdown(SHUT_WR) > close()
// 날아오는데이터 참조키 오류남

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <time.h>

#define PORT 9190
#define BUF_SIZE 1024

// DB 연결 정보
char *host = "localhost";
char *user = "MY_DBID";
char *pass = "MY_DBPW";
char *dbname = "iotdb";

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main()
{
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char buffer[BUF_SIZE];
    int str_len;
    int res;

    MYSQL *conn;

    // 1. MySQL 연결
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        fprintf(stderr, "DB 초기화 실패: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
        return 1;
    }

    if (!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
    {
        fprintf(stderr, "DB 연결 실패: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
        mysql_close(conn);
        return 1;
    }
    printf("DB 연결 성공\n");

    // 2. 서버 소켓 생성 및 설정
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
    {
        error_handling("bind() 에러");
    }

    if (listen(serv_sock, 5) == -1)
    {    
        error_handling("listen() 에러");
    }

    printf("서버 대기 중 (PORT %d)\n", PORT);

    // 3. 클라이언트 연결 수락
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
    {
        error_handling("accept() 에러");
    }

    printf("클라이언트 연결: %s\n", inet_ntoa(clnt_addr.sin_addr));

    // 4. 데이터 수신 루프
    while (1)
    {
        memset(buffer, 0, BUF_SIZE);
        str_len = read(clnt_sock, buffer, BUF_SIZE - 1);

        // read() == 0 → 클라이언트한테 0 수신 (정상 close)
        if (str_len == 0)
        {
            printf("클라이언트 종료 (0 수신)\n");
            break;
        }
        else if (str_len < 0)
        {
            perror("read() 오류");
            break;
        }

        buffer[str_len] = '\0';
        printf("수신 데이터: %s\n", buffer);

        // @ 구분자로 파싱
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

                // DB용 ID 추출
                char house_id_db[30];
                strcpy(house_id_db, house_id);
                char *at_pos = strchr(house_id_db, '@');
                
                if (at_pos != NULL)
                    *at_pos = '\0';

                char query[256];
                sprintf(query,
                        "INSERT INTO WasteRecord(House_ID, Amount, Record_DATE, Record_TIME, IP_addr, MAC_addr) "
                        "VALUES('%s', %.2f, CURDATE(), CURTIME(), '192.168.0.10', 'AA:BB:CC:DD:EE:01')",
                        house_id_db, amount);

                res = mysql_query(conn, query);
                if (!res)
                {    
                    printf("DB 저장 완료: %s → %.2fg\n", house_id_db, amount);
                }
                else
                {
                    fprintf(stderr, "DB 저장 오류: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
                }
            }
        }
    }

    // 5. 서버 → 클라이언트 종료
    shutdown(clnt_sock, SHUT_WR);
    printf("서버 종료\n");

    // 클라이언트 FIN 수신 대기
    char tmp[64];
    while (read(clnt_sock, tmp, sizeof(tmp)) > 0)
    {
        // 남은 데이터가 있으면 무시
    }

    // 6. 소켓 및 DB 종료
    close(clnt_sock);
    close(serv_sock);
    mysql_close(conn);

    printf("서버 정상 종료\n");
    return 0;
}