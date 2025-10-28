// 251023 총량 쿼리 추가 > ESP 전송까지
// 251022 multi thread 추가
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <pthread.h>
#include <time.h>

#define PORT 9190
#define BUF_SIZE 1024

void* handleClient(void* arg);
void freeList();
void printList();
void insertLastNode(char* houseId, float amount);
void error_handling(char* message);

// 데이터 연결리스트 활용
typedef struct Node
{
    char house_id[15];
    float amount;
    struct Node* link;
} Node;

Node* head = NULL;

// DB 연결 정보
char *host = "localhost";
char *user = "root";
char *pass = "poly";
char *dbname = "iotdb";

// accept 전달 구조체 (주소를 스레드로 전달)
typedef struct ClientInfo
{
    int sock;
    struct sockaddr_in addr;

} ClientInfo;

int main()
{
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    pthread_t t_id;
    int res;

    MYSQL *conn;

    conn = mysql_init(NULL);

    if (conn == NULL)
    {
        fprintf(stderr, "DB 연결 시작 실패\n");
        return 1;
    }

    if (!mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0))
    {
        fprintf(stderr, "DB 연결 실패: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    printf("DB 연결 성공\n");

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) error_handling("socket() 에러");

    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) error_handling("bind() 에러");

    if (listen(serv_sock, 5) == -1) error_handling("listen() 에러");

    printf("멀티스레드 서버 대기 중 (PORT %d)\n\n", PORT);

    while (1)
    {
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

        if (clnt_sock == -1)
        {
            perror("accept() 에러");
            continue;
        }

        printf("클라이언트 연결: %s\n", inet_ntoa(clnt_addr.sin_addr));

        // 클라이언트 정보 전달
        ClientInfo* pclient = malloc(sizeof(ClientInfo));
        if (pclient == NULL)
        {
            perror("malloc() 실패");
            close(clnt_sock);
            continue;
        }
        pclient->sock = clnt_sock;
        pclient->addr = clnt_addr;

        // 스레드 생성 
        res = pthread_create(&t_id, NULL, handleClient, (void*)pclient);
        if (res != 0)
        {
            perror("pthread_create() 실패");
            free(pclient);
            close(clnt_sock);
            continue;
        }
        pthread_detach(t_id); // 스레드 회수
    }

    close(serv_sock);
    mysql_close(conn);
    freeList();

    printf("서버 종료되면 안되는데..?\n");
    return 0;

} // main()

