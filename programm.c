#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH 49999
#define BUFFER 8192

struct car{
    int range, count;
    struct car *right, *left;
};

struct station{
    int key, range, index;
    struct car *parking;
    struct station *sib;
    struct station *next, *prev;
};

struct buffer{
    struct station **arr;
    int load;
};

struct queue{
    int key, range, step;
    struct queue *father, *next;
};

struct station **table;
struct buffer buffer;
struct station *tail;

struct queue *head;
struct queue *bottom;

void aggiungi_stazione();
struct station* allocate(int);
int hash(int);
void in_buffer(struct station *);
void insert_heapfy(int);
void swap_elem(int, int);
void clear_buffer();
struct station* remove_max();
void delete_heapfy(int);
void add_car(struct station*, int);

void demolisci_stazione();
struct station* find_previous(int);
void free_car(struct car*);
void remove_pos(struct station *);

void aggiungi_auto();
struct station* find_station(int);

void rottama_auto();
void remove_node(struct station*, struct car*, struct car*);
int max_range(struct station*);

void pianifica_percorso();
void left_to_right(struct station*, struct station*);
void right_to_left(struct station*, struct station*);
void print_from_bottom(struct queue*);
void free_queue(struct queue*);

void set_command(){
    while(getchar() != '\n');
}

// void printf_buffer(){
//     printf("BUFFER:\n");
//     for(int i = 0; i < buffer.load; i++)
//         printf("  -key: %d\trange: %d\tindex: %d\n", buffer.arr[i]->key, buffer.arr[i]->range, buffer.arr[i]->index);
//     printf("\n");
// }

// void print_list(){
//     printf("LIST:\n");
//     for(struct station *tmp = tail; tmp != NULL; tmp = tmp->prev)
//         printf("  -key: %d\trange: %d\n", tmp->key, tmp->range);
//     printf("\n");
// }

int main(){
    table = (struct station **) calloc(HASH, sizeof(struct station *));

    buffer.load = 0;
    buffer.arr = (struct station **) calloc(BUFFER, sizeof(struct station *));

    tail = NULL;
    head = NULL;
    bottom = NULL;

    char command[20];
    while(fscanf(stdin, "%s", command) != EOF){
        switch(command[12]){
            case 'z': aggiungi_stazione();
                break;
            case 'a': demolisci_stazione();
                break;
            case 'o': aggiungi_auto();
                break;
            case '\0': rottama_auto();
                break;
            case 'r': pianifica_percorso();
                //print_buffer();
                //print_list();
                break;
        }
        set_command();
    }

    return 0;
}

void aggiungi_stazione(){
    int key;
    if(fscanf(stdin, "%d", &key) == EOF)
        return;
    
    struct station *ptr = allocate(key);
    if(ptr == NULL){
        printf("non aggiunta\n");
        return;
    }

    in_buffer(ptr);

    int cars, range;
    if(fscanf(stdin, "%d", &cars) == EOF)
        return;

    for(int i = 0; i < cars;  i++){
        if(fscanf(stdin, "%d", &range) == EOF)
            return;

        add_car(ptr, range);
        if(range > ptr->range)
            ptr->range = range;
    }

    printf("aggiunta\n");
}

struct station* allocate (int key){
    struct station *elem = table[hash(key)];

    if(elem == NULL){
        elem = (struct station*) calloc(1, sizeof(struct station));
        elem->key = key;
        table[hash(key)] = elem;
        return elem;
    }

    struct station *prev = NULL;
    while(elem != NULL){
        if(elem->key == key)
            return NULL;

        prev = elem;
        elem = elem->sib;
    }

    prev->sib = (struct station*) calloc(1, sizeof(struct station));
    prev->sib->key = key;
    return prev->sib;
}

int hash(int key){
    return key % HASH; 
}

void in_buffer(struct station *ptr){
    buffer.arr[buffer.load] = ptr;
    ptr->index = buffer.load;

    insert_heapfy(buffer.load);
    buffer.load++;

    if(buffer.load == BUFFER)
        clear_buffer();
}

void insert_heapfy(int index){
    if(index == 0)
        return;

    int father = (index - 1) / 2;

    if(buffer.arr[father]->key < buffer.arr[index]->key){
        swap_elem(father, index);
        insert_heapfy(father);
    }
}

void swap_elem(int i, int j){
    buffer.arr[i]->index = j;
    buffer.arr[j]->index = i;

    struct station *swap = buffer.arr[i];
    buffer.arr[i] = buffer.arr[j];
    buffer.arr[j] = swap;
}

