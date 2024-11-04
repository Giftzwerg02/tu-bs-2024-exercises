#include <stdio.h>

#include "list.h"

int main (int argc, char** argv) {

	node* head;
	for(int i = 0; i < argc; i++) {
		if(head == NULL) {
			head = createHead(argv[i]);
		} else {
			append(head, argv[i]);
		}
	}

	print(head);

	return 0;
}
