#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

struct car {
    int fuel, count;
    struct car *right;
    struct car *left;
};

struct station {
    int distance;
    int maxRange;
    struct station *right;
    struct station *left;
    struct car *cars;
};

struct stop {
    int distance;
    int maxRange;
};

struct route {
    int size;
    struct stop *stops;
};

void add_station();

void remove_station();

void add_car_to_station();

void remove_car_from_station();

void plan_route();

void prepare_buffer();

struct car *new_car(int);

void free_cars(struct car *);

void add_car(struct car **, int);

void free_station(struct station **);

bool insert_station(struct station *);

struct station *find_station(int);

struct car *find_min_car(struct car *);

struct stop *all_between(int, int);

struct car *remove_one_car(struct car *, int);

struct station *remove_node(struct station *, int);

struct station *find_min_station(struct station *);

void print_highway(struct station *station);

void printCars(struct car *);


struct station *highway;
struct route *route;


int main() {
    route = NULL;
    highway = NULL;

    char command[20];
    while (fscanf(stdin, "%s", command) != EOF && strcmp(command, "\n") != 0) {
        if (strcmp(command, "aggiungi-stazione") == 0)
            add_station();
        else if (strcmp(command, "demolisci-stazione") == 0)
            remove_station();
        else if (strcmp(command, "aggiungi-auto") == 0)
            add_car_to_station();
        else if (strcmp(command, "rottama-auto") == 0)
            remove_car_from_station();
        else if (strcmp(command, "pianifica-percorso") == 0)
            plan_route();
        else if (strcmp(command, "print") == 0)
            print_highway(highway);
        prepare_buffer();
        printf("\n");
    }
    return 0;
}

void prepare_buffer() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void add_station() {
    int distance, sizePark;
    fscanf(stdin, " %d %d", &distance, &sizePark);

    //create station
    struct station *newStation = (struct station *) malloc(sizeof(struct station));
    newStation->distance = distance;
    newStation->maxRange = 0;
    newStation->cars = NULL;
    newStation->right = NULL;
    newStation->left = NULL;
    if (insert_station(newStation)) {
        //add cars
        for (int i = 0; i < sizePark; i++) {
            int fuel;
            scanf("%d", &fuel);
            if (fuel > newStation->maxRange)
                newStation->maxRange = fuel;
            add_car(&newStation->cars, fuel);
        }
        printf("aggiunta");
    } else {
        printf("non aggiunta");
    }
}

bool insert_station(struct station *newStation) {
    if (highway == NULL) {
        highway = newStation;
        return true;
    }

    struct station *current = highway;
    while (current != NULL) {
        if (current->distance == newStation->distance)
            return false;
        if (current->distance > newStation->distance) {
            if (current->left == NULL) {
                current->left = newStation;
                return true;
            } else
                current = current->left;
        } else {
            if (current->right == NULL) {
                current->right = newStation;
                return true;
            } else
                current = current->right;
        }
    }
    return false;
}

void add_car(struct car **cars, int fuel) {
    if (*cars == NULL) {
        *cars = new_car(fuel);
        return;
    }
    struct car *current = *cars;
    do {
        if (current->fuel == fuel) {
            current->count++;
            return;
        }
        if (current->fuel > fuel) {
            if (current->left == NULL) {
                current->left = new_car(fuel);
                return;
            }
            current = current->left;
        } else {
            if (current->right == NULL) {
                current->right = new_car(fuel);
                return;
            }
            current = current->right;
        }
    } while (true);
}


struct car *new_car(int fuel) {
    struct car *newCar = (struct car *) malloc(sizeof(struct car));
    newCar->fuel = fuel;
    newCar->count = 1;
    newCar->left = NULL;
    newCar->right = NULL;
    return newCar;
}

void remove_station() {
    int distance;
    fscanf(stdin, "%d", &distance);
    highway = remove_node(highway, distance);
}

struct station *find_min_station(struct station *root) {
    if (root == NULL)
        return NULL;
    else if (root->left != NULL)
        return find_min_station(root->left);
    return root;
}

