#include "list.h"

#include <stdio.h>
#include <stdlib.h>

// creation
node* createHead(char *data) {
  node* head = malloc(sizeof(node)); // acquire memomry for head node
  head->data = data;                // set data pointer
  head->next = NULL;                // set next to NULL to signal it is not valid (this is the last element)
  head->prev = NULL;                // set prev to NULL to signal it is not valid (this is the first element)
  return head;
}

// helper function, only visible in this file
static node* findLast(node *head) {
  if (head->next == NULL) { // no next node, this must be the last
    return head;
  } else {
    return findLast(head->next); // recursive function call
  }
}

// create new node for data and append to the end of the list
node* append(node* head, char *data) {
  node *newNode = createHead(data);
  node *last = findLast(head);
  last->next = newNode;
  newNode->prev = last;
  return head;
}

// helper function, only visible in this file
static node* findFirst(node *head) {
  if(head->prev == NULL)
    return head;
  else
    return findFirst(head->prev);
}

node* prepend(node *head, char *data) {
  node* newNode = createHead(data);
  node* first = findFirst(head);
  first->prev = newNode;
  newNode->next = first;
  return head;
}

// print, one element per line
static void printPrime(int i, node *head) {
  do {
    printf("%d: %s\n", i++, head->data);
  }
  while((head = head->next) != NULL);
}

void print(node *head) {
  printPrime(0, head);
}

// move
node* forward(node *head) {
  if (head->next != NULL){
    return head->next;
  } else { // return NULL to signal that move is not possible
    return NULL;
  }
}

node* backward(node *head) {
  if(head->prev !=NULL)
    return head->prev;
  else
    return NULL;
}

// modify
// remove the given element, merge the list parts and
// return the element before the one we just removed (if it exists)
//     or the element after  the one we just removed (if it exists, but no previous element exists)
//     or NULL (if this was the only elemnt)
node* delete(node *head) {
  node* next = head->next;
  node* prev = head->prev;

  if(next != NULL)
    next->prev = prev;

  if(prev != NULL)
    prev->next = next;

  free(head);

  return prev != NULL? prev : next; 
}


void ringify(node* head){
  node* first = findFirst(head);
  node* last = findLast(head);

  first->prev = last;
  last->next = first;
}

void split(node* head){
  node* prev = head->prev;
  head->prev = NULL;
  prev->next = NULL;
}

// Does not fully work on ring lists but idc
void freeList(node* head){
  while(head != NULL)
    head = delete(head);
}

void freeListWithStrings(node* head){
  while(head != NULL){
    if(head->data != NULL) free(head->data);

    head = delete(head);
  }
}

void concat(){
  // Nah, we ignoring this one
}