void* handleClient(void* arg)
{
    ClientInfo* ci = (ClientInfo*)arg; // 스레드 함수가 전달받은 인자 원래대로
    int clnt_sock = ci->sock;
    struct sockaddr_in clnt_addr = ci->addr;
    
    free(ci);

    char buffer[BUF_SIZE];
    int str_len;
    int res;

    MYSQL *conn;
    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, "localhost", "root", "poly", "iotdb", 0, NULL, 0))
    {
        fprintf(stderr, "스레드 DB 연결 실패: %s\n", mysql_error(conn));
        close(clnt_sock);
        pthread_exit(NULL);
    }

    // 클라이언트 소켓에 read 타임아웃 설정 (클라이언트 강제종료 이슈 해결)
    struct timeval timeout;
    timeout.tv_sec = 25;
    timeout.tv_usec = 0;
    setsockopt(clnt_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while (1)
    {
        memset(buffer, 0, BUF_SIZE);
        str_len = read(clnt_sock, buffer, BUF_SIZE - 1);

        if (str_len <= 0)
        {
            if (str_len < 0)
            {
                perror("read() 오류");
            }
            printf("Thread.ver // 클라이언트 연결 종료\n");
            break;
        }

        buffer[str_len] = '\0';

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
                insertLastNode(house_id, amount);

                char apt_unit[8] = {0};
                char apt_num[8] = {0};
                sscanf(house_id, "%7[^-]-%7s", apt_unit, apt_num);

                char query[512];
                sprintf(query,
                        "INSERT INTO WasteRecord(House_ID, Amount, Record_DATE, Record_TIME, IP_addr, MAC_addr) "
                        "VALUES((SELECT House_ID FROM House WHERE APT_UNIT='%s' AND APT_NUM='%s'), %.2f, CURDATE(), CURTIME(), '%s', 'AA:BB:CC:DD:EE:01')",
                        apt_unit, apt_num, amount, inet_ntoa(clnt_addr.sin_addr));

                res = mysql_query(conn, query);
                if (!res)
                {
                    printf("DB insert : %s / %.2fg\n", house_id, amount);

                    char billingQuery[512];
                    sprintf(billingQuery,
                            "INSERT INTO Billing (House_ID, Period, Total_Bill, Status) "
                            "VALUES ((SELECT House_ID FROM House WHERE APT_UNIT='%s' AND APT_NUM='%s'), "
                            "DATE_FORMAT(CURDATE(), '%%Y-%%m'), %.2f, 'UNPAID') "
                            "ON DUPLICATE KEY UPDATE Total_Bill = Total_Bill + %.2f;",
                            apt_unit, apt_num, amount, amount);

                    if (mysql_query(conn, billingQuery))
                    {
                        fprintf(stderr, "Billing 쿼리 오류: %s\n", mysql_error(conn));
                    }

                    // 251022 총량 쿼리 추가 > ESP 전송까지
                    // Cur_Month_Total 누적
                    char updateHouseQuery[512];
                    
                    sprintf(updateHouseQuery,
                        "UPDATE House SET Cur_Month_Total = Cur_Month_Total + %.2f, "
                        "Last_Update_Date = CURDATE() "
                        "WHERE APT_UNIT='%s' AND APT_NUM='%s';",
                        amount, apt_unit, apt_num);
                    
                    if (mysql_query(conn, updateHouseQuery))
                    {
                        fprintf(stderr, "House UPDATE 오류: %s\n", mysql_error(conn));
                    }

                    // 월 변경 시 자동 초기화
                    char resetQuery[512];
                    
                    sprintf(resetQuery,
                        "UPDATE House SET Cur_Month_Total = 0.0 "
                        "WHERE MONTH(CURDATE()) <> MONTH(Last_Update_Date);");
                    
                    if (mysql_query(conn, resetQuery))
                    {
                        fprintf(stderr, "House RESET 오류: %s\n", mysql_error(conn));
                    }

                    // ESP로 이번달 총량 전송
                    char selectQuery[256];
                    
                    sprintf(selectQuery,
                        "SELECT Cur_Month_Total FROM House WHERE APT_UNIT='%s' AND APT_NUM='%s';",
                        apt_unit, apt_num);
                    
                    if (!mysql_query(conn, selectQuery))
                    {
                        MYSQL_RES* resTotal = mysql_store_result(conn);
                        MYSQL_ROW row = mysql_fetch_row(resTotal);
                        if (row && row[0])
                        {
                            send(clnt_sock, row[0], strlen(row[0]), 0);
                            printf("총량 %.2f g → ESP 전송 완료\n", atof(row[0]));
                        }
                        mysql_free_result(resTotal);
                    }
                }
                else
                {
                    fprintf(stderr, "DB 저장 오류: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
                }
            }
        }
    }

    close(clnt_sock);
    mysql_close(conn);
    pthread_exit(NULL);

} // handle_client()

// 연결리스트
void insertLastNode(char* houseId, float amount)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->house_id, houseId);
    newNode->amount = amount;
    newNode->link = NULL;

    if (head == NULL)
    {
        head = newNode;
    }
    else
    {
        Node* temp = head;
        while (temp->link != NULL)
        {
            temp = temp->link;
        }
        temp->link = newNode;
    }

} // insertLastNode()

void printList()
{
    Node* temp = head;
    printf("\nprintList() 실행\n");

    while (temp != NULL)
    {
        printf("House_ID : %s, Amount : %.2f g\n", temp->house_id, temp->amount);
        temp = temp->link;
    }

} // printList()

void freeList()
{
    Node* temp;
    while (head != NULL)
    {
        temp = head;
        head = head->link;
        free(temp);
    }
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}