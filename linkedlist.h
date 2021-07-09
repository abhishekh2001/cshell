#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LINKEDLIST_HEADER
#define LINKEDLIST_HEADER

typedef struct node Node;

typedef struct list List;

struct node {
  int shell_id;
  int data;
  char * cmd;
  struct node * next;
};

struct list {
  Node * head; 
  int num;
};

List * makelist();
int add(int data, char * cmd, List * list);
void delete(int data, List * list);
void display(List * list);
Node * get_proc(int data, List * list);
void reverse(List * list);
void destroy(List * list);
int get_node_count(List * list);
Node * get_proc_shell_id(int data, List * list);

#endif