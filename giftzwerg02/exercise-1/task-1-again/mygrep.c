#include <ctype.h>
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
void err(char *msg);
void print_matches(FILE *fp, char *keyword, int insensitive, FILE *out);
char *strtolower(char *str);
void exec(FILE *fp, char* keyword, int insensitive, FILE *out);

int main(int argc, char **argv) {
  parseargs(argc, argv, &args);
  if(argc < 2 || optind >= argc) {
    usage();
  }

  FILE *out = stdout;
  if(args.outfile != NULL) {
    out = fopen(args.outfile, "w");
    if(out == NULL) {
      err("fopen");
    }
  }

  char *keyword = argv[optind];
  if(optind + 1 == argc) {
      exec(stdin, keyword, args.insensitive, out);
  } else {
    for(int i = optind + 1; i < argc; i++) {
      char *filename = argv[i];
      FILE *fp = fopen(filename, "r");
      exec(fp, keyword, args.insensitive, out);
    }
  }

  if(args.outfile != NULL) {
    if(fclose(out) == EOF) {
      err("fclose");
    }
  }
  exit(EXIT_SUCCESS);
}

void exec(FILE *fp, char* keyword, int insensitive, FILE *out) {
    if(fp == NULL) {
      err("fopen");
    }

    print_matches(fp, keyword, insensitive, out);

    if(fclose(fp) == EOF) {
      err("fclose");
    }
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

void print_matches(FILE *fp, char *keyword, int insensitive, FILE *out) {
  char buf[8192];
  while(fgets(buf, sizeof(buf), fp) != NULL) {
    if(insensitive == 1) {
      char *ik = strtolower(keyword);
      char *ib = strtolower(buf);
      if(strstr(ib, ik) != NULL) {
        if(fputs(buf, out) == EOF) {
          err("fputs");
        }
      }
    } else {
      if(strstr(buf, keyword) != NULL) {
        if(fputs(buf, out) == EOF) {
          err("fputs");
        }
      }
    }
  }
}

char *strtolower(char *str) {
  int size = strlen(str);
  char *ret = malloc(size + 1);
  for(int i = 0; i < size; i++) {
    ret[i] = tolower(str[i]);
  }
  ret[size] = '\0';
  return ret;
}
