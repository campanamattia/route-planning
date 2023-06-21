#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAR 500
#define STATION 10000

#define F1 113
#define F2 127

#define DENSITY 0.75
#define UP 1.75

//to save the cars info
struct car {
    int data;
    int count;
};

struct parking {
    int last;
    int table_size;
    int heap_size;
    struct car *cars_table;
    struct car *max_heap;
    //se aggiorno un valore dentro la tabella che mi indica la posizione della macchina nell'heap ho un inserimento e rimozione sempre in log(n) e non in n
    //ne vale la pena?

};

//cell to use in the hash table
struct cell {
    int data;
    int max; //nel caso di un max heap non serve
    struct parking parking;
};

struct table {
    int size;
    struct cell *cells;
};

struct table table;

struct page {
    int data;
    int index;
};

//to plan the route
struct mastro {
    int last;
    int size;
    struct page *pages;
};

struct mastro mastro;

void insert_car();

void insert_station();

void remove_car();

void remove_station();

void plan_route();

void print();

void prepare_buffer();

int find(int);

unsigned int hash(int);

unsigned int quadratic_prob(int, int);

void add_to_table(int, int);

void add_to_array(int, int);

void check_table();

void check_mastro();

void check_parking(int);

void add_car_to_station(int, int);

int binary_mastro(int, int, int);

int binary_cars(int, int, int, int);

int main() {
    table.size = STATION;
    table.cells = (struct cell *) malloc(table.size * sizeof(struct cell));
    memset(table.cells, -1, table.size * sizeof(struct cell));

    mastro.size = STATION;
    mastro.last = 0;
    mastro.pages = (struct page *) malloc(mastro.size * sizeof(struct page));

    char com[20];
    while (fscanf(stdin, "%s", com) != EOF) {
        switch (com[0]) {
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
                print();
                break;
        }
        printf("\n");
        prepare_buffer();
    }
    return 0;
}

void prepare_buffer() {
    while (getchar() != '\n');
}

void insert_station() {
    int data, count, index;
    fscanf(stdin, "%d", &data);
    index = find(data);
    if (table.cells[index].data == data) {
        printf("non aggiunta");
        return;
    }
    add_to_table(data, index);
    add_to_array(data, index);
    fscanf(stdin, "%d", &count);
    for (int i = 0; i < count; i++) {
        fscanf(stdin, "%d", &data);
        add_car_to_station(index, data);
        if (data > table.cells[index].max) {
            table.cells[index].max = data;
        }
    }
    printf("aggiunta");
}

void insert_car() {
    int data, index;
    fscanf(stdin, "%d", &data);
    index = find(data);
    if (table.cells[index].data != data) {
        printf("non aggiunta");
        return;
    }
    fscanf(stdin, "%d", &data);
    add_car_to_station(index, data);
    printf("aggiunta");
    return;
}

void remove_car() {
    int data, index;
    fscanf(stdin, "%d", &data);
    index = find(data);
    if (table.cells[index].data != data) {
        printf("non presente");
        return;
    }

}

void remove_station() {

}

void plan_route() {

}

int find(int data) {
    unsigned int index = hash(data);
    int attempt = 0;
    while (table.cells[index].data != data && table.cells[index].data != -1 && attempt < table.size) {
        attempt++;
        index = quadratic_prob(index, attempt);
    }
    return index;
}

unsigned int hash(int key) {
    const unsigned int seed = 42;
    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    unsigned int h = seed ^ sizeof(key);
    key *= m;
    key ^= key >> r;
    key *= m;
    h *= m;
    h ^= key;

    return h % table.size;
}

unsigned int quadratic_prob(int index, int attempt) {
    return (index + attempt * F1 + attempt * attempt * F2) % table.size;
}

void add_to_table(int data, int index) {
    table.cells[index].data = data;
    table.cells[index].max = 0;
    table.cells[index].parking.last = 0;
    table.cells[index].parking.size = CAR;
    table.cells[index].parking.cars = (struct car *) malloc(table.cells[index].parking.size * sizeof(struct car));

    check_table();
}

void check_table() {
    if (((double) (mastro.last + 1) / (double) table.size) > DENSITY) {
        table.cells = (struct cell *) realloc(table.cells, table.size * UP * sizeof(struct cell));
        memset(table.cells + table.size, -1, table.size * (UP - 1) * sizeof(struct cell));
        table.size *= UP;
    }
}

void add_to_array(int data, int index) {
    if (mastro.last == 0) {
        mastro.pages[mastro.last].data = data;
        mastro.pages[mastro.last].index = index;
        mastro.last++;
        check_mastro();
        return;
    }
    int pos = binary_mastro(data, 0, mastro.last);
    memcpy(mastro.pages + pos + 1, mastro.pages + pos, (mastro.last - pos) * sizeof(struct page));
    mastro.pages[pos].data = data;
    mastro.pages[pos].index = index;
    mastro.last++;
    check_mastro();
    return;
}

int binary_mastro(int data, int start, int end) {
    int low = start;
    int high = end - 1;

    while (low <= high) {
        int mid = (low + high) / 2;

        if (data < mastro.pages[mid].data) {
            high = mid - 1;
        } else if (data > mastro.pages[mid].data) {
            low = mid + 1;
        } else {
            return mid;
        }
    }

    return low;
}

void check_mastro() {
    if (mastro.last == mastro.size) {
        mastro.size *= UP;
        mastro.pages = (struct page *) realloc(mastro.pages, mastro.size * sizeof(struct page));
    }
}

void add_car_to_station(int index, int data) {
    if (table.cells[index].parking.last == 0) {
        table.cells[index].parking.cars[table.cells[index].parking.last].data = data;
        table.cells[index].parking.cars[table.cells[index].parking.last].count = 1;
        table.cells[index].parking.last++;
        check_parking(index);
        return;
    }
    int pos = binary_cars(index, data, 0, table.cells[index].parking.last);
    if (table.cells[index].parking.cars[pos].data == data) {
        table.cells[index].parking.cars[pos].count++;
        return;
    }
    memcpy(table.cells[index].parking.cars + pos + 1, table.cells[index].parking.cars + pos,
           (table.cells[index].parking.last - pos) * sizeof(struct car));
    table.cells[index].parking.cars[pos].data = data;
    table.cells[index].parking.cars[pos].count = 1;
    table.cells[index].parking.last++;
    check_parking(index);
    return;
}

int binary_cars(int index, int data, int start, int end) {
    int low = start;
    int high = end - 1;

    while (low <= high) {
        int mid = (low + high) / 2;

        if (data < table.cells[index].parking.cars[mid].data) {
            high = mid - 1;
        } else if (data > table.cells[index].parking.cars[mid].data) {
            low = mid + 1;
        } else {
            return mid;
        }
    }

    return low;
}

void check_parking(int index) {
    if (table.cells[index].parking.last == table.cells[index].parking.size) {
        table.cells[index].parking.size *= UP;
        table.cells[index].parking.cars = (struct car *) realloc(table.cells[index].parking.cars,
                                                                 table.cells[index].parking.size * sizeof(struct car));
    }
}

void print() {
    for (int i = 0; i < mastro.last; i++) {
        printf("- Data: %d\tRange: %d\tIndex: %d\n", table.cells[mastro.pages[i].index].data,
               table.cells[mastro.pages[i].index].max, mastro.pages[i].index);
        for (int j = 0; j < table.cells[mastro.pages[i].index].parking.last; j++) {
            printf("\t%d ^ %d\n", table.cells[mastro.pages[i].index].parking.cars[j].data,
                   table.cells[mastro.pages[i].index].parking.cars[j].count);
        }
    }
}