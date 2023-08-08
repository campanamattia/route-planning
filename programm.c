#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH 79999
#define BUFFER 131072
// #define BREAK 11469 //88

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
struct helper{
    int key, range, step;
    struct helper *father, *next;
};

struct station **table;
struct buffer buffer;
struct station *tail;

struct helper *head;
struct helper *bottom;

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
void remove_from_list(struct station*);
void remove_pos(struct station *);

void aggiungi_auto();
struct station* find_station(int);

void rottama_auto();
void remove_node(struct station*, struct car*, struct car*);
int max_range(struct station*);

void pianifica_percorso();
void left_to_right(struct station*, struct station*);
void print_from_bottom(struct helper*);
void right_to_left(struct station*, struct station*);
void print_from_head(struct helper*);
void free_helper();

void set_command(){
    while(getchar() != '\n');
}

// void print_buffer(){
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

// void print_queue(){
//     printf("QUEUE:\n");
//     for(struct helper *tmp = head; tmp != NULL; tmp = tmp->next)
//         printf("  -key: %d\trange: %d\tfather: %d\tstep: %d\n", tmp->key, tmp->range, ((tmp->father != NULL) ? tmp->father->key : 0), tmp->step);
//     printf("\n");
// }

// int print = 1;
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
                break;
        }
        // print++;
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
    int index = hash(key);
    struct station *elem = table[index];

    if(elem == NULL){
        elem = (struct station*) calloc(1, sizeof(struct station));
        elem->key = key;
        table[index] = elem;
        return elem;
    }

    struct station *prev = NULL;
    while(elem != NULL && !(elem->key < key)){
        if(elem->key == key)
            return NULL;

        prev = elem;
        elem = elem->sib;
    }

    if(prev == NULL){
        table[index] = (struct station*) calloc(1, sizeof(struct station));
        table[index]->sib = elem;
        elem = table[index];
    } else {
        elem = (struct station*) calloc(1, sizeof(struct station));
        elem->sib = prev->sib;
        prev->sib = elem;
    }

    elem->key = key;
    return elem;
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

    // print_buffer();
    struct station *list = tail;
    while(buffer.load > 0){
        struct station *elem = remove_max();
        if(elem == NULL)
            //something wrong
            continue;
        elem->index = -1;

        if(tail == NULL){
            tail = elem;
            list = elem;
            continue;
        }

        struct station *next = list->next;
        while(list != NULL && list->key > elem->key){
            next = list;
            list = list->prev;
        }

        if(list == tail && list->key < elem->key){
            list->next = elem;
            elem->prev = list;
            tail = elem;
            list = elem;
            continue;
        }

        if(list != NULL &&list->key < elem->key){
            list->next = elem;
            next->prev = elem;

            elem->next = next;
            elem->prev = list;
            list = elem;
            continue;
        }

        if(list == NULL && next != NULL && next->key > elem->key){
            next->prev = elem;
            elem->next = next;
            list = elem;
            continue;
        }

        //something wrong
        printf("something wrong");
    }

    // print_list();
}

struct station* remove_max(){
    struct station  *max = buffer.arr[0];

    buffer.load--;
    swap_elem(0, buffer.load);

    delete_heapfy(0);
    return max;
}

void delete_heapfy(int pos){
    struct station *left;
    struct station *right;
    struct station *max = buffer.arr[pos];

    if(pos * 2 + 1 < buffer.load)
        left = buffer.arr[pos * 2 + 1];
    else left = NULL;
    
    if(pos * 2 + 2 < buffer.load)
        right =  buffer.arr[pos * 2 + 2];
    else right = NULL;

    if(left != NULL && left->key > max->key)
        max = left;
    
    if(right != NULL && right->key > max->key)
        max = right;
    
    if(max->key == buffer.arr[pos]->key)
        return;

    int index = max->index;
    swap_elem(max->index, pos);
    delete_heapfy(index);
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
                car->right->count = 1;
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

    if(ptr->index == -1)
        remove_from_list(ptr);
    else
        remove_pos(ptr);

    free(ptr);
    printf("demolita\n");
}

struct station* find_previous(int key) {
    int index = hash(key);
    struct station *elem = table[index];
    struct station *prev = NULL;