void clear_buffer(){
    if(buffer.load <= 0)
        return;

    struct station *list = tail;
    while(buffer.load > 0){
        struct station *elem = remove_max();
        if(elem == NULL)
            //something wrong
            continue;
        elem->index = -1;
        if(list == NULL){
            list = elem;
            tail = elem;
            continue;
        }

        while(list != NULL && list->prev != NULL){
            if(list->key < elem->key)
                break;
            
            list = list->prev;
        }

        if(list->key < elem->key){
            if(list == tail){
                list->next = elem;
                elem->prev = list;
                tail = elem;
                list = elem;
                continue;
            }
            elem->next = list->next;
            elem->prev = list;

            elem->next->prev = elem;
            elem->prev->next = elem;
            list = elem;
            continue;
        }

        list->prev = elem;
        elem->next = list;
    }
}

struct station* remove_max(){
    struct station *max = buffer.arr[0];
    buffer.load--;
    swap_elem(0, buffer.load);
    
    delete_heapfy(0);
    return max;
}

void delete_heapfy(int index){
    struct station *left, *right;
    struct station *max = buffer.arr[index];

    if(index * 2 + 1 < buffer.load)
        left = buffer.arr[index * 2 + 1];
    else left = NULL;

    if(index * 2 + 2 < buffer.load)
        right = buffer.arr[index * 2 + 1];
    else right = NULL;

    if(left != NULL && left->key > max->key)
        max = left;
    if(right != NULL && right->key > max->key)
        max = right;

    if(max == buffer.arr[index])
        return;
    
    int next = max->index;
    swap_elem(max->index, index);
    delete_heapfy(next);
}

void add_car(struct station *ptr, int range){
    if(ptr->parking == NULL){
        ptr->parking = (struct car*) calloc(1, sizeof(struct car));
        ptr->parking->range = range;
        ptr->parking->count = 1;
        return;
    }

    struct car *car = ptr->parking;
    while(car != NULL){
        if(car->range == range){
            car->count++;
            return;
        }

        if(car->range < range){
            if(car->right == NULL){
                car->right = (struct car*) calloc(1, sizeof(struct car));
                car->right->range = range;
                car->count = 1;
                return;
            }
            car = car->right;
            continue;
        }

        if(car->range > range){
            if(car->left == NULL){
                car->left = (struct car*) calloc(1, sizeof(struct car));
                car->left->range = range;
                car->left->count = 1;
                return;
            }
            car = car->left;
            continue;
        }
    }

    printf("i didn't add the car, i don't know why");
}

void demolisci_stazione() {
    int key;
    if (fscanf(stdin, "%d", &key) == EOF)
        return;
    
    struct station *ptr = table[hash(key)];
    if (ptr == NULL) {
        printf("non demolita\n");
        return;
    }

    if(ptr->key == key)
        table[hash(key)] = ptr->sib;
    else{
        struct station *prev = find_previous(key);
        if(prev == NULL){
            printf("non demolita\n");
            return;
        }
        ptr = prev->sib;
        prev->sib = ptr->sib;
    }

    struct car *car = ptr->parking;
    if (car != NULL)
        free_car(car);

    if (ptr->index == -1) {
        if (tail == ptr) {
            tail = ptr->prev;
            tail->next = NULL;
        } else {
            ptr->next->prev = ptr->prev;
            if (ptr->prev != NULL)
                ptr->prev->next = ptr->next;
        }
    } else {
        remove_pos(ptr);
    }

    free(ptr);
    printf("demolita\n");
}

struct station* find_previous(int key) {
    int index = hash(key);
    struct station *elem = table[index];
    struct station *prev = NULL;

    while (elem != NULL) {
        if (elem->key == key)
            return prev;

        prev = elem;
        elem = elem->sib;
    }

    return NULL;
}

void free_car(struct car *car){
    if(car == NULL)
        return;
    
    free_car(car->left);
    free_car(car->right);

    free(car);
}

void remove_pos(struct station *elem){
    buffer.load--;
    if(elem->index == buffer.load)
        return;
    int index = elem->index;
    swap_elem(index, buffer.load);
    delete_heapfy(index);
}

void aggiungi_auto(){
    int key, range;
    if(fscanf(stdin, "%d %d", &key, &range) == EOF)
        return;
    
    struct station *ptr = find_station(key);
    if(ptr == NULL){
        printf("non aggiunta\n");
        return;
    }

    add_car(ptr, range);
    printf("aggiunta\n");
}

struct station* find_station(int key) {
    int index = hash(key);
    struct station *elem = table[index];

    while (elem != NULL) {
        if (elem->key == key)
            return elem;

        elem = elem->sib;
    }

    return NULL;
}

