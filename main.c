#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CAR 50
#define STATION 1000

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
    int size;
    struct car *cars;
};

//cell to use in the hash table
struct cell {
    int data;
    int max;
    struct parking parking;
};

struct table {
    int size;
    double density;
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

void prepare_buffer();

int find(int);

unsigned int hash(int);

unsigned int quadratic_prob(int, int);

void add_to_table(int, int);

void add_to_array(int, int);

void add_car_to_station(int, int);

int main() {
    table.size = STATION;
    table.density = 0.0;
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

void add_to_table(int data, int index) {
    table.cells[index].data = data;
    table.cells[index].max = 0;
    table.cells[index].parking.last = 0;
    table.cells[index].parking.size = CAR;
    table.cells[index].parking.cars = (struct car *) malloc(table.cells[index].parking.size * sizeof(struct car));

    table.density += 1.0 / table.size;
    if (table.density > DENSITY) {
        table.density = table.density / UP;
        table.size *= UP;
        table.cells = (struct cell *) realloc(table.cells, table.size * sizeof(struct cell));
        memset(table.cells + (int) (table.size / UP), -1, table.size * (UP-1) * sizeof(struct cell));
    }
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