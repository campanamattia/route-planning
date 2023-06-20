#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct car {
    int data;
    int count;
    struct car *right;
    struct car *left;
};

struct node {
    int data;
    struct car *cars;
    struct node *right;
    struct node *left;
};

struct node *root = NULL;

struct hash{
    int data;
    struct node *node;
};

struct hash *hashTable[100];

void prepare_buffer();

int main(){
    char com[20];
    while(fscanf(stdin, "%s", com) != EOF){
        switch(com[0]){
            case 'a':
                switch (com[9]) {
                    case 'a':
                        insert_car();
                        break;
                    default:
                        insert_station();
                        break;
                }
                break;
            case 'r':
                remove_car();
                break;
            case 'p':
                plan_route();
                break;
            case 'd':
                remove_station();
                break;
            default:
                break;
        }
        printf("\n");
        prepare_buffer();
    }
    return 0;
}

void prepare_buffer(){
    while(getchar() != '\n');
}