void rottama_auto(){
    int key, range;
    if(fscanf(stdin, "%d %d", &key, &range) == EOF)
        return;
    
    struct station *ptr = find_station(key);
    if(ptr == NULL){
        printf("non rottamata\n");
        return;
    }

    struct car *car = ptr->parking;
    if(car == NULL){
        printf("non rottamata\n");
        return;
    }

    struct car *father = NULL;
    while(car != NULL){
        if(car->range == range){
            car->count--;
            if(car->count == 0){
                remove_node(ptr, father, car);
                if(ptr->range == range)
                    ptr->range = max_range(ptr);
            }
            printf("rottamata\n");
            return;
        }

        father = car;
        car = (car->range < range) ? car->right : car->left;
    }

    printf("non rottamata\n");
}

void remove_node(struct station *ptr, struct car *father, struct car *car) {
    if (ptr == NULL || car == NULL)
        return;

    if (car->left == NULL || car->right == NULL) {
        struct car *child = (car->left != NULL) ? car->left : car->right;

        if (father == NULL)
            ptr->parking = child;
        else {
            if (car == father->left)
                father->left = child;
            else
                father->right = child;
        }

        free(car);
        return;
    }

    struct car *successor = car->right;
    struct car *successor_parent = car;

    while (successor->left != NULL) {
        successor_parent = successor;
        successor = successor->left;
    }

    car->range = successor->range;
    car->count = successor->count;

    if (successor_parent == car)
        car->right = successor->right;
    else
        successor_parent->left = successor->right;

    free(successor);
}

int max_range(struct station *ptr){
    if(ptr->parking == NULL)
        return 0;

    struct car *car = ptr->parking;
    while(car->right != NULL)
        car = car->right;

    return car->range;
}

void pianifica_percorso(){
    int from, to;
    if(fscanf(stdin, "%d %d", &from, &to) == NULL)
        return;

    int km = to - from;
    struct station *start = find_station(from);
    struct station *end = find_station(to);

    if(start == NULL || end == NULL){
        printf("nessun percorso\n");
        return;
    }

    if(start == end || start->key >= abs(km)){
        printf("%d %d\n", start->key, end->key);
        return;
    }

    if(km > 0)
        left_to_right(start, end);
    else
        right_to_left(start, end);

    free_queue(head);
}

void left_to_right(struct station *start, struct station *end){
    head = (struct queue*) calloc(1, sizeof(struct queue));
    bottom = head;
    head->key = start->key;
    head->range = start->range;

    struct queue *pop = head;
    struct station *tmp = head;
    while(pop != NULL && tmp != NULL && tmp != end){
        if(pop->key + pop->range < tmp->key){
            pop = pop->next;
            continue;
        }

        struct queue *elem = (struct queue*) calloc(1, sizeof(struct queue));
        elem->key = tmp->key;
        elem->range = tmp->range;
        elem->father = pop;

        bottom->next = elem;
        bottom = elem;

        if(bottom->key + bottom->range >= end->key){
            print_from_bottom(bottom);
            printf("%d\n", end->key);
            return;
        }

        tmp = tmp->next;
    }
    printf("nessun percorso\n");
}

void right_to_left(struct station *start, struct station *end){
    head = (struct queue*) calloc(1, sizeof(struct queue));
    bottom = head;
    head->key = start->key;
    head->range = start->range;
    head->step = 0;

    struct queue *pop = head;
    struct station *tmp = start;
    while(pop != NULL && tmp != NULL && tmp != end){
        if(pop->key - pop->range > tmp->key){
            pop = pop->range;
            continue;
        }

        struct queue *elem = (struct queue*) calloc(1, sizeof(struct queue));
        elem->key = tmp->key;
        elem->range = tmp->range;
        elem->father = pop;
        elem->step = pop->step + 1;

        bottom->next = elem;
        bottom = elem;

        if(bottom->key - bottom->range <= end->key)
            break;
        
        tmp = tmp->prev;
    }

    if(bottom->key - bottom->range > end->range){
        printf("nessun percorso\n");
        return;
    }

    while(pop != NULL && tmp != NULL && tmp != end){
        if(pop->key - pop->range > tmp->key){
            pop = pop->next;
            continue;
        }

        if(tmp->key - tmp->range <= end->range && pop->step + 1 <= bottom->step){
            bottom->key = tmp->key;
            bottom->range = tmp->range;
            bottom->father = pop;
            bottom->step = pop->step +1;
        }

        tmp = tmp->prev;
    }

    struct queue *son = bottom;
    struct queue *father = bottom->father;
    while(father != NULL){
        for(pop = father->next; father->step == pop->step && pop != son; pop = pop->next){
            if(pop->key - pop->range <= son->key)
                son->father = pop;
        }

        son = father;
        father = father->father;
    }

    print_from_bottom(bottom);
    printf("%d\n", end->key);
}

void print_from_bottom(struct queue *ptr){
    if(ptr == NULL)
        return;

    print_from_bottom(ptr->father);
    printf("%d ", ptr->key);
}