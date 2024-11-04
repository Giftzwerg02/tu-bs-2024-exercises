#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Integers from 1 to 23:\n");
  uint64_t ints[] = {0x0807060504030201, 0x100f0e0d0c0b0a09, 0x17161514131211};
  for (size_t i = 0; i <= 22; i++) {
    printf("  %d\n", ((uint8_t*) ints)[i]);
  }
  return 0;
}
