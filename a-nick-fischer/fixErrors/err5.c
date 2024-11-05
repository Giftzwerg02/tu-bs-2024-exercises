#include <stdio.h>
#include <stdlib.h>

typedef struct {
  float x;
  float y;
} vec;

vec *plus(vec *a, vec *b) {
  vec *r = malloc(sizeof (vec));
  r->x = a->x + b->x;
  r->y = a->y + b->y;
  return r;
}

int main (int argc, char* argv[]) {
  vec a = { 12.34, 45.67 };
  vec b = { 89.1,  1011.12 };

  vec *aPlusB = plus(&a, &b);
  printf("a + b = { x: %f, y: %f }\n", aPlusB->x, aPlusB->y);
  free(aPlusB);
}
