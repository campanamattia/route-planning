#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATIONS 99991
#define BUFFER 32768


struct car{
    int range;
    int count;
    struct car *right, *left;
};

struct station {
    int key;
    int range;
    int index;
    struct car *cars;
    struct station *sib;
    struct station *next, *prev;
};

struct buffer{
    int load;
    struct station **arr;
};

struct queue_l{
    int key;
    int range;
    struct queue_l *from;
    struct queue_l *next;
};

struct queue_r{
    int key;
    int step;
    int range;
    struct queue_r *from;
    struct queue_r *next;
};

void aggiungi_stazione();
void demolisci_stazione();
void aggiungi_auto();
void rottama_auto();
void pianifica_percorso();

void prep_command();
struct station* allocate(int);
struct station* find_to_delete(int);
struct station* find(int);
unsigned int hash(int);
void add_to_table(int , int);
void add_to_buffer(struct station *);
struct station* remove_max();
void delete_heapfy(int);
void insert_heapfy(int);
int max_range(struct car *);
void empty_buffer();
void swap_elem(int, int);
void add_car(struct station *, int);
void free_cars(struct car*);
void remove_pos(int);
void remove_node(struct station*, struct car*, struct car*);
void printf_from_bottom_l(struct queue_l*);
void printf_from_bottom_r(struct queue_r*);
void left_to_right(struct station*, struct station*);
void right_to_left(struct station*, struct station*);
void free_queue_l(struct queue_l *);
void free_queue_r(struct queue_r*);


struct buffer buffer;
struct station *tail;
struct station **table;

struct queue_l *head_l;
struct queue_l *bottom_l;

struct queue_r *head_r;
struct queue_r *bottom_r;


int main(){
    char command[20];

    tail = NULL;

    buffer.load = 0;
    buffer.arr = (struct station **) calloc(BUFFER, sizeof(struct station *));

    table = (struct station **) calloc(STATIONS, sizeof(struct station *));

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
        prep_command();
    }

    return 0;
}

void aggiungi_stazione(){
    int key, size;
    if(fscanf(stdin, "%d", &key) == EOF)
        return;

    struct station *elem = allocate(key);

    if(elem == NULL){
        printf("non aggiunta\n");
        return;
    }

    add_to_buffer(elem);

    if(fscanf(stdin, "%d", &size) == EOF)
        return;
    
    for(int i = 0; i < size; i++){
        if(fscanf(stdin, "%d", &key) == EOF)
            return;

        add_car(elem, key);
        if(elem->range < key)
            elem->range = key;
    }

    printf("aggiunta\n");
}

struct station* allocate(int key){
    struct station *to_insert;
    int index = hash(key);
    
    for(to_insert = table[index]; to_insert != NULL && to_insert->key != key;)
        if(to_insert->sib != NULL)
            to_insert = to_insert->sib;

    if(table[index] == NULL){
        table[index] = (struct station *) calloc(1, sizeof(struct station));
        table[index]->key = key;
        return table[index];
    }

    if(to_insert->key == key)
        return NULL;

    to_insert->sib = (struct station *) calloc(1, sizeof(struct station))  ;
    to_insert = to_insert->sib;
    to_insert->key = key;
    return to_insert;
}

void add_to_buffer(struct station *station){
    station->index = buffer.load;
    buffer.arr[buffer.load] = station;

    insert_heapfy(buffer.load);
    buffer.load++;

    if(buffer.load == BUFFER)
        empty_buffer();
}

void insert_heapfy(int pos){
    if(pos == 0)
    return;
    int parent = (pos-1)/2;

    if (buffer.arr[parent]->key > buffer.arr[pos]-> key)
    return;

    swap_elem(parent, pos);
    insert_heapfy(parent);
}

void swap_elem(int i, int j){
    buffer.arr[i]->index = j;
    buffer.arr[j]->index = i;

    struct station *swap = buffer.arr[i];
    buffer.arr[i] = buffer.arr[j];
    buffer.arr[j] = swap;
}

