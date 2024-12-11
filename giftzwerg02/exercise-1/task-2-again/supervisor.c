#include <assert.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lib.h"

typedef struct {
  int *limit;
  int delay;
} arguments_t;

int fd;
circbuf_t *buf;
sem_t *sem_read;
sem_t *sem_write;
sem_t *sem_xor;
int stop_sup;

void argparse(int argc, char **argv, arguments_t *arguments);

void shutdown() {
  printf("Shutting down...\n");
  buf->stop = 1; 
  close_circbuf_shm(fd, buf);
  unlink_circbuf_shm();
  close_sem(sem_read);
  close_sem(sem_write);
  close_sem(sem_xor);
  unlink_sem(SEM_NAME_READ);
  unlink_sem(SEM_NAME_WRITE);
  unlink_sem(SEM_NAME_XOR);
}

void sighandler(int signal) { 
  printf("sighandler\n");
  stop_sup = 1;
}

int loop_limit(int *l) {
  if(l == NULL) {
    return 1;
  }
  *l -= 1;
  return *l;
}

int main(int argc, char **argv) {
  arguments_t args = {
      // set defaults
      .limit = NULL,
      .delay = 0,
  };

  argparse(argc, argv, &args);

  buf = create_circbuf_shm(&fd);

  signal(SIGINT, sighandler);
  signal(SIGTERM, sighandler);

  sem_read = create_sem(SEM_NAME_READ, 0);
  sem_write = create_sem(SEM_NAME_WRITE, BUF_SIZE);
  sem_xor = create_sem(SEM_NAME_XOR, 1);

  int bestsol = INT32_MAX;

  sleep(args.delay);

  while (loop_limit(args.limit) && stop_sup != 1) {
    sem_wait(sem_read);

    solution_t sol = read_circ(buf);

    sem_post(sem_write);

    if (sol.count < bestsol) {
      bestsol = sol.count;
      printf("Solution with %d edges:", sol.count);
      print_edges(sol.removed_edges, sol.count);
      printf("\n");
    }

    if(bestsol == 0) {
      break;
    }
  }

  if(bestsol == 0) {
    printf("The graph is 3-colorable!\n");
  } else {
    printf("The graph might not be 3-colorable, best solutino removes %d edges.\n", bestsol);
  }

  shutdown();

  exit(EXIT_SUCCESS);
}

void argparse(int argc, char **argv, arguments_t *arguments) {
  int c;
  int opt_n = 0, opt_w = 0;
  while ((c = getopt(argc, argv, "n:w:")) != -1) {
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
