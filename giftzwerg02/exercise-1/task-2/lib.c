#include "lib.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

const int file_permissions = 0600;

void error(char *scope) { perror(scope); }

void push(graph_buffer *buf, graph*val) {
  debug("writing at: %d\n", buf->write_pos);
  buf->data[buf->write_pos] = *val;
  buf->write_pos += 1;
  buf->write_pos %= MAX_BUFFER_SIZE;
}

graph pop(graph_buffer *buf) {
  graph res = buf->data[buf->read_pos];
  buf->read_pos += 1;
  buf->read_pos %= MAX_BUFFER_SIZE;
  return res;
}

graph *parse_graph(char **edges_str, int edges_count) {
  debug("trying to parse graph of size: %d", edges_count);
  graph *g = malloc(sizeof(graph));

  for (int i = 0; i < edges_count; i++) {
	debug("parsing edge: %s", edges_str[i]);
    node nodes[2];
    int k = 0;
    char *rest = edges_str[i];
    char *token;

    while ((token = strtok_r(rest, NODE_DEL, &rest)) != NULL) {
      int node_id = strtol(token, NULL, 10);
      node n = {.id = node_id, .col = NONE};
	  debug_node(&n);
      nodes[k] = n;
      k++;
    }

    edge e = {
        .start = nodes[0],
        .end = nodes[1],
    };

    g->edges[i] = e;
  }

  g->count = edges_count;

  return g;
}

void color_graph(graph *g) {
	int node_size = g->count * 2;
	color colors[node_size];
	for (int i = 0; i < node_size; i++) {
		colors[i] = random_col();
	}

	for(int i = 0; i < g->count; i++) {
		edge *e = &g->edges[i];
		e->start.col = colors[e->start.id % node_size];
		e->end.col = colors[e->end.id % node_size];
	}
}

color random_col(void) {
	color c = 1 + rand() % 3;
	return c;
}

graph *remove_eq_col_edges(graph *g) {
	graph *removed_graph = malloc(sizeof(graph));
	int k = 0;

	for(int i = 0; i < g->count; i++) {
		edge e = g->edges[i];
		if(e.start.col == e.end.col) {
			debug("%s", "found invalid edge: ");
			debug_edge(&e);
			removed_graph->edges[k] = e;
			k++;
		}
	}

	removed_graph->count = k;
	return removed_graph;
}

void stop_generators(graph_buffer *buf) {
	buf->stop_flag = true;
}

void print_solution(graph *g, FILE *fp) {
	if(g->count == 0) {
		fprintf(fp, "The graph is 3-colorable!\n");
		fflush(fp);
		return;
	}

	fprintf(fp, "Solution with %d edges:", g->count);
	for(int i = 0; i < g->count; i++) {
		fprintf(fp, " %d-%d", g->edges[i].start.id, g->edges[i].end.id);
	}
	fprintf(fp, "\n");
	fflush(fp);
}

void debug_graph(graph *graph) {
  debug("%s", "Graph Debug:\n");
  debug("Number of edges: %d\n", graph->count);

  for (int i = 0; i < graph->count; i++) {
    debug("Edge %d:\n", i);
    debug_edge(&graph->edges[i]);
	debug("%s", "---\n");
  }
}

void debug_edge(edge *edge) {
  debug_node(&edge->start);
  debug_node(&edge->end);
}

void debug_node(node *node) {
  debug("    Node(id: %d, col: %d)", node->id, node->col);
}