void add_car(struct station *station, int key){
    if(station->cars == NULL){
        station->cars = (struct car*) calloc(1, sizeof(struct car));
        station->cars->range = key;
        station->cars->count = 1;
        return;
    }

    struct car *car = station->cars;
    
    while(car != NULL){
        if(car->range == key){
            car->count++;
            return;
        }

        if(car->range < key){
            if(car->right == NULL){
                car->right = (struct car*) calloc(1, sizeof(struct car));
                car->right->range = key;
                car->right->count = 1;
                return;
            }
            car  = car->right;
            continue;
        }

        if(car->range > key){
            if(car->left == NULL){
                car->left = (struct car*) calloc(1, sizeof(struct car));
                car->left->range = key;
                car->left->count = 1;
                return;
            }
            car = car->left;
            continue;
        }
    }
}

int max_range(struct car *car){
    if(car == NULL)
        return 0;
    
    while(car->right != NULL)
        car = car->right;
    
    return car->range;
}

void empty_buffer(){
    if(buffer.load <= 0)
        return;
    // for(int i = 0; i < buffer.load; i++){
    //     printf("%d\t", table[buffer.arr[i]]->key);
    // }
    // printf("\n");s

    struct station *tmp = tail;
    while(buffer.load > 0){
        struct station *to_insert = remove_max();
        if(to_insert == NULL)
            //something wrong
            continue;
        to_insert->index = -1;

        if(tail == NULL){
            tail = to_insert;
            tmp = to_insert;
            continue;
        }
        while(tmp != NULL && tmp->prev != NULL){
            if(to_insert->key > tmp->key)
                break;
            tmp = tmp->prev;
        }
        if(tmp == NULL)
            //something wrong
            return;

        if(tmp->prev == NULL && tmp->key > to_insert->key){
            tmp->prev = to_insert;
            to_insert->next = tmp;
            continue;
        }

        if(tmp == tail){
            tail = to_insert;
            to_insert->prev = tmp;
            tmp->next = to_insert;
        }

        tmp->next->prev = to_insert;
        to_insert->next = tmp->next;
        to_insert->prev = tmp;
        tmp->next = to_insert;
    }
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

void demolisci_stazione(){
    int key;
    if(fscanf(stdin, "%d", &key) == EOF)
        return;
    
    struct station *ptr = find_to_delete(key);
    
    if(ptr == NULL){
        printf("non demolita\n");
        return;
    }

    struct station *to_delete;
    if(ptr == table[hash(key)]){
        table[hash(key)] = ptr->sib;
        to_delete = ptr;
    } else {
        to_delete = ptr->sib;
        ptr->sib = to_delete->sib;
    }

    if(to_delete->cars != NULL)
        free_cars(to_delete->cars);

    if(to_delete->index == -1){
        if(to_delete == tail)
            tail = to_delete->prev;
        else
            to_delete->next->prev = to_delete->prev;
        if(to_delete->prev != NULL)
            to_delete->prev->next = to_delete->next;
    } else
        remove_pos(to_delete->index);

    free(to_delete);
    printf("demolita\n");
}

struct station* find_to_delete(int key){
    struct station *ptr = table[hash(key)];

    if(ptr == NULL)
        return NULL;
    
    if(ptr->key == key)
        return ptr;
    
    while(ptr->sib != NULL && ptr->sib->key != key)
        ptr->sib;

    if(ptr->sib == NULL && ptr->key != key)
        return NULL;
    
    return ptr;
}

struct station* find(int key){
    struct station *ptr = table[hash(key)];
    if(ptr == NULL)
        return NULL;
    
    if(ptr->key == key)
        return ptr;
    
    while(ptr != NULL && ptr->key != key)
        ptr = ptr->sib;

    return ptr;
}

void free_cars(struct car *car){
    if(car == NULL)
        return;

    free_cars(car->left);
    free_cars(car->right);

    free(car);
}

void remove_pos(int index){
    buffer.load--;
    swap_elem(index, buffer.load);

    delete_heapfy(index);
}

void aggiungi_auto(){
    int key, range;
    if(fscanf(stdin, "%d %d", &key, &range) == EOF)
        return;
    
    struct station *ptr = find(key);
    if(ptr == NULL){
        printf("non aggiunta\n");
        return;
    }

    add_car(ptr, range);
    if(range > ptr->range)
        ptr->range = range;
    printf("aggiunta\n");
    return;
}

void rottama_auto(){
    int key, range;
    if(fscanf(stdin, "%d %d", &key, &range) == EOF)
        return;
    
    struct station *ptr = find(key);
    if(ptr == NULL){
        printf("non rottamata\n");
        return;
    }

    struct car *car = ptr->cars;
    if(car == NULL){
        printf("non rottamata\n");
        return;
    }

    struct car *father = car;
    while(car != NULL){
        if(car->range == range){
            car->count--;
            if(car->count == 0)
                remove_node(ptr, father, car);
            printf("rottamata\n");
            return;
        }

        father = car;
        if(car->range > range)
            car = car->left;
        else 
            car = car->right;
    }

    printf("non rottamata\n");
}

void remove_node(struct station *ptr, struct car *father, struct car *car){
    if(father == car){
        if(car->left == NULL)
            ptr->cars = car->right;
        if(car->right == NULL)
            ptr->cars = car->left;

        if(ptr->cars == car){
            struct car *tmp = car->right;
            while(tmp->left != NULL){
                father = tmp;
                tmp = tmp->left;
            }
            car->range = tmp->range;
            car->count = tmp->count;
            father->left = tmp->right;
            car = tmp;
        }

        free(car);
        return;
    }

    struct car **side = &father->right;
    if(father->left == car)
        side = &father->left;

    if(car->left == NULL){
        *side = car->right;
        free(car);
        return;
    }

    if(car->right == NULL){
        *side = car->left;
        free(car);
        return;
    }
    
    struct car *tmp = car->right;
    while(tmp->left != NULL){
        father = tmp;
        tmp = tmp->left;
    }
    car->range = tmp->range;
    car->count = tmp->count;
    father->left = tmp->right;
    car = tmp;
    free(car);
}

void pianifica_percorso(){
    int from, to;
    if(fscanf(stdin, "%d %d", &from, &to) == EOF)
        return;
    
    int km = to - from;
    struct station *start = find(from);
    struct station *end = find(to);

    if(start == NULL || end == NULL){
        printf("nessun percorso");
        return;
    }

    if(start == end || start->range >= abs(km)){
        printf("%d %d\n", start->key, start->key);
        return;
    }

    empty_buffer();
    // struct station *tmp = tail;
    // while(tmp != NULL){
    //     printf("%d\t", tmp->key);
    //     tmp = tmp->prev;
    // }
    // printf("\n");

    if(km < 0){
        right_to_left(start, end);
        free_queue_r(head_r);
        return;
    }

    left_to_right(start, end);
    free_queue_l(head_l);
}

void left_to_right(struct station *start, struct station *end){
    head_l =(struct queue_l*) calloc(1, sizeof(struct queue_l));
    head_l->key = start->key;
    head_l->range = start->range;
    bottom_l = head_l;

    struct queue_l *pop = head_l;
    struct station *tmp = start->next;
    while(pop != NULL && tmp != NULL && pop->key != end->key){
        if(pop->range + pop->key < tmp->key){
            pop = pop->next;
            continue;
        }

        bottom_l->next = (struct queue_l*) calloc(1, sizeof(struct queue_l));
        bottom_l = bottom_l->next;
        bottom_l->key = tmp->key;
        bottom_l->range = tmp->range;
        bottom_l->from = pop;

        if(bottom_l->key + bottom_l->range >= end->key){
            printf_from_bottom_l(bottom_l);
            printf("%d\n", end->key);
            return;
        }

        tmp = tmp->next;
    }

    printf("nessun percorso\n");
}

void printf_from_bottom_l(struct queue_l *pop){
    if(pop == NULL)
        return;
    
    printf_from_bottom_l(pop->from);
    printf("%d ", pop->key);
    return;
}

void free_queue_l(struct queue_l *to_free){
    if(to_free == NULL)
        return;

    while(to_free != NULL){
        struct queue_l *tmp = to_free->next;
        free(to_free);
        to_free = tmp;
    }

    head_l = NULL;
    bottom_l = NULL;
}

void right_to_left(struct station *start, struct station *end){
    head_r = (struct queue_r*) calloc(1, sizeof(struct queue_r));
    head_r->key = start->key;
    head_r->range = start->range;
    bottom_r = head_r;

    struct queue_r *pop = head_r;
    struct station *tmp = start->prev;
    while(pop != NULL && tmp != NULL && pop->key != end->key && tmp->key != end->key){
        if(pop->key - pop->range > tmp->key){
            pop = pop->next;
            continue;
        }

        bottom_r->next = (struct queue_r*) calloc(1, sizeof(struct queue_r));
        bottom_r = bottom_r->next;
        bottom_r->key = tmp->key;
        bottom_r->range = tmp->range;
        bottom_r->from = pop;
        bottom_r->step = pop->step + 1;

        if(bottom_r->key - bottom_r->range <= end->key)
            break;
        
        tmp = tmp->prev;
    }

    if(pop == NULL || bottom_r->key - bottom_r->range > end->key || tmp == NULL){
        printf("nessun percorso\n");
        return;
    }


    while(pop != NULL && tmp != NULL && pop != bottom_r && tmp->key != end->key){
        if(pop->key - pop->range > tmp->key){
            pop = pop->next;
            continue;
        }

        if(tmp->key - tmp->range <= end->key && bottom_r->step == pop->step + 1){
            bottom_r->key = tmp->key;
            bottom_r->range = tmp->range;
            bottom_r->from = pop;
        }

        tmp = tmp->prev;
    }

    // struct queue_r *print = head_l;
    // printf("%d, %d, NULL\n", print->key, print->range);
    // print = print->next;
    // while(print != NULL){
    //     printf("%d, r: %d, f: %d\n", print->key, print->range, print->from->key);
    //     print = print->next;
    // }
    // printf("\n");

    struct queue_r *father = bottom_r->from;
    struct queue_r *son = bottom_r;
    while(father != NULL){
        for(pop = father->next; father->step == pop->step && pop != son; pop = pop->next){
            if(pop->key - pop->range <= son->key)
                son->from = pop;
        }

        son = son->from;
        father = father->from;
    }

    // print = head_r;
    // printf("%d, %d, NULL\n", print->key, print->range);
    // print = print->next;
    // while(print != NULL){
    //     printf("%d, r: %d, f: %d\n", print->key, print->range, print->from->key);
    //     print = print->next;
    // }
    // printf("\n");

    printf_from_bottom_r(bottom_r);
    printf("%d\n", end->key);
}

void printf_from_bottom_r(struct queue_r *pop){
    if(pop == NULL)
        return;
    
    printf_from_bottom_r(pop->from);
    printf("%d ", pop->key);
}

void free_queue_r(struct queue_r *to_free){
    if(to_free == NULL)
        return;
    
    while(to_free != NULL){
        struct queue_r *tmp = to_free->next;
        free(to_free);
        to_free = tmp;
    }

    head_r = NULL;
    bottom_r = NULL;
}

unsigned int hash(int key){
    const unsigned int seed = 42;
    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    unsigned int h = seed ^ sizeof(key);
    key *= m;
    key ^= key >> r;
    key *= m;
    h*= m;
    h ^= key;

    return h % STATIONS;
}

void prep_command(){
    while (getchar() != '\n');
}