#include <stdbool.h>

typedef enum {
    RED,
    GREEN,
    BLUE
} color_t;

typedef bool* edge_matrix_t;

typedef struct {
    int vertex_count;
    color_t* vertices;

    // Two-dimensional array, stored as one in-memory
    edge_matrix_t edges;
} graph_t;


graph_t* graph_from_spec(char* spec);

void graph_free(graph_t* graph);

void graph_print(graph_t* graph);

edge_matrix_t graph_color(graph_t* graph);