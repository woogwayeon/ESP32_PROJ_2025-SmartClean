// 너무 비싸서 10g당 1원으로 조정 > jsp에서 계산하자

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
char *user = "MY_DBID";
char *pass = "MY_DBPW";
char *dbname = "iotdb";

int main()
{
    // 소켓통신
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

    // 이부분 없으면 테스트실행시 계속 bind() 에러남
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

    printf("서버 대기 중 (PORT %d)\n\n", PORT);

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

        // 연결한 클라이언트랑 통신
        while (1)
        {
            memset(buffer, 0, BUF_SIZE);
            str_len = read(clnt_sock, buffer, BUF_SIZE - 1);

            if (str_len == 0)
            {
                break;
            } 
            else if (str_len < 0)
            {
                perror("read() 오류");
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

                    // 데이터 연결리스트 추가
                    insertLastNode(house_id, amount);

                    // 몇동 몇호 (insert 할때 써야함)
                    char apt_unit[8] = {0};
                    char apt_num[8] = {0};
                    sscanf(house_id, "%7[^-]-%7s", apt_unit, apt_num);

                    // INSERT WasteRecord
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
                    } 
                    else
                    {
                        fprintf(stderr, "DB 저장 오류: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
                    } 
                } 
            } 
        } 

        shutdown(clnt_sock, SHUT_WR);

        char tmp[64];
        // read() > 0 데이터 남음 > 버퍼비우기 (책 175)
        while (read(clnt_sock, tmp, sizeof(tmp)) > 0){} 

        close(clnt_sock);
        printf("[%s] 연결 종료, 다음 연결 대기\n\n", inet_ntoa(clnt_addr.sin_addr));

        // printList();
    } 

    // 종료 시 메모리 해제
    freeList();

    close(serv_sock);
    mysql_close(conn);

    printf("서버 정상 종료\n");
    return 0;

} // main end

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
}

void printList()
{
    Node* temp = head;
    printf("\nprintList() 실행\n");

    while (temp != NULL)
    {
        printf("House_ID : %s, Amount : %.2f g\n", temp->house_id, temp->amount);
        temp = temp->link;
    }
}

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