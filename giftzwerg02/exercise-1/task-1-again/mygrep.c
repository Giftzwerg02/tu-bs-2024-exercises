#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>

typedef struct {
  int insensitive;
  char *outfile;
} args_t;

args_t args = {
  .insensitive = 0,
  .outfile = NULL,
};

void parseargs(int argc, char **argv, args_t *args);
void usage();

int main(int argc, char **argv) {
  parseargs(argc, argv, &args);
  if(optind >= argc+1) {
    usage();
  }
  char *keyword = argv[optind];
  for(int i = optind + 1; i < argc; i++) {
    char *filename = argv[i];

  }
  exit(EXIT_SUCCESS);
}

void parseargs(int argc, char **argv, args_t *args) {
  int c;
  int opt_i = 0;
  int opt_o = 0;
  while((c = getopt(argc, argv, "io:")) != -1) {
    switch (c) {
      case 'o':
        if(opt_o != 0) {
          usage();
        }
        opt_o++;
        args->outfile = optarg;
        break;
      case 'i':
        if(opt_i != 0) {
          usage();
        }
        opt_i++;
        args->insensitive = 1;
        break;
      case '?':
        usage();
      default: 
        assert(0);
    }
  }
}

void usage() {
  printf("nope\n");
  exit(EXIT_FAILURE);
}

void err(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}
