#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <mysql/mysql.h>

char * host = "localhost";
char * user = "root";
char * pass = "poly";
char * dbname = "iot";

char device[] = "/dev/ttyACM0";
int fd;
int str_len;
int baud = 9600;

int main()
{
        MYSQL * conn;
        int res; // DB
        char in_sql[200] = {0}; //DB
        char *pArray[2] = {0}; //humi, temp
        char ser_buff[100] = {0};
        int buffindex = 0; //serial index
        char *pToken;     // humi:temp
        int temp, humi;
        int index, sql_index;
        int flag = 0;

        conn = mysql_init(NULL);
        if((fd = serialOpen(device, baud)) < 0 )
                exit(1);
        if(wiringPiSetup() == -1)
        return -1;
        if(!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
        {
                fprintf(stderr,"ERROR:%s[%d]\n",mysql_error(conn),mysql_errno(conn));
                exit(1);
        }
        else
                printf("Connection Successful!\n\n");
        while(1)
        {
                if(serialDataAvail(fd))
                {
                        ser_buff[buffindex++] = serialGetchar(fd);
                        if(ser_buff[buffindex-1] == 'L')
                        {
				flag = 1;
                                ser_buff[buffindex-1] = '\0';
                                str_len = strlen(ser_buff);
                                printf("ser_buff = %s\n", ser_buff);
                                pToken = strtok(ser_buff, ":");
                                int i = 0;
                                while(pToken != NULL)
                                {
                                        pArray[i] = pToken;
                                        if(++i > 3)
                                          break;
                                        pToken = strtok(NULL, " ");
                                }
                                temp = atoi(pArray[0]);
                                humi = atoi(pArray[1]);
                                printf("temp = %d, humi = %d\n", temp, humi);
                                for(int i = 0; i <=str_len; i++)
                                        ser_buff[i] = 0;
                                buffindex = 0;
			 }
                        if(temp < 100 && humi < 100)
                        {
                                if(flag == 1)
                                {
				sprintf(in_sql, "insert into sensing(id, date, time, humi,temp) values(null,curdate(),curtime(),%d,%d)",humi,temp);

                                res = mysql_query(conn, in_sql);
                                printf("res = %d\n", res);
                        if(!res)
                              printf("inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn));

                        else{
                               fprintf(stderr, "ERROR: %s[%d]\n",mysql_error(conn),mysql_errno(conn));
                                exit(1);
                        }
                }
        flag = 0;
        }
     }
  }
 mysql_close(conn);
 return EXIT_SUCCESS;
}
