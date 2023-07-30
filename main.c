#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOMBSTONE ((void *)-1)
#define STATIONS 99991
#define BUFFER 32768
#define F1 113
#define F2 127
#define DENSITY 0.75
#define UP 1.75

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
    struct station *next, *prev;
};

struct buffer{
    unsigned int *arr;
    int load;
};

struct mastro{
    int size;
    int load;
    struct station **table;
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
int allocate(int);
int find(int);
unsigned int hash(int);
unsigned int search_alg(int, int);
void realloc_table();
void add_to_table(int , int);
void add_to_buffer(int);
int remove_max();
void delete_heapfy(int);
void insert_heapfy(int);
int max_range(struct car *);
void empty_buffer();
void swap_elem(int, int);
void add_car(int, int);
int new_pos(struct station **, int);
void free_cars(struct car*);
void remove_pos(int);
void remove_node(int, struct car*, struct car*);
void printf_from_bottom_l(struct queue_l*);
void printf_from_bottom_r(struct queue_r*);
void left_to_right(struct station*, struct station*);
void right_to_left(struct station*, struct station*);
void free_queue_l(struct queue_l *);
void free_queue_r(struct queue_r*);


struct buffer buffer;
struct mastro mastro;
struct station *tail;

struct queue_l *head_l;
struct queue_l *bottom_l;

struct queue_r *head_r;
struct queue_r *bottom_r;


int main(){
    char command[20];

    tail = NULL;

    buffer.load = 0;
    buffer.arr = (unsigned int *) calloc(BUFFER, sizeof(unsigned int));

    mastro.size = STATIONS;
    mastro.load = 0;
    mastro.table = (struct station **) calloc(STATIONS, sizeof(struct station *));

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
    int key, index, size;
    if(fscanf(stdin, "%d", &key) == EOF)
        return;

    index = allocate(key);

    if(mastro.table[index] != NULL && mastro.table[index] != TOMBSTONE){
        printf("non aggiunta\n");
        return;
    }

    add_to_table(index, key);
    add_to_buffer(index);

    if(fscanf(stdin, "%d", &size) == EOF)
        return;
    
    for(int i = 0; i < size; i++){
        if(fscanf(stdin, "%d", &key) == EOF)
            return;

        add_car(index, key);
        if(mastro.table[index]->range < key)
            mastro.table[index]->range = key;
    }

    printf("aggiunta\n");
}

int allocate(int key){
    int index = hash(key);
    if(mastro.table[index] == NULL || (mastro.table[index] != TOMBSTONE && mastro.table[index]->key == key))
        return index;
    
    int tmb = -1;
    if(mastro.table[index] == TOMBSTONE)
        tmb = index;
    
    index = search_alg(index, 1);
    for(int attempt = 2; mastro.table[index] != NULL; attempt++){
        if(mastro.table[index] == TOMBSTONE){
            if(tmb == -1)
                tmb = index;
            continue;
        }

        if(mastro.table[index]->key == key)
            break;
    }
    
    if(mastro.table[index] == NULL && tmb >= 0 && mastro.table[tmb] == TOMBSTONE)
        return tmb;
    
    return index;
}

void add_to_table(int index, int key){
    mastro.table[index] = (struct station*) calloc(1, sizeof(struct station));
    mastro.table[index]->key = key;

    mastro.load++;
    double density = mastro.load / (double) mastro.size;
    if(density >= DENSITY)
        realloc_table();
}

void realloc_table(){
    mastro.size *= UP;
    struct station **new_table = (struct station**) calloc(mastro.size, sizeof(struct station*));

    if(tail == NULL && buffer.load <= 0)
        //something wrong
        return;
    
    if(tail != NULL){
        struct station *tmp;
        for(tmp = tail; tmp != NULL; tmp = tmp->prev){
            int new_index = new_pos(new_table, tmp->key);
            new_table[new_index] = tmp;
        }
    }

    if(buffer.load > 0){
        for(int i = 0; i < buffer.load; i++){
            if(mastro.table[buffer.arr[i]] == NULL || mastro.table[buffer.arr[i]] == TOMBSTONE)
                continue;
            int new_index = new_pos(new_table, mastro.table[buffer.arr[i]]->key);
            new_table[new_index] = mastro.table[buffer.arr[i]];
        }
    }

    free(mastro.table);
    mastro.table = new_table;
}

int new_pos(struct station **table, int key){
    int index = hash(key);
    if(table[index] == NULL)
        return index;
    
    index = search_alg(index, 1);
    for(int attempt = 2; table[index] != NULL; attempt++)
        index = search_alg(index, attempt);
    
    return index;
}

void add_to_buffer(int index){
    mastro.table[index]->index = buffer.load;
    buffer.arr[buffer.load] = index;

    insert_heapfy(buffer.load);
    buffer.load++;

    if(buffer.load == BUFFER)
        empty_buffer();
}

void insert_heapfy(int pos){
    if(pos == 0)
    return;
    int parent = (pos-1)/2;

    if (mastro.table[buffer.arr[parent]]->key > mastro.table[buffer.arr[pos]]-> key)
    return;

    swap_elem(parent, pos);
    insert_heapfy(parent);
}

void swap_elem(int i, int j){
    mastro.table[buffer.arr[i]]->index = j;
    mastro.table[buffer.arr[j]]->index = i;

    int swap = buffer.arr[i];
    buffer.arr[i] = buffer.arr[j];
    buffer.arr[j] = swap;
}

void add_car(int index, int key){
    struct car *car = mastro.table[index]->cars;
    if(car == NULL){
        car = (struct car*) calloc(1, sizeof(struct car));
        car->range = key;
        car->count = 1;
        mastro.table[index]->cars = car;
        return;
    }

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
    //     printf("%d\t", mastro.table[buffer.arr[i]]->key);
    // }
    // printf("\n");s

    struct station *tmp = tail;
    while(buffer.load > 0){
        struct station *to_insert = mastro.table[remove_max()];
        if(to_insert == NULL || to_insert == TOMBSTONE)
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

int remove_max(){
    int max = buffer.arr[0];

    buffer.load--;
    swap_elem(0, buffer.load);
    delete_heapfy(0);

    return max;
}

void delete_heapfy(int pos){
    struct station *left;
    struct station *right;
    struct station *max = mastro.table[buffer.arr[pos]];

    if(pos * 2 + 1 < buffer.load)
        left = mastro.table[buffer.arr[pos * 2 + 1]];
    else left = NULL;
    
    if(pos * 2 + 2 < buffer.load)
        right =  mastro.table[buffer.arr[pos * 2 + 2]];
    else right = NULL;

    if(left != NULL && left != TOMBSTONE && left->key > max->key)
        max = left;
    
    if(right != NULL && right != TOMBSTONE && right->key > max->key)
        max = right;
    
    if(max->key == mastro.table[buffer.arr[pos]]->key)
        return;

    int index = max->index;
    swap_elem(max->index, pos);
    delete_heapfy(index);
}

void demolisci_stazione(){
    int index;
    if(fscanf(stdin, "%d", &index) == EOF)
        return;
    
    index = find(index);
    if(index < 0){
        printf("non demolita\n");
        return;
    }

    struct station *to_delete = mastro.table[index];
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

    mastro.table[index] = TOMBSTONE;
    free(to_delete);

    printf("demolita\n");
}

int find(int key){
    int index = hash(key);
    if(mastro.table[index] == NULL)
        return -1;
    if(mastro.table[index] != TOMBSTONE && mastro.table[index]->key == key)
        return index;

    index = search_alg(index, 1);
    for(int attempt = 2; mastro.table[index] != NULL; attempt++){
        if(mastro.table[index] != TOMBSTONE && mastro.table[index]->key == key)
            break;
        
        index = search_alg(index, attempt);
    }

    if(mastro.table[index] != NULL && mastro.table[index] != TOMBSTONE && mastro.table[index]->key == key)
        return index;
    
    return -1;
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
    int index, range;
    if(fscanf(stdin, "%d %d", &index, &range) == EOF)
        return;
    
    index = find(index);
    if(index < 0){
        printf("non aggiunta\n");
        return;
    }

    add_car(index, range);
    if(range > mastro.table[index]->range)
        mastro.table[index]->range = range;
    printf("aggiunta\n");
    return;
}

void rottama_auto(){
    int index, range;
    if(fscanf(stdin, "%d %d", &index, &range) == EOF)
        return;
    
    index = find(index);
    if(index < 0){
        printf("non rottamata\n");
        return;
    }

    struct car *car = mastro.table[index]->cars;
    if(car == NULL){
        printf("non rottamata\n");
        return;
    }

    struct car *father = car;
    while(car != NULL){
        if(car->range == range){
            car->count--;
            if(car->count == 0)
                remove_node(index, father, car);
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

void remove_node(int index, struct car *father, struct car *car){
    if(father == car){
        if(car->left == NULL)
            mastro.table[index]->cars = car->right;
        if(car->right == NULL)
            mastro.table[index]->cars = car->left;

        if(mastro.table[index]->cars == car){
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
    int start, end;
    if(fscanf(stdin, "%d %d", &start, &end) == EOF)
        return;
    
    int km = end - start;
    start = find(start);
    end = find(end);

    if(start < 0 || end < 0){
        printf("nessun percorso");
        return;
    }

    if(start == end || mastro.table[start]->range >= abs(km)){
        printf("%d %d\n", mastro.table[start]->key, mastro.table[end]->key);
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
        right_to_left(mastro.table[start], mastro.table[end]);
        free_queue_r(head_r);
        return;
    }

    left_to_right(mastro.table[start], mastro.table[end]);
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

    return h % mastro.size;
}

unsigned int search_alg (int index, int attempt){
    return (index + attempt * F1 + attempt * attempt * F2) % mastro.size;
}

void prep_command(){
    while (getchar() != '\n');
}