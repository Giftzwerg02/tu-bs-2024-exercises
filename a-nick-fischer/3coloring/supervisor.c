#include <stdio.h>
#include <getopt.h>
#include <cstdlib>

typedef struct {
    int limit;
    int delay;
} supervisor_options_t;

supervisor_options_t read_options(int argc, char* argv){
    supervisor_options_t options = {
        .limit = -1,
        .delay = 0
    };
    
    char flag;
    while((flag = getopt(argc, argv, "n:w:")) != NULL){
        switch(flag){
            case 'n':
                options.limit = atoi(optarg);
                break;
            case 'w':
                options.delay = atoi(optarg);
                break;
            case ':':
                fprintf(stderr, "No value for option %c specified\n", optopt);
                exit(EXIT_FAILURE);
                break;
            case '?':
                fprintf(stderr, "Unknown option -%s\n", optopt);
                exit(EXIT_FAILURE);
                break;
            default:
                assert(0);
        }
    }
}

int main(int argc, char* argv[]){
    supervisor_options_t options = read_options();
}