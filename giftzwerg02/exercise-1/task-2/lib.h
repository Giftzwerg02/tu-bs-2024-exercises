#ifndef LIB_H

#define LIB_H


#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>

#define SHM_NAME "/edge-buffer"
#define MAX_DATA (500)
#define MAX_BUFFER_SIZE (10)

extern const int file_permissions;

#define EB_FREE_SEM "/sem-eb-free"
#define EB_USED_SEM "/sem-eb-used"
#define EB_MUTEX_SEM "/sem-eb-mutex"

#define EDGES_DEL " "
#define NODE_DEL "-"

typedef enum {
	NONE,
	RED,
	GREEN,
	BLUE,	
} color;

typedef struct {
	int id;
	color col;
} node;

typedef struct {
	node start;
	node end;
} edge;

typedef struct {
	int count;	
	edge edges[MAX_DATA];
} graph;

typedef struct {
	int read_pos;
	int write_pos;
	graph data[MAX_BUFFER_SIZE];
} graph_buffer;


void error(char* scope);

void push(graph_buffer* buf, graph* val);
graph pop(graph_buffer* buf);

graph *parse_graph(char **edges_str, int edges_count);
void debug_graph(graph *graph);
void debug_edge(edge *edge);
void debug_node(node *node);

void color_graph(graph *graph);
color random_col(void);
graph *remove_eq_col_edges(graph *graph);
void print_solution(graph *g, FILE *fp);

#ifdef DEBUG
#define debug(format, ...)                                                     \
  fprintf(stderr, "%s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(format, ...)
#endif

#endif
