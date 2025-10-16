// strtok() 사용

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    char house_id[15];  // 동호수
    float amount;       // 음쓰

    struct Node* link;
} Node;

Node* head = NULL; 

void insertFirstNode(char* houseId, float amount)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->house_id, houseId);
    newNode->amount = amount;

    newNode->link = head; 
    head = newNode; 
}

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

int main()
{
    printf("단일연결리스트 데이터넣기\n");

    char buf[30];
    char* token;
    char houseId[14];
    float amount = 0.0;

    char dustData[3][20] = { "101-902@132.52", "101-903@88.12", "102-401@245.70" };

    int count = sizeof(dustData) / sizeof(dustData[0]);

    for (int i = 0; i < count; i++)
    {
        // strtok() 원본 망가짐
        strcpy(buf, dustData[i]); 

        token = strtok(buf, "@"); // 101-902
        
        if (token != NULL)
        {
            strcpy(houseId, token);
            token = strtok(NULL, "@"); // 두 번째 토큰: amount
            
            if (token != NULL)
            {
                amount = atof(token);
                insertLastNode(houseId, amount);
            }
        }
    }
    
    printList();

    freeList();

    return 0;
}