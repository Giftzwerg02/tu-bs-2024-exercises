#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <sys/param.h>
#include "graph.h"

#define DEFAULT_ASSUMED_VERTEX_COUNT 256
#define MAXIMAL_INTEGER_IN_STRING_FORM_SIZE 128
#define CHECK(var) if(var == NULL) { return NULL; }

inline bool is_connected(edge_matrix_t edges, int from, int to){
    return (edges + from)[to];
}

inline void set_connected(edge_matrix_t edges, int from, int to, bool value){
    // I was so stupid that I wrote good code
    // this thing efforlessly gets us the symmetry property we are looking for
    (edges + from)[to] = value;
}

inline color_t random_color(){
    return (color_t) rand() % 3;
}

char* color_name(color_t color){
    switch(color){
        case RED: return "RED";
        case GREEN: return "GREEN";
        case BLUE: return "BLUE";
        default: assert(0);
    }
}

size_t count_edges(graph_t* graph){
    int count = 0;
    for(int i = 0; i < graph->vertex_count; i++)
        for(int j = 0; j < graph->vertex_count; j++)
            count += is_connected(graph->edges, i, j)? 1 : 0;

    return count;
}

graph_t* alloc_graph(int vertex_count){
    graph_t* graph = malloc(sizeof(graph_t));
    CHECK(graph);

    graph->vertex_count = vertex_count;

    graph->vertices = calloc(vertex_count, sizeof(color_t));
    CHECK(graph->vertices);

    graph->edges = calloc(vertex_count * 2, sizeof(bool));
    CHECK(graph->edges);

    return graph;
}

// Format 0-1 2-3 etc.
graph_t* graph_from_spec(char* spec){
    typedef struct { int a; int b; } edge_t;

    int buffer_idx = 0;
    edge_t* edge_buffer = calloc(DEFAULT_ASSUMED_VERTEX_COUNT, sizeof(edge_t));
    CHECK(edge_buffer);

    char* temp_vertex_buffer = calloc(MAXIMAL_INTEGER_IN_STRING_FORM_SIZE, sizeof(char));
    CHECK(temp_vertex_buffer);

    int largest_vertex = 0;
    int vertex_start = 0;
    edge_t edge;

    for(int i = 0; i <= strlen(spec); i++){
        switch(spec[i]){
            case ' ': case '\0':
                edge.a = atoi(temp_vertex_buffer);
                largest_vertex = MAX(edge.a, largest_vertex);

                memcpy(temp_vertex_buffer, spec + vertex_start, i - vertex_start);
                temp_vertex_buffer[i - vertex_start + 1] = '\0';

                edge.b = atoi(temp_vertex_buffer);
                largest_vertex = MAX(edge.b, largest_vertex);

                edge_buffer[buffer_idx] = edge;
                buffer_idx++;

                vertex_start = i + 1;
                break;

            case '-':
                memcpy(temp_vertex_buffer, spec + vertex_start, i - vertex_start);
                temp_vertex_buffer[i - vertex_start + 1] = '\0';
                vertex_start = i + 1;
                break;

            case '0': case '1': case '2':
            case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': break;

            default:
                return NULL;
        }
    }

    graph_t* graph = alloc_graph(largest_vertex + 1);
    CHECK(graph);

    for(int i = 0; i < buffer_idx; i++){
        edge_t edge = edge_buffer[i];
        set_connected(graph->edges, edge.a, edge.b, true);
    }

    free(temp_vertex_buffer);
    free(edge_buffer);

    return graph;
}

void graph_free(graph_t* graph){
    free(graph->edges);
    free(graph->vertices);
    free(graph);
}

void graph_print_edges(int vertex_count, edge_matrix_t edges){
    printf("Edges: \n");

    printf(" %3s |", " ");
    for(int i = 0; i < vertex_count; i++){
        printf(" %3d |", i);
    }

    for(int row = 0; row < vertex_count; row++){
        printf("\n %3d |", row);

        for(int column = 0; column < vertex_count; column++){
            char symbol = is_connected(edges, row, column)? 'x' : ' ';

            printf(" %3c |", symbol);
        }
    }
}

void graph_print(graph_t* graph){
    graph_print_edges(graph->vertex_count, graph->edges);

    printf("\n\nColors: \n");
    for(int i = 0; i < graph->vertex_count; i++){
        char* color = color_name(graph->vertices[i]);
        printf("%d: %s\n", i, color);
    }
}

edge_matrix_t graph_color(graph_t* graph){
    srand(time(NULL));

    // Step 1: Randomly color all vertices
    for(int i = 0; i < graph->vertex_count; i++){
        graph->vertices[i] = random_color();
    }

    // Step 2: Disconnect all vertices with identical color
    int total_edges_count = count_edges(graph);

    edge_matrix_t removed_edges = calloc(total_edges_count, sizeof(bool));
    CHECK(removed_edges);

    for(int i = 0; i < graph->vertex_count; i++){
        for(int j = 0; j < graph->vertex_count; j++){
            if(!is_connected(graph->edges, i, j)) continue;

            bool different_color = graph->vertices[i] != graph->vertices[j];
            if(different_color) continue;

            set_connected(graph->edges, i, j, false);
            set_connected(removed_edges, i, j, true);
        }
    }

    return removed_edges;
}