struct station *remove_node(struct station *node, int key) {
    if (node == NULL) {
        printf("non demolita");
        return NULL;
    } else if (key < node->distance) {
        node->left = remove_node(node->left, key);
    } else if (key > node->distance) {
        node->right = remove_node(node->right, key);
    } else {
        // Found the node to be deleted

        // Case 1: No child or only one child
        if (node->left == NULL) {
            struct station *temp = node->right;
            free_station(&node);
            printf("demolita");
            return temp;
        } else if (node->right == NULL) {
            struct station *temp = node->left;
            free_station(&node);
            printf("demolita");
            return temp;
        }

        // Case 2: Two children
        struct station *minRightSubtree = find_min_station(node->right);
        node->distance = minRightSubtree->distance;
        node->right = remove_node(node->right, minRightSubtree->distance);
    }
    return node;
}

void free_station(struct station **station) {
    if (station == NULL)
        return;

    free_cars((*station)->cars);
    free(*station);
}

void free_cars(struct car *car) {
    if (car == NULL)
        return;

    free_cars(car->left);
    free_cars(car->right);
    free(car);
}

void add_car_to_station() {
    int distance, fuel;
    fscanf(stdin, "%d %d", &distance, &fuel);
    struct station *station = find_station(distance);

    //I can add the car to the station
    if (station != NULL) {
        if (fuel > station->maxRange)
            station->maxRange = fuel;
        add_car(&station->cars, fuel);
        printf("aggiunta");

        // I can't add the car because there is no station
    } else {
        printf("non aggiunta");
        return;
    }
}

struct station *findStation(int distance) {
    struct station *current = highway;
    while (current != NULL) {
        if (current->distance == distance)
            return current;
        else if (current->distance > distance)
            current = current->left;
        else
            current = current->right;
    }
    return NULL;
}

void remove_car_from_station() {
    int distance, fuel;
    fscanf(stdin, " %d %d", &distance, &fuel);
    struct station *station = findStation(distance);
    if (station != NULL) { //I can remove the car from the station
        station->cars = remove_one_car(station->cars, fuel);
    } else { // I can't remove the car because there is no station
        printf("non rottamata");
        return;
    }
}

struct car *find_min_car(struct car *car) {
    if (car == NULL) {
        return NULL;
    } else if (car->left != NULL) {
        return find_min_car(car->left);
    }
    return car;
}

struct car *remove_one_car(struct car *cars, int key) {
    if (cars == NULL) {
        printf("non rottamata");
        return NULL;
    } else if (key < cars->fuel) {
        cars->left = remove_one_car(cars->left, key);
    } else if (key > cars->fuel) {
        cars->right = remove_one_car(cars->right, key);
    } else {
        // Found the node to be deleted

        //Case 0: more than one car
        if (cars->count > 1) {
            cars->count--;
            printf("rottamata");
            return cars;
        }

        // Case 1: No child or only one child
        if (cars->left == NULL) {
            struct car *temp = cars->right;
            free(cars);
            printf("rottamata");
            return temp;
        } else if (cars->right == NULL) {
            struct car *temp = cars->left;
            free(cars);
            printf("rottamata");
            return temp;
        }

        // Case 2: Two children
        struct car *minRightSubtree = find_min_car(cars->right);
        cars->fuel = minRightSubtree->fuel;
        cars->count = minRightSubtree->count;
        cars->right = remove_one_car(cars->right, minRightSubtree->fuel);
        printf("rottamata");
    }
    return cars;
}


void plan_route() {
    int start, end;
    fscanf(stdin, " %d %d", &start, &end);
    all_between(start, end);
}

struct stop *all_between(int start, int end) {
    route->size = 0;
    return NULL;
}

void print_highway(struct station *station) {
    if (station == NULL) {
        return;
    }
    if (station->left != NULL)
        print_highway(station->left);
    printf("stazione a distanza: %d\n", station->distance);
    if (station->cars != NULL)
        printCars(station->cars);
    else printf("\tnessuna auto\n");
    if (station->right != NULL)
        print_highway(station->right);
    return;
}

void printCars(struct car *node) {
    if (node == NULL) {
        return;
    }
    if (node->left != NULL)
        printCars(node->left);
    printf("\t-autonomia: %d ", node->fuel);
    printf("\t-numero in loco: %d ", node->count);
    printf("\n");
    if (node->right != NULL)
        printCars(node->right);
    return;
}
