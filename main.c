#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define START_SIZE 2
#define INCREMENT 1.3
#define DECREMENT 0.77


struct car {
    int range;
    int count;
    struct car *right;
    struct car *left;
};

struct stop {
    int distance;
    int max_range;
    struct car *cars;
};

struct highway {
    int last;
    int size;
    struct stop *stops;
};

struct road {
    int lenght;
    int weight;
    int *stops;
};

//command
void add_stop();

void add_car();

void remove_car();

void remove_stop();

void elaborate();

//auxiliary
void swap (struct stop *, struct stop *);

void sort_highway();

void heapify(int, int);

struct stop *find_station(int);

struct stop new_stop(int);

struct car *new_car(int);

void add_car_to_stop(struct stop, int);

void free_cars();

void set_buffer();

void print_highway();

void print_cars(struct car *);

void to_Milan();

void to_Turin();

struct highway highway;
int start, end;
struct stop *p_start, *p_end;

int main() {
    start = -1;
    end = -1;
    p_start = NULL;
    p_end == NULL;
    highway.last = 0;
    highway.size = START_SIZE;
    highway.stops = (struct stop *) (struct stop **) (struct stop *) malloc(START_SIZE * sizeof(struct stop *));
    char command[20];
    while (fscanf(stdin, "%s", command) != EOF && strcmp(command, "\n") != 0) {
        switch (command[0]) {
            case 'a':
                switch (command[9]) {
                    case 's':
                        add_stop();
                        break;
                    case 'a':
                        add_car();
                        break;
                }
                break;
            case 'd':
                remove_car();
                break;
            case 'r':
                remove_stop();
                break;
            case 'p':
                elaborate();
                break;
            case 'k':
                print_highway();
                break;
        }
        set_buffer();
        printf("\n");
    }
    return 0;
}

void set_buffer() {
    while ((getchar()) != '\n');
}

void add_stop() {
    int distance, cars;
    fscanf(stdin, "%d %d", &distance, &cars);

    if (find_station(distance) != NULL) {
        printf("non aggiunta");
        return;
    }

    struct stop elem = new_stop(distance);

    for (int i = 0; i < cars; i++) {
        int range;
        scanf("%d", &range);
        add_car_to_stop(elem, range);
        if ((elem.max_range) < range)
            elem.max_range = range;
    }
    highway.stops[highway.last] = elem;
    highway.last++;
    if (highway.last == highway.size) {
        highway.size *= INCREMENT;
        highway.stops = realloc(highway.stops, highway.size * sizeof(struct stop *)); //TODO: check
    }
    sort_highway();
    printf("aggiunta");
    return;
}

struct stop *find_station(int distance) {
    int left = 0;
    int right = highway.last;
    while (left < right) {
        int mid = (left + right) / 2;
        if (highway.stops + mid == NULL)
            do {
                mid--;
            } while (highway.stops + mid == NULL);
        if (highway.stops[mid].distance == distance)
            return &highway.stops[mid];
        else if (highway.stops[mid].distance < distance)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return NULL;
}

struct stop new_stop(int distance) {
    struct stop *elem = malloc(sizeof(struct stop));
    elem->distance = distance;
    elem->max_range = 0;
    elem->cars = NULL;
    return *elem;
}

void sort_highway() {
    if (highway.last <= 1)
        return;

    // Build max heap
    for (int i = highway.last / 2 - 1; i >= 0; i--)

        heapify(highway.last, i);

    // Heap sort
    for (int i = highway.last - 1; i >= 0; i--) {

        swap(&highway.stops[0], &highway.stops[i]);

        // Heapify root element
        // to get highest element at
        // root again
        heapify( i, 0);
    }
}

void heapify(int n, int i) {
    // Find largest among root,
    // left child and right child

    // Initialize largest as root
    int largest = i;

    // left = 2*i + 1
    int left = 2 * i + 1;

    // right = 2*i + 2
    int right = 2 * i + 2;

    // If left child is larger than root
    if (left < highway.last && highway.stops[left].distance > highway.stops[largest].distance)

        largest = left;

    // If right child is larger than largest
    // so far
    if (right < highway.last && highway.stops[right].distance > highway.stops[largest].distance)

        largest = right;

    // Swap and continue heapifying
    // if root is not the largest
    // If largest is not the root
    if (largest != i) {

        swap(&highway.stops[i], &highway.stops[largest]);

        // Recursively heapify the affected
        // subtree
        heapify(highway.last, largest);
    }

}

void swap(struct stop *a, struct stop *b) {
    struct stop temp = *a;
    *a = *b;
    *b = temp;
}

void add_car() {
    printf("add_car");
}

void remove_car() {
    printf("remove_car");
}

void remove_stop() {
    printf("remove_stop");
}

void elaborate() {
    scanf("%d %d", &start, &end);
    if (start == end) {
        if (find_station(start) != NULL) {
            printf("%d", start);
            return;
        } else {
            printf("nessun percorso");
            return;
        }
    }
    p_start = find_station(start);
    p_end = find_station(end);
    if (p_start->max_range == 0 || p_end == NULL) {
        printf("nessun percorso");
        return;
    }
    if (p_start->max_range >= abs((p_end->distance - p_start->distance))) {
        printf("%d %d", start, end);
        return;
    }
    if (p_start < p_end)
        to_Milan();
    else
        to_Turin();
}

void add_car_to_stop(struct stop stop, int range) {
    while (true) {
        if (stop.cars == NULL) {
            stop.cars = new_car(range);
            return;
        }
        if (stop.cars->range == range) {
            stop.cars->count++;
            return;
        }
        if (stop.cars->range < range) {
            if (stop.cars->right == NULL) {
                stop.cars->right = new_car(range);
                return;
            } else {
                stop.cars = stop.cars->right;
                continue;
            }
        }
        if (stop.cars->range > range) {
            if (stop.cars->left == NULL) {
                stop.cars->left = new_car(range);
                return;
            } else {
                stop.cars = stop.cars->left;
                continue;
            }
        }
    }
}

struct car *new_car(int range) {
    struct car *elem = malloc(sizeof(struct car));
    elem->range = range;
    elem->count = 1;
    elem->left = NULL;
    elem->right = NULL;
    return elem;
}

void print_highway() {
    printf("Highway:\n");
    for (int i = 0; i < highway.last; i++) {
        if (highway.stops + i == NULL)
            continue;
        printf("%d] distance: %d\tmax range: %d\n", i, highway.stops[i].distance, highway.stops[i].max_range);
        print_cars(highway.stops[i].cars);
        printf("\n");
    }
}

void print_cars(struct car *pCar) {
    if (pCar == NULL)
        return;
    print_cars(pCar->left);
    printf("\t\trange: %d", pCar->range);
    printf("\tcount: %d\n", pCar->count);
    print_cars(pCar->right);
}

void to_Milan() {
}

void to_Turin() {
}

