#include <stdio.h>

#include "list.h"

int main (int argc, char** argv) {
  if(argc == 1) return 0;

  node* head = createHead(argv[1]);
  
  for(int i = 2; i < argc; i++)
    append(head, argv[i]);

  print(head);

  freeList(head);

  return 0;
}
