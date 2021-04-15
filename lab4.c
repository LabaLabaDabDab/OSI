#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int BUFSIZ = 2;

struct node {
    char *value;
    struct node* next;
};

struct node *nodeInit(){
    struct node *head = (struct node*)malloc(sizeof(struct node));
    if (head == NULL){
        exit(1);
    }
    head->next = NULL;
    head->value = NULL;
    return head;
}

void freeNode(struct node* currentNode){
    free(currentNode->value);
    free(currentNode);
    return;
}

struct node *addString(char *newLine){
    struct node *newNode = NULL;
    newNode = (struct node *)malloc(sizeof(struct node));
    if (newNode == NULL){
        exit(1);
    }
    newNode->value = (char *)malloc(strlen(newLine) + 1);
    if (newNode->value == NULL){
        exit(1);
    }
    strcpy(newNode->value, newLine);
    newNode->next = NULL;
    return(newNode);
}


int main(){
    char line[BUFSIZ];
    struct node *head = NULL,
            *currentNode = NULL,
            *i = NULL;

    head = nodeInit();
    currentNode = head;

    while (gets(line) != NULL) {
        if (line[0] == '.')
            break;
        currentNode->next = addString(line);
        currentNode = currentNode->next;
    }

    for (i = head->next; i != NULL; i = i->next)
        puts(i->value);
    i = head->next;

    struct node *j = NULL;
    while (i != NULL) {
        j = i->next;
        freeNode(i);
        i = NULL;
        i = j;
    }
    free(head);
    return 0;
}
