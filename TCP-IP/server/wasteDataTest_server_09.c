// 우아한 종료 서버 
// 클라이언트 0 수신 > shutdown(SHUT_WR) > close()
// MYSQL_RES, MYSQL_ROW 사용하지 않음
// 서버는 무한루프로 항상 대기, 여러 클라이언트 처리 가능

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

    // 1. MySQL 연결
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
        error_handling("bind() 에러");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() 에러");

    printf("서버 대기 중 (PORT %d)\n", PORT);

    // 3. 무한루프: 클라이언트 요청 계속 처리
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

        // 클라이언트와 통신
        while (1)
        {
            memset(buffer, 0, BUF_SIZE);
            str_len = read(clnt_sock, buffer, BUF_SIZE - 1);

            if (str_len == 0)
            {
                printf("클라이언트 종료 (0 수신)\n");
                break;
            } // if (str_len == 0) end
            else if (str_len < 0)
            {
                perror("read() 오류");
                break;
            } // else if (str_len < 0) end

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

                    // "101-902" → APT_UNIT, APT_NUM 분리
                    char apt_unit[8] = {0};
                    char apt_num[8] = {0};
                    sscanf(house_id, "%7[^-]-%7s", apt_unit, apt_num);

                    // SELECT 서브쿼리로 House_ID 바로 조회하여 삽입
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
                    } // if (!res) end
                    else
                    {
                        fprintf(stderr, "DB 저장 오류: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
                    } // else end

                    // Billing 테이블 업데이트 (이게 없어서 값 갱신 안되는 이슈생김)
                    int house_pk = 0;
                    MYSQL_RES *r = mysql_use_result(conn);
                    if (r)
                    {
                        MYSQL_ROW row = mysql_fetch_row(r);
                        if (row)
                            house_pk = atoi(row[0]);
                        mysql_free_result(r);
                    } // if (r) end

                    float unit_price = 30.0;
                    float add_bill = amount * unit_price;

                    char check_query[256];
                    sprintf(check_query,
                        "SELECT COUNT(*) FROM Billing WHERE House_ID = (SELECT House_ID FROM House WHERE APT_UNIT='%s' AND APT_NUM='%s') "
                        "AND Period = DATE_FORMAT(CURDATE(), '%%Y-%%m')",
                        apt_unit, apt_num);

                    res = mysql_query(conn, check_query);
                    if (!res)
                    {
                        MYSQL_RES *rs = mysql_store_result(conn);
                        if (rs)
                        {
                            MYSQL_ROW row = mysql_fetch_row(rs);
                            int exists = atoi(row[0]);
                            mysql_free_result(rs);

                            if (exists == 0)
                            {
                                char insert_bill[512];
                                sprintf(insert_bill,
                                    "INSERT INTO Billing(House_ID, Period, Total_Bill, Status) "
                                    "VALUES((SELECT House_ID FROM House WHERE APT_UNIT='%s' AND APT_NUM='%s'), "
                                    "DATE_FORMAT(CURDATE(), '%%Y-%%m'), %.2f, 'UNPAID')",
                                    apt_unit, apt_num, add_bill);

                                res = mysql_query(conn, insert_bill);
                                if (!res)
                                {
                                    printf("Billing 새 행 추가: %s (%s-%s) %.2f원\n", house_id, apt_unit, apt_num, add_bill);
                                } // if (!res) end
                                else
                                {
                                    fprintf(stderr, "Billing INSERT 오류: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
                                } // else end
                            } // if (exists == 0) end
                            else
                            {
                                char update_bill[512];
                                sprintf(update_bill,
                                    "UPDATE Billing SET Total_Bill = Total_Bill + %.2f "
                                    "WHERE House_ID = (SELECT House_ID FROM House WHERE APT_UNIT='%s' AND APT_NUM='%s') "
                                    "AND Period = DATE_FORMAT(CURDATE(), '%%Y-%%m')",
                                    add_bill, apt_unit, apt_num);

                                res = mysql_query(conn, update_bill);
                                if (!res)
                                {
                                    printf("Billing 누적 갱신 완료: %s (+%.2f원)\n", house_id, add_bill);
                                } // if (!res) end
                                else
                                {
                                    fprintf(stderr, "Billing UPDATE 오류: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
                                } // else end
                            } // else end
                        } // if (rs) end
                    } // if (!res) end
                    
                } // if (token != NULL) end
            } // if (token != NULL) end
        } // 내부 수신 while end

        // 클라이언트 종료 시
        shutdown(clnt_sock, SHUT_WR);
        printf("서버 송신 종료 (shutdown)\n");

        char tmp[64];
        while (read(clnt_sock, tmp, sizeof(tmp)) > 0)
        {
            // 남은 데이터 무시
        } // while (read) end

        close(clnt_sock);
        printf("클라이언트 연결 종료, 다음 연결 대기\n");
    } // 외부 while(1) end

    // 4. DB 및 서버 소켓 종료 (이 루프는 실제로 종료 안 됨)
    close(serv_sock);
    mysql_close(conn);

    printf("서버 정상 종료\n");
    return 0;
} // main end