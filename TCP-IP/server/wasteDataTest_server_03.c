#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <time.h>

void error_handling(char* message);

#define PORT 9190
#define BUF_SIZE 1024

// DB 접속 정보
char * host = "localhost";
char * user = "MY_DBID";
char * pass = "MY_DBPW";
char * dbname = "iotdb";

int main() 
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char buffer[BUF_SIZE];
    int res;

    // MySQL 연결 초기화
    MYSQL *conn;
    conn = mysql_init(NULL);

    if (conn == NULL) 
    {
        fprintf(stderr,"DB 초기화 실패: %s [%d]\n",mysql_error(conn),mysql_errno(conn));
        return 1;
    }

    // DB 연결
    if (!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0))) 
    {
        fprintf(stderr,"DB 연결 실패: %s [%d]\n",mysql_error(conn),mysql_errno(conn));
        mysql_close(conn);
        return 1;
    }

    printf("DB 연결 성공\n");

    // TCP 서버 소켓 생성
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) 
    {
        error_handling("socket() 에러");
        return 1;
    }

    // 포트 재사용 옵션 설정 (서버 재시작 시 포트 점유 방지)
    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 서버 주소 구조체 설정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    // 소켓 바인드
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) 
    {
        error_handling("bind() 에러");
    }

    // 연결 대기열 생성
    if (listen(serv_sock, 5) == -1) 
    {
        error_handling("listen() 에러");
    }

    printf("서버 실행 중... PORT %d\n", PORT);

    // 서버가 꺼지지 않도록 무한 루프
    while (1) 
    {
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) 
        {
            fprintf(stderr, "accept() 실패\n");
            continue;
        }

        printf("클라이언트 연결: %s\n", inet_ntoa(clnt_addr.sin_addr));

        // 데이터 수신 루프 (클라이언트 연결이 유지되는 동안)
        while (1)
        {
            memset(buffer, 0, BUF_SIZE);
            int str_len = read(clnt_sock, buffer, BUF_SIZE - 1);

            if (str_len <= 0)
            { 
                printf("클라이언트 연결 종료\n");
                close(clnt_sock);
                break;
            }

            buffer[str_len] = '\0';
            printf("수신 데이터: %s\n", buffer);

            // 수신된 문자열 파싱 (예: "101-902@53.27")
            char house_id[30];
            float amount = 0.0;
            char *token = strtok(buffer, "@");

            if (token != NULL)
            {
                // 첫 번째 토큰: House_ID
                strcpy(house_id, token);
                token = strtok(NULL, "@");

                if (token != NULL)
                {
                    // 두 번째 토큰: Amount
                    amount = atof(token);

                    // INSERT 쿼리 작성
                    char query[256];
                    sprintf(query,
                        "INSERT INTO WasteRecord (House_ID, Amount, Record_DATE, Record_TIME, IP_addr, MAC_addr) "
                        "VALUES ((SELECT House_ID FROM House WHERE User_ID='%s' LIMIT 1), %.2f, CURDATE(), CURTIME(), '192.168.45.99', 'AA:BB:CC:DD:EE:FF')",
                        house_id, amount);

                    // 쿼리 실행
                    res = mysql_query(conn, query);

                    if (!res)
                    {
                        printf("DB 저장 성공: %s → %.2fg\n", house_id, amount);
                    }
                    else
                    {
                        fprintf(stderr, "DB 저장 실패: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
                    }
                }
            }
        }
    }

    // 이 부분은 이론적으로 도달하지 않음 (무한 루프)
    mysql_close(conn);
    close(serv_sock);
    return 0;
}

// 에러 처리 함수
void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}