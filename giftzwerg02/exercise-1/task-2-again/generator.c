#include "lib.h"
#include "unistd.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  RED = 0,
  GREEN = 1,
  BLUE = 2,
} col_t;

graph_t parseargs(int argc, char **argv);

col_t *coloring(int max_id);

solution_t get_solution(graph_t *graph, col_t *cols);

void write_solution(circbuf_t *buf, solution_t sol, sem_t *sr, sem_t *sw,
                    sem_t *sx);

int main(int argc, char **argv) {
  graph_t g = parseargs(argc, argv);
  circbuf_t *buf = open_circbuf_shm();

  sem_t *sem_read = open_sem(SEM_NAME_READ);
  sem_t *sem_write = open_sem(SEM_NAME_WRITE);
  sem_t *sem_xor = open_sem(SEM_NAME_XOR);

  while (buf->stop != 1) {
    col_t *cols = coloring(g.max_id);
    solution_t sol = get_solution(&g, cols);
    write_solution(buf, sol, sem_read, sem_write, sem_xor);
    sleep(1);
  }

  exit(EXIT_SUCCESS);
}

int max(int a, int b, int c) {
  int m = a;
  if (m < b) {
    m = b;
  }
  if (m < c) {
    m = c;
  }
  return m;
}

graph_t parseargs(int argc, char **argv) {
  edge_t *edges = malloc(sizeof(edge_t) * (argc - 1));
  int max_id = 0;
  for (int i = 1; i < argc; i++) {
    char *edge = argv[i];
    int k = 0;
    while (1) {
      if (edge[k] == '-') {
        break;
      }
      k++;
    }

    int n1Size = k;
    int n2Size = strlen(edge) - k - 1;

    char *n1 = malloc(sizeof(char) * n1Size + 1);
    char *n2 = malloc(sizeof(char) * n2Size + 1);

    for (int nidx = 0; nidx < n1Size; nidx++) {
      n1[nidx] = edge[nidx];
    }
    n1[n1Size] = '\0';

    for (int nidx = 0; nidx < n2Size; nidx++) {
      n2[nidx] = edge[nidx + k + 1];
    }
    n2[n2Size] = '\0';

    edge_t e = {
        .n1 = parsenum(n1),
        .n2 = parsenum(n2),
    };
    edges[i - 1] = e;

    max_id = max(max_id, e.n1, e.n2);
  }
  graph_t g = {
      .edges = edges,
      .count = argc - 1,
      .max_id = max_id,
  };
  return g;
}

col_t *coloring(int max_id) {
  col_t *cols = malloc(sizeof(col_t) * (max_id + 1));
  for (int i = 0; i <= max_id; i++) {
    col_t col = (random() % 3);
    cols[i] = col;
  }
  return cols;
}

solution_t get_solution(graph_t *graph, col_t *cols) {
  solution_t sol = {};

  for (int i = 0; i < graph->count; i++) {
    edge_t gedge = graph->edges[i];
    col_t c1 = cols[gedge.n1];
    col_t c2 = cols[gedge.n2];

    if (c1 == c2) {
      sol.removed_edges[sol.count] = gedge;
      sol.count++;
    }
  }

  return sol;
}

void write_solution(circbuf_t *buf, solution_t sol, sem_t *sr, sem_t *sw,
                    sem_t *sx) {
  sem_wait(sx);
  sem_wait(sw);

  write_circ(buf, &sol);

  sem_post(sr);
  sem_post(sx);
}