    while (elem != NULL && !(elem->key < key)) {
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

void remove_from_list(struct station *ptr){
    if(ptr == tail){
        if(ptr->prev != NULL)
            ptr->prev->next = NULL;
        tail = ptr->prev;
        return;
    }

    if(ptr->next != NULL && ptr->prev != NULL){
        struct station *next = ptr->next;
        struct station *prev = ptr->prev;

        prev->next = ptr->next;
        next->prev = ptr->prev;
        return;
    }

    if(ptr->prev == NULL){
        struct station *next = ptr->next;
        next->prev = NULL;
        return;
    }
}

void remove_pos(struct station *elem){
    buffer.load--;
    if(elem->index == buffer.load)
        return;
    int index = elem->index;
    swap_elem(index, buffer.load);
    insert_heapfy(index);
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

    if(range > ptr->range)
        ptr->range = range;

    printf("aggiunta\n");
}

struct station* find_station(int key) {
    int index = hash(key);
    struct station *elem = table[index];

    while (elem != NULL && !(elem->key < key)) {
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
            if(car->count == 0)
                remove_node(ptr, father, car);
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

    int range = car->range;

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
        if(range == ptr->range)
            ptr->range = max_range(ptr);
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
    
    if(range == ptr->range)
        ptr->range = max_range(ptr);
}

int max_range(struct station *ptr){
    if(ptr == NULL || ptr->parking == NULL)
        return 0;
    
    struct car *car = ptr->parking;
    while(car->right != NULL)
        car = car->right;
    
    return car->range;
}

void pianifica_percorso(){
    int from, to;
    if(fscanf(stdin, "%d %d", &from, &to) == EOF)
        return;

    int km = to - from;
    struct station *start = find_station(from);
    struct station *end = find_station(to);

    if(start == NULL || end == NULL){
        printf("nessun percorso\n");
        return;
    }

    if(start == end || start->range >= abs(km)){
        printf("%d %d\n", start->key, end->key);
        return;
    }

    clear_buffer();

    if(km > 0)
        left_to_right(start, end);
    else
        right_to_left(start, end);

    // if(print == BREAK)
    //     print_queue();
    free_helper();
}

void left_to_right(struct station *start, struct station *end){
    head = (struct helper*) calloc(1, sizeof(struct helper));
    bottom = head;
    head->key = start->key;
    head->range = start->range;

    struct helper *pop = head;
    struct station *tmp = start->next;
    while(pop != NULL && tmp != NULL && tmp != end){
        if(pop->key + pop->range < tmp->key){
            pop = pop->next;
            continue;
        }

        struct helper *elem = (struct helper*) calloc(1, sizeof(struct helper));
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

    // print_from_bottom(bottom);
    printf("nessun percorso\n");
}

void print_from_bottom(struct helper *ptr){
    if(ptr == NULL)
        return;

    print_from_bottom(ptr->father);
    printf("%d ", ptr->key);
}

void right_to_left(struct station *start, struct station *end){
    // print_list();
    head = (struct helper*) calloc(1, sizeof(struct helper));
    bottom = head;
    head->key = start->key;
    head->range = start->range;
    head->step = 0;

    struct helper *pop = head;
    struct helper *stop = head;
    struct station *tmp = start;
    while(pop != NULL && tmp != NULL && tmp->key != end->key){
        if(pop->key - pop->range > tmp->key){
            if(pop->next != NULL && pop->next->step == pop->step){
                pop = pop->next;
                continue;
            }
            else if(head != stop){
                stop = head;
                pop = head;
                continue;
            } else 
                break;
        }

        struct helper *elem = (struct helper*) calloc(1, sizeof(struct helper));
        elem->key = tmp->key;
        elem->range = tmp->range;
        elem->father = pop;
        elem->step = pop->step + 1;

        elem->next = head;
        head = elem;

        if(head->key - head->range <= end->key)
            break;
        
        tmp = tmp->prev;
    }

    if(head->key - head->range > end->key){
        // print_from_bottom(bottom);
        printf("nessun percorso\n");
        return;
    }

    stop = pop->father;
    while(pop != NULL && tmp != NULL && tmp->key != end->key){
        if(pop->key - pop->range > tmp->key){
            if(pop->step + 1 == stop->step)
                break;
            pop = pop->next;
            continue;
        }

        if(tmp->key - tmp->range <= end->key){
            head->key = tmp->key;
            head->father = pop;
        }

        tmp = tmp->prev;
    }

    print_from_head(head);
    printf("%d\n", end->key);
}

void print_from_head(struct helper *ptr){
    if(ptr == NULL)
        return;
    print_from_head(ptr->father);
    printf("%d ", ptr->key);
}

void free_helper(){
    if(head == NULL)
        return;

    while(head != NULL){
        struct helper *ptr = head->next;
        free(head);
        head = ptr;
    }

    head = NULL;
    bottom = NULL;
}