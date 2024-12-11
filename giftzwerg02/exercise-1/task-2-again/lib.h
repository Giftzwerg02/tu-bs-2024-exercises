#ifndef MY_LIB_H
#define MY_LIB_H

#include <semaphore.h>

#define BUF_SIZE 512
#define BUFENTRY_SIZE 8

#define SHM_NAME "shmname"
#define SEM_NAME_XOR "semxor"
#define SEM_NAME_READ "semread"
#define SEM_NAME_WRITE "semwrite"

// actual devil
#define PERMISSIONS 0666

typedef struct {
  int n1, n2;
} edge_t;

typedef struct {
  edge_t removed_edges[BUFENTRY_SIZE];
  int count;
} solution_t;

typedef struct {
  edge_t *edges;
  int count;
  int max_id;
} graph_t;

typedef struct {
  solution_t solutions[BUF_SIZE];
  int count;
  int write_idx;
  int read_idx;
  int stop;
} circbuf_t;

solution_t read_circ(circbuf_t *cirbuf);
void write_circ(circbuf_t *cirbuf, solution_t *data);

circbuf_t *open_circbuf_shm(int *fd);
circbuf_t *create_circbuf_shm(int *fd);
void close_circbuf_shm(int fd, circbuf_t *buf);
void unlink_circbuf_shm();

sem_t *create_sem(char *name, int init_val);
sem_t *open_sem(char *name);

void close_sem(sem_t *sem);
void unlink_sem(char *name);

void usage(void);
void err(char *msg);
int parsenum(char *str);
void print_edges(edge_t *edges, int count);
void print_edge(edge_t *edge);

#endif
