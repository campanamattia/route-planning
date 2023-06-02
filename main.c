#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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


struct road {
    int distance;
    struct road *next;
};

void addStation();

void removeStation();

void addCarToStation();

void removeCarFromStation();

void planRoute();

void prepearBuffer();
struct car* new_car(int);
void freeCars(struct car *);
void addCar(struct car **, int);
void freeStation(struct station **);
bool insertStation(struct station *);
struct station *findStation(int);
struct car *findMinimumCar(struct car *);
struct route *allBetweenStartAndEnd(int , int );
struct car *removeOneCar(struct car *, int);
struct station *removeNode(struct station *, int);
struct station *findMinimumStation(struct station *);
void printHighway(struct station *);
void printCars(struct car *);


struct station *highway = NULL;
int main() {
    char command[20];
    while (fscanf(stdin, "%s", command) != EOF && strcmp(command, "\n") != 0){
        if (strcmp(command, "aggiungi-stazione") == 0)
            addStation();
        else if (strcmp(command, "demolisci-stazione") == 0)
            removeStation();
        else if (strcmp(command, "aggiungi-auto") == 0)
            addCarToStation();
        else if (strcmp(command, "rottama-auto") == 0)
            removeCarFromStation();
        else if (strcmp(command, "pianifica-percorso") == 0)
            planRoute();
        else if (strcmp(command, "print") == 0)
            printHighway(highway);
        prepearBuffer();
        printf("\n");
    }
    return 0;
}

void prepearBuffer() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void addStation() {
    int distance, sizePark;
    fscanf(stdin, " %d %d", &distance, &sizePark);

    //create station
    struct station *newStation = (struct station *) malloc(sizeof(struct station));
    newStation->distance = distance;
    newStation->maxRange = 0;
    newStation->cars = NULL;
    newStation->right = NULL;
    newStation->left = NULL;
    if (insertStation(newStation)) {
        //add cars
        for (int i = 0; i < sizePark; i++) {
            int fuel;
            scanf("%d", &fuel);
            if (fuel > newStation->maxRange)
                newStation->maxRange = fuel;
            addCar(&newStation->cars, fuel);
        }
        printf("aggiunta");
    } else {
        printf("non aggiunta");
    }
}

bool insertStation(struct station *newStation) {
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

void addCar(struct car **cars, int fuel) {
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


struct car* new_car(int fuel){
    struct car *newCar = (struct car *) malloc(sizeof(struct car));
    newCar->fuel = fuel;
    newCar->count = 1;
    newCar->left = NULL;
    newCar->right = NULL;
    return newCar;
}

void removeStation() {
    int distance;
    fscanf(stdin, "%d", &distance);
    highway = removeNode(highway, distance);
}

struct station *findMinimumStation(struct station *root){
    if (root == NULL)
        return NULL;
    else if (root->left != NULL)
        return findMinimumStation(root->left);
    return root;
}

struct station *removeNode(struct station *node, int key) {
    if (node == NULL) {
        printf("non demolita");
        return NULL;
    } else if (key < node->distance) {
        node->left = removeNode(node->left, key);
    } else if (key > node->distance) {
        node->right = removeNode(node->right, key);
    } else {
        // Found the node to be deleted

        // Case 1: No child or only one child
        if (node->left == NULL) {
            struct station *temp = node->right;
            freeStation(&node);
            printf("demolita");
            return temp;
        } else if (node->right == NULL) {
            struct station *temp = node->left;
            freeStation(&node);
            printf("demolita");
            return temp;
        }

        // Case 2: Two children
        struct station *minRightSubtree = findMinimumStation(node->right);
        node->distance = minRightSubtree->distance;
        node->right = removeNode(node->right, minRightSubtree->distance);
    }
    return node;
}

void freeStation(struct station **station) {
    if (station == NULL)
        return;

    freeCars((*station)->cars);
    free(*station);
}

void freeCars(struct car *car) {
    if (car == NULL)
        return;

    freeCars(car->left);
    freeCars(car->right);
    free(car);
}

void addCarToStation() {
    int distance, fuel;
    fscanf(stdin, "%d %d", &distance, &fuel);
    struct station *station = findStation(distance);

    //I can add the car to the station
    if (station != NULL) {
        if (fuel > station->maxRange)
            station->maxRange = fuel;
        addCar(&station->cars, fuel);
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

void removeCarFromStation() {
    int distance, fuel;
    fscanf(stdin, " %d %d", &distance, &fuel);
    struct station *station = findStation(distance);
    if (station != NULL) { //I can remove the car from the station
        station->cars = removeOneCar(station->cars, fuel);
    } else { // I can't remove the car because there is no station
        printf("non rottamata");
        return;
    }
}

struct car *findMinimumCar(struct car *car) {
    if (car == NULL) {
        return NULL;
    } else if (car->left != NULL) {
        return findMinimumCar(car->left);
    }
    return car;
}

struct car *removeOneCar(struct car *cars, int key) {
    if (cars == NULL) {
        printf("non rottamata");
        return NULL;
    } else if (key < cars->fuel) {
        cars->left = removeOneCar(cars->left, key);
    } else if (key > cars->fuel) {
        cars->right = removeOneCar(cars->right, key);
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
        struct car *minRightSubtree = findMinimumCar(cars->right);
        cars->fuel = minRightSubtree->fuel;
        cars->count = minRightSubtree->count;
        cars->right = removeOneCar(cars->right, minRightSubtree->fuel);
        printf("rottamata");
    }
    return cars;
}

void printCars(struct car *node);

void planRoute() {
    int start, end;
    fscanf(stdin, " %d %d", &start, &end);
    struct route *possible_stop = allBetweenStartAndEnd(start, end);
}

struct route *allBetweenStartAndEnd(int start, int end) {
    return NULL;
}

void printHighway(struct station *station) {
    if (station == NULL) {
        return;
    }
    if(station->left != NULL)
        printHighway(station->left);
    printf("stazione a distanza: %d\n", station->distance);
    if(station->cars != NULL)
            printCars(station->cars);
    else printf("\tnessuna auto\n");
    if(station->right != NULL)
        printHighway(station->right);
    return;
}

void printCars(struct car *node) {
    if (node == NULL) {
        return;
    }
    if(node->left != NULL)
        printCars(node->left);
    printf("\t-autonomia: %d ", node->fuel);
    printf("\t-numero in loco: %d ", node->count);
    printf("\n");
    if(node->right != NULL)
        printCars(node->right);
    return;
}
