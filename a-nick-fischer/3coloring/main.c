#include "graph.h"

int main(int argc, char* argv[]){
    graph_t* graph = graph_from_spec("0-1 2-1");

    graph_color(graph);

    graph_print(graph);

    graph_free(graph);

    return 0;
}