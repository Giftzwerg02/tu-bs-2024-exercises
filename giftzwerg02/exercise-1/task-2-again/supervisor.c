#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include "lib.h"

typedef struct {
  int *limit;
  int delay;
} arguments_t;

void argparse(int argc, char **argv, arguments_t *arguments);

int main(int argc, char **argv) {
  arguments_t args = {
      // set defaults
      .limit = NULL,
      .delay = 0,
  };

  argparse(argc, argv, &args);

  circbuf_t *buf = create_circbuf_shm();
  sem_t *sem_xor = create_sem(SEM_NAME_XOR, 1);
  sem_t *sem_read = create_sem(SEM_NAME_READ, 0);
  sem_t *sem_write = create_sem(SEM_NAME_WRITE, BUF_SIZE);

  while(1) {
    sem_wait(sem_read);

    solution_t sol = read_circ(buf);
    printf("got solution! ---\n");
    print_edges(sol.removed_edges, sol.count);
    printf("------\n");

    sem_post(sem_write);
  }

  exit(EXIT_SUCCESS);
}

void argparse(int argc, char **argv, arguments_t *arguments) {
  int c;
  int opt_n = 0, opt_w = 0;
  while ((c = getopt(argc, argv, "n:w:")) != 0) {
    switch (c) {
    case 'n':
      if (opt_n != 0) {
        usage();
      }
      opt_n = 1;
      int l = parsenum(optarg);
      if (l < 0) {
        usage();
      }
      arguments->limit = &l;
      break;
    case 'w':
      if (opt_w != 0) {
        usage();
      }
      opt_w = 1;
      int d = parsenum(optarg);
      if (d < 0) {
        usage();
      }
      arguments->delay = d;
      break;
    case '?':
      usage();
      break;
    default:
      assert(0);
    }
  }
  if (argc != optind) {
    usage();
  }
}
