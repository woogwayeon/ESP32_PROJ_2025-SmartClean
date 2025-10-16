#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    char house_id[30];  // 동호수
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
    printf("\nprintList()\n");

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

    insertLastNode("101-902", 22.5);
    insertLastNode("101-903", 18.2);
    insertLastNode("102-401", 35.7);

    printList();

    freeList();

    return 0;
}