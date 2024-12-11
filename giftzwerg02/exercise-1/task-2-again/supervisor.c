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
  int limit;
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
  stop_sup = 1;
}

int main(int argc, char **argv) {
  arguments_t args = {
      // set defaults
      .limit = -1,
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

  while (args.limit != 0 && stop_sup != 1) {
    if(args.limit != -1) {
      args.limit--;
    }
    sem_wait(sem_read);

    solution_t sol = read_circ(buf);

    sem_post(sem_write);

    if (sol.count < bestsol) {
      bestsol = sol.count;
    }

    if(bestsol == 0) {
      break;
    }
  }

  if(bestsol == 0) {
    printf("The graph is 3-colorable!\n");
  } else {
    printf("The graph might not be 3-colorable, best solution removes %d edges.\n", bestsol);
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
      arguments->limit = l;
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
