#include "lib.h"
#include "fcntl.h"
#include "semaphore.h"
#include "sys/mman.h"
#include "unistd.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

solution_t read_circ(circbuf_t *circbuf) {
  solution_t res = circbuf->solutions[circbuf->read_idx];
  circbuf->read_idx = (circbuf->read_idx + 1) % BUF_SIZE;
  return res;
}

void write_circ(circbuf_t *circbuf, solution_t *data) {
  circbuf->solutions[circbuf->write_idx] = *data;
  circbuf->write_idx = (circbuf->write_idx + 1) % BUF_SIZE;
}

circbuf_t *open_circbuf_shm(int *fd) {
  *fd = shm_open(SHM_NAME, O_RDWR, PERMISSIONS);
  if(*fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  circbuf_t *buf = mmap(NULL, sizeof(circbuf_t), PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);

  if(buf == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  return buf;
}

circbuf_t *create_circbuf_shm(int *fd) {
  *fd = shm_open(SHM_NAME, O_CREAT | O_RDWR | O_EXCL, PERMISSIONS);
  if(*fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  if (ftruncate(*fd, sizeof(circbuf_t)) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }

  circbuf_t *buf = mmap(NULL, sizeof(circbuf_t), PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);

  if(buf == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  return buf;
}

sem_t *create_sem(char *name, int init_val) {
  sem_t *sem = sem_open(name, O_CREAT | O_EXCL | O_RDWR, PERMISSIONS, init_val);
  if(sem == SEM_FAILED) {
    perror("sem_open (create)");
    exit(EXIT_FAILURE);
  }
  return sem;
}

sem_t *open_sem(char *name) {
  sem_t *sem = sem_open(name, O_RDWR);
  if(sem == SEM_FAILED) {
    perror("sem_open (open)");
    exit(EXIT_FAILURE);
  }
  return sem;
}

void usage(void) {
  fprintf(stderr, "%s\n", "you done goofed up");
  exit(EXIT_FAILURE);
}

void err(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int parsenum(char *str) {
  long res = strtol(str, NULL, 10);
  if (errno != 0) {
    usage();
  }
  return res;
}

void print_edges(edge_t *edges, int count) {
  for(int i = 0; i < count; i++) {
    edge_t e = edges[i];
    printf(" %d-%d", e.n1, e.n2);
  }
}


void print_edge(edge_t *edge) {
  printf("Edge: %d-%d\n", edge->n1, edge->n2);
}


void close_sem(sem_t *sem) {
  if(sem_close(sem) == -1) {
    perror("sem_close");
    exit(EXIT_FAILURE);
  }
}

void unlink_sem(char *name) {
  if(sem_unlink(name) == -1) {
    perror("sem_unlink");
    exit(EXIT_FAILURE);
  }
}

void close_circbuf_shm(int fd, circbuf_t *buf) {
  if(munmap(buf, sizeof(circbuf_t)) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }
  if(close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }
}

void unlink_circbuf_shm() {
  shm_unlink(SHM_NAME);
}
