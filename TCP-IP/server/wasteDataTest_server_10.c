// 너무 비싸서 g당 1원으로 조정

// 우아한 종료 서버 
// 클라이언트 0 수신 > shutdown(SHUT_WR) > close()
// 서버 무한루프

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
} // error_handling end

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

    // MySQL 연결
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        fprintf(stderr, "DB 초기화 실패\n");
        return 1;
    } // if (conn == NULL) end

    if (!mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0))
    {
        fprintf(stderr, "DB 연결 실패: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    } // if (!mysql_real_connect) end

    printf("DB 연결 성공\n");

    // socket()
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() 에러");

    int opt = 1;
    // 이거 없으면 bind() err (테스트시)
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

    // 서버 while(1)
    while (1)
    {
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
        {
            perror("accept() 에러");
            continue;
        } // if (clnt_sock == -1) end

        printf("클라이언트 연결: %s\n", inet_ntoa(clnt_addr.sin_addr));

        // 연결한 클라이언트랑 통신하는거
        while (1)
        {
            memset(buffer, 0, BUF_SIZE);
            str_len = read(clnt_sock, buffer, BUF_SIZE - 1);

            if (str_len == 0)
            {
                // 끝까지
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

            // 네트워크프로토콜 : @
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

                    // 몇동 몇호
                    char apt_unit[8] = {0};
                    char apt_num[8] = {0};
                    sscanf(house_id, "%7[^-]-%7s", apt_unit, apt_num);

                    // INSERT WasteRecord
                    const char *ipstr = inet_ntoa(clnt_addr.sin_addr);
                    char query[512];
                    sprintf(query,
                        "INSERT INTO WasteRecord(House_ID, Amount, Record_DATE, Record_TIME, IP_addr, MAC_addr) "
                        "VALUES((SELECT House_ID FROM House WHERE APT_UNIT='%s' AND APT_NUM='%s'), %.2f, CURDATE(), CURTIME(), '%s', 'AA:BB:CC:DD:EE:01')",
                        apt_unit, apt_num, amount, ipstr);

                    res = mysql_query(conn, query);
                    if (!res)
                    {
                        printf("DB 저장 완료: %s → %.2fg\n", house_id, amount);

                        char billingQuery[512];
                        sprintf(billingQuery,
                            "INSERT INTO Billing(House_ID, Period, Total_Bill, Status) "
                            "VALUES((SELECT House_ID FROM House WHERE APT_UNIT='%s' AND APT_NUM='%s'), "
                            "DATE_FORMAT(CURDATE(), '%%Y-%%m'), %.2f, 'UNPAID') "
                            "ON DUPLICATE KEY UPDATE Total_Bill = Total_Bill + %.2f",
                            apt_unit, apt_num, add_bill, add_bill);

                        if (mysql_query(conn, billingQuery))
                        {
                            fprintf(stderr, "Billing 업데이트 오류: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
                        }
                        else
                        {
                            printf("Billing 누적 업데이트 완료 (추가 %.2f원)\n", add_bill);
                        }
                    } // if (!res) end
                    else
                    {
                        fprintf(stderr, "DB 저장 오류: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
                    } // else end

                } // if() > g 측정

            } // if() > 101-902 동호수 자른거

        } // while()

        // 클라이언트 종료
        shutdown(clnt_sock, SHUT_WR);
        printf("서버 송신 종료 (shutdown)\n");

        char tmp[64];
        while (read(clnt_sock, tmp, sizeof(tmp)) > 0)
        {
            // 남은 데이터 무시
        } // while(read~) end

        close(clnt_sock);
        printf("클라이언트 연결 종료, 다음 연결 대기\n");

    } // while(1)

    // 소켓종료
    close(serv_sock);
    mysql_close(conn);

    printf("서버 정상 종료\n");
    return 0;
    
} // main end