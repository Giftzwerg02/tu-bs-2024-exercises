#include "lib.h"
#include "fcntl.h"
#include "semaphore.h"
#include "sys/mman.h"
#include "unistd.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

solution_t read_circ(circbuf_t *cirbuf) {
  solution_t res = cirbuf->solutions[cirbuf->read_idx];
  cirbuf->read_idx = (cirbuf->read_idx + 1) % BUF_SIZE;
  return res;
}

void write_circ(circbuf_t *cirbuf, solution_t *data) {
  cirbuf->solutions[cirbuf->write_idx] = *data;
  cirbuf->read_idx = (cirbuf->write_idx + 1) % BUF_SIZE;
}

circbuf_t *open_circbuf_shm() {
  int fd = shm_open(SHM_NAME, O_RDWR, PERMISSIONS);
  if(fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  if (ftruncate(fd, sizeof(circbuf_t)) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }

  circbuf_t *buf = mmap(NULL, sizeof(circbuf_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if(buf == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  return buf;
}

circbuf_t *create_circbuf_shm() {
  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR | O_EXCL, PERMISSIONS);
  if(fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  if (ftruncate(fd, sizeof(circbuf_t)) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }

  circbuf_t *buf = mmap(NULL, sizeof(circbuf_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

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
  printf("Edges: ");
  for(int i = 0; i < count; i++) {
    edge_t e = edges[i];
    printf("%d-%d ", e.n1, e.n2);
  }
  printf("\n");
}


void print_edge(edge_t *edge) {
  printf("Edge: %d-%d\n", edge->n1, edge->n2);
}
