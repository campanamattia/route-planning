#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATION 10000
#define BUFFER 512
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
  int max;
  int index;
  struct car *cars;
  struct station *next, *prec;
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

void aggiungi_stazione();
void demolisci_stazione();
void aggiungi_auto();
void rottama_auto();
void calcola_percorso();

unsigned int hash(int);
unsigned int search_alg(int, int);
void clear_buffer();
void add_to_table(int, int);
void add_to_buffer(int);

struct buffer buffer;
struct station tail;
struct mastro mastro;

int main (){
  char command[18];
  buffer.arr = (int *) malloc (sizeof(int) * BUFFER);
  buffer.load = 0;

  mastro.size = STATION;
  mastro.load = 0;
  mastro.table = (struct station **) calloc (STATION, sizeof(struct station *));

  while (fscanf(stdin, "%s", command) != EOF){
    switch(command[12]){
      case 'z': aggiungi_stazione();
        break;
      case 'a': demolisci_stazione();
        break;
      case 'o': aggiungi_auto();
        break;
      case '\0': rottama_auto();
        break;
      case 'r': calcola_percorso();
        break;
      default: print_status();
    }
    clear_buffer();
  }
  return 0;
}

void clear_buffer(){
  while (getchar() != '\n');
}

void aggiungi_stazione(){
  int key, size, index;
  fscanf(stdin, "%d", &key);
  index = find(key);

  if(mastro.table[index] != NULL){
    printf("non aggiunta\n");
    return;
  }
  
  add_to_table(key, index);
  add_to_buffer(index);

  fscanf(stdin, "%d", &size);
  for(int i = 0; i < size; i++){
    fscanf(stdin, "%d", &key);
    insert_car(index, key);
    if(mastro.table[index]->max < key)
      mastro.table[index]->max = key;
  }

  printf("aggiunta\n");
}

void add_to_table(int key, int index){
  struct station *elem = (struct station *) malloc (sizeof(struct station));
  elem->key = key;
  elem->max = -1;
  elem->cars = NULL;
  elem->next = NULL;
  elem->prec = NULL;
  
  mastro.table[index] = elem;
}

void add_to_buffer(int index){
  buffer.arr[buffer.load] = index;
  mastro.table[index]->index = buffer.load;
  
  if(buffer.load == 0){
    buffer.load++;
    return;
  }

  insert_heapfy(buffer.load);
  if (buffer.load++ == BUFFER)
    set_road();
}

int find (int key){
  unsigned int index = hash(key);
  if (mastro.table[index] == NULL)
    return index;

  for(int attempt = 1; mastro.table[index] != NULL &&  mastro.table[index]->key != key && attempt < mastro.size; attempt ++)
    index = search_alg(index, attempt);

  return index;
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

int remove_max(){
  int max = buffer.arr[0];

  swap_elem(0, buffer.load--);

  delete_heapfy(0);

  return max;
}

void remove_pos(int pos){
  int a;
}

void insert_heapfy(int pos){
  int parent = (pos-1)/2;

  if (! mastro.table[buffer.arr[parent]]->key < mastro.table[buffer.arr[pos]]-> key)
    return;

  swap_elem(parent, pos);
  insert_heapfy(parent);
}

void delete_heapfy(int pos){
  int left = pos * 2 + 1;
  int right = pos * 2 + 2;
  int max = pos;

  if (left < buffer.load && mastro.table[buffer.arr[left]]->key > mastro.table[buffer.arr[max]]->key)
    max = left;
  if (right < buffer.load && mastro.table[buffer.arr[right]]->key > mastro.table[buffer.arr[max]]->key)
    max = right;

  if (max == pos)
    return;

  swap_elem(max, pos);
  max_heapfy(max);
}

void swap_elem(int temp1, int temp2){
  mastro.table[buffer.arr[temp1]]->index = temp2;
  mastro.table[buffer.arr[temp2]]->index = temp1;

  int index = buffer.arr[temp1];
  buffer.arr[temp1] = buffer.arr[temp2];
  buffer.arr[temp2] = buffer.arr[temp1];
}
