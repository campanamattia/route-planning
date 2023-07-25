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

void aggiungi_stazione();
void demolisci_stazione();
void aggiungi_auto();
void rottama_auto();
void calcola_percorso();

int find(int);
unsigned int hash(int);
unsigned int search_alg(int, int);
void clear_buffer();
void add_to_table(int, int);
void add_to_buffer(int);
void remove_pos(int);
int remove_max();
void delete_heapfy(int);
void insert_heapfy(int);
void swap_elem(int, int);
void set_road();
void insert_car(int, int);
struct car* new_car(int);
void free_binary_tree(struct car *);
void realloc_table();
void remove_car(int, struct car*, struct car*);
int reload_max(int);

struct buffer buffer;
struct mastro mastro;
struct station *tail;


int main (){
  char command[18];
  tail = NULL;

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
      //default: print_status();
    }
    clear_buffer();
  }
  return 0;
}

void clear_buffer(){
  while (getchar() != '\n');
}

int find (int key){
  unsigned int index = hash(key);
  if (mastro.table[index] == NULL)
    return index;

  for(int attempt = 1; mastro.table[index] != NULL &&  mastro.table[index]->key != key && attempt < mastro.size; attempt ++)
    index = search_alg(index, attempt);

  return index;
}

void aggiungi_stazione(){
  int key, size, index;
  fscanf(stdin, "%d", &key);
  index = find(key);

  if(mastro.table[index] != NULL || mastro.table[index]->key == key){
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

void demolisci_stazione(){
  int index;
  fscanf(stdin, "%d", &index);
  index = find(index);

  if(mastro.table[index] == NULL){
    printf("non demolita\n");
    return;
  }

  free_binary_tree(mastro.table[index]->cars);

  if(mastro.table[index]->index == -1){
    mastro.table[index]->next->prev = mastro.table[index]->prev;
    mastro.table[index]->prev->next = mastro.table[index]->next;
    free(mastro.table[index]);
    mastro.table[index] = NULL;
  } else {
    remove_pos(mastro.table[index]->index);
    free(mastro.table[index]);
    mastro.table[index] = NULL;
  }

  printf("demolita\n");
}

void aggiungi_auto(){
  int index;
  fscanf(stdin, "%d", &index);
  index = find(index);

  if(mastro.table[index] == NULL){
    printf("non aggiunta\n");
    return;
  }
  int key;
  fscanf(stdin, "%d", &key);
  insert_car(index, key);
  printf("aggiunta\n");
}

void rottama_auto(){
  int index;
  fscanf(stdin, "%d", &index);
  index = find(index);

  if(mastro.table[index] == NULL){
    printf("non rottamata\n");
    return;
  }

  int key;
  fscanf(stdin, "%d", &key);
  
  struct car *node = mastro.table[index]->cars;
  struct car *father = node;
  while(1){
    if(node == NULL){
      printf("non rottamata\n");
      return;
    }

    if(node->range > key){
      father = node;
      node = node->left;
      continue;
    }

    if(node->range < key){
      father = node;
      node = node->right;
      continue;
    }

    if((node->count--) == 0){
      remove_car(index, father, node);
      if(key == mastro.table[index]->max)
        mastro.table[index]->max = reload_max(index);
    }
    printf("rottamata\n");
    return;
  }
}

void calcola_percorso(){
  int start, end;
  fscanf(stdin, "%d %d", &start, &end);
  
  if(start == end && find(start) != NULL){
    printf("%d\n", start);
    return;
  }
  
  int km = abs(start - end);
  start = find(start);
  end = find(end);

  if(mastro.table[start] == NULL || mastro.table[end] == NULL){
    printf("nessun percorso\n");
    return;
  }

  if(mastro.table[start]->max >= km){
    printf("%d %d\n", mastro.table[start]->key, mastro.table[end]->key);
    return;
  }

  set_road();

  if(start < end)
    left_to_right(start, end, km);
  else
    right_to_left(start, end, km);
}

void set_road(){
  struct station *tmp = tail;

  while(buffer.load > 0){
    int index = remove_max();
    mastro.table[index]->index = -1;
    int key = mastro.table[index]->key;

    if (tmp == NULL){
      tail = mastro.table[index];
      tmp = tail;
      continue;
    }

    while(tmp->key > key && tmp->prev != NULL){
      tmp = tmp->prev;
    }

    if (tmp->key < key){
      tmp->next->prev = mastro.table[index];
      mastro.table[index]->next = tmp->next;
      mastro.table[index]->prev = tmp;
      tmp->next = mastro.table[index];
      continue;
    }

    mastro.table[index]->next = tmp;
    mastro.table[index]->prev = tmp->prev;
    tmp->prev = mastro.table[index];
  }
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

void add_to_table(int key, int index){
  struct station *elem = (struct station *) malloc (sizeof(struct station));
  elem->key = key;
  elem->max = -1;
  elem->cars = NULL;
  elem->next = NULL;
  elem->prev = NULL;
  mastro.table[index] = elem;

  mastro.load++;
  float density = mastro.load / (float) mastro.size;
  if(density > DENSITY)
    realloc_table();
}

void realloc_table(){
  int new = mastro.size * UP;
  mastro.table = realloc(mastro.table, new);
  for(int index = 0; index + mastro.size < new; index ++)
    mastro.table[mastro.size + index] = NULL;
  
  mastro.size = new;
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

int remove_max(){
  int max = buffer.arr[0];
  swap_elem(0, buffer.load--);
  delete_heapfy(0);

  return max;
}

void remove_pos(int pos){
  swap_elem(pos, buffer.load--);
  delete_heapfy(pos);
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
  delete_heapfy(max);
}

void swap_elem(int temp1, int temp2){
  mastro.table[buffer.arr[temp1]]->index = temp2;
  mastro.table[buffer.arr[temp2]]->index = temp1;

  int index = buffer.arr[temp1];
  buffer.arr[temp1] = buffer.arr[temp2];
  buffer.arr[temp2] = buffer.arr[temp1];
}

void free_binary_tree(struct car *ptr){
  if (ptr == NULL)
    return;
  free_binary_tree(ptr->right);
  free_binary_tree(ptr->left);

  free(ptr);
}

void insert_car(int index, int key){
  struct car *tree = mastro.table[index]->cars;

  if(tree == NULL){
    mastro.table[index]->cars = new_car(key);
    return;
  }

  while(1){
    if(tree->range == key){
      tree->count++;
      return;
    }

    if(tree->range > key){
      if(tree->left != NULL){
        tree = tree->left;
        continue;
      }
      tree->left = new_car(key);
      return;
    }

    if(tree->range < key){
      if(tree->right != NULL){
        tree = tree->right;
        continue;
      }
      tree->right = new_car(key);
      return;
    }
  }
}

struct car* new_car(int key){
  struct car *elem = (struct car *) malloc(sizeof(struct car));
  elem->range = key;
  elem->count = 1;
  elem->right = NULL;
  elem->left = NULL;

  return elem;
}

void remove_car(int index, struct car *father, struct car *node){
  struct car **side;
  side = &(mastro.table[index]->cars);

  if(father != node){
    if(father->left->range == node->range)
      side = &(father->left);
    else 
      side = &(father->right);
  }

  if(node->right == NULL && node->left == NULL){
    *side = NULL;
    free(node);
    return;
  }

  if(node->left == NULL){
    *side = node->right;
    free(node);
    return;
  }

  if(node->right == NULL){
    *side = node->left;
    free(node);
    return;
  }

  struct car *sub = node->right;
  struct car *sub_father = sub;
  while(1){
    if(sub->left == NULL)
      break;
    sub_father = sub;
    sub = sub->left;
  }
  
  sub_father->left = sub->right;
  node->range = sub->range;
  node->count = sub->count;
  free(sub);
}

int reload_max(int index){
  struct car *tmp = mastro.table[index]->cars;

  while(tmp->right != NULL){
    tmp = tmp->right;
  }

  return tmp->range;
}

void left_to_right(int start, int end, int km){

}

void right_to_left(int start, int end, int km){

}