#include "linkedlist.h"

struct node {
  int data;
  char * cmd;
  struct node * next;
};

struct list {
  Node * head; 
};

Node * createnode(int data, char * cmd);

Node * createnode(int data, char * cmd){
  Node * newNode = malloc(sizeof(Node));
  newNode->cmd = (char*) malloc(sizeof(char) * strlen(cmd));
  if (!newNode) {
    return NULL;
  }
  newNode->data = data;
  strcpy(newNode->cmd, cmd);
  newNode->next = NULL;
  return newNode;
}

List * makelist(){
  List * list = malloc(sizeof(List));
  if (!list) {
    return NULL;
  }
  list->head = NULL;
  return list;
}

void display(List * list) {
  Node * current = list->head;
  if(list->head == NULL) 
    return;
  
  for(; current != NULL; current = current->next) {
    printf("[%d] %s\n", current->data, current->cmd);
  }
}

Node * get_proc(int data, List * list) {
  Node * current = list->head;
  if(list->head == NULL) 
    return NULL;
  
  for(; current != NULL; current = current->next) {
    if (current->data == data) {
        return current;
    }
  }
  return NULL;
}

void add(int data, char * cmd, List * list){
  Node * current = NULL;
  if(list->head == NULL){
    list->head = createnode(data, cmd);
  }
  else {
    current = list->head; 
    while (current->next!=NULL){
      current = current->next;
    }
    current->next = createnode(data, cmd);
  }
}

void delete(int data, List * list){
  Node * current = list->head;            
  Node * previous = current;           
  while(current != NULL){           
    if(current->data == data){      
      previous->next = current->next;
      if(current == list->head)
        list->head = current->next;
      free(current->cmd);
      free(current);
      return;
    }                               
    previous = current;             
    current = current->next;        
  }                                 
}                                   

void reverse(List * list){
  Node * reversed = NULL;
  Node * current = list->head;
  Node * temp = NULL;
  while(current != NULL){
    temp = current;
    current = current->next;
    temp->next = reversed;
    reversed = temp;
  }
  list->head = reversed;
}

void destroy(List * list){
  Node * current = list->head;
  Node * next = current;
  while(current != NULL){
    free(current->cmd);
    next = current->next;
    free(current);
    current = next;
  }
  free(list);
}