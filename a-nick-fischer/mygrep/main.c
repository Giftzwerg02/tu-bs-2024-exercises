#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define STD_BUFFER_SIZE (1024 * 4)

typedef struct {
    int inputFilesCount;
    char** inputFiles;
    char* keyword;
    char* outputFile;
    bool caseSensitive;
    bool valid;
} ProgramOptions;

typedef char** ProgramArgs;

static ProgramOptions parseArguments(int argc, char** argv){    
    ProgramOptions options = {
        .outputFile = NULL,
        .caseSensitive = true,
        .valid = false,
    };

    int opt;
    while((opt = getopt(argc, argv, "io:")) != -1){
        switch(opt){
            case 'i':
                options.caseSensitive = false;
                break;
            case 'o':
                options.outputFile = optarg;
                break;
            case ':':
                fprintf(stderr, "No value for option %c specified\n", optopt);
                return options;
            case '?':
                fprintf(stderr, "Unknown option: %c\n", optopt);
                return options;
            default:
                assert(0);
        }
    }

    if(optind == argc){
        fprintf(stderr, "No keyword specified\n");
        return options;
    }

    options.keyword = argv[optind];
    optind++;

    if(optind == argc){
        fprintf(stderr, "No input files specified\n");
        return options;
    }

    options.inputFilesCount = argc - optind;
    options.inputFiles = &argv[optind];
    options.valid = true;

    return options;
}

static void lowerCase(char* str, int maxSize){
    for(int i = 0; str[i] != '\0' && i < maxSize; i++){
        str[i] = tolower(str[i]);
    }
}

static void searchInFile(char* filename, char* keyword, FILE* output, bool caseSensitive){
    size_t bufferSize = STD_BUFFER_SIZE;
    char* readBuffer = calloc(bufferSize);
    if(readBuffer == NULL){
        perror("Memory allocation error");
        return;
    }


    FILE* file = fopen(filename, "r");
    if(file == NULL){
        fprintf(stderr, "Could not open file %s for searching: %s\n", filename, strerror(errno));
        return;
    }

    size_t keywordLen = strlen(keyword);
    if(!caseSensitive) lowerCase(keyword, keywordLen);

    size_t readContentMaxSize = bufferSize / 2;
    char* readContent = readBuffer + readContentMaxSize;
    char* wholeContent = readContent;


    /*
     * The read Buffer
     *    Newly read content -------------------
     *                                          |
     *    Content from previous fgets           |
     *    in case buffer was too small          |
     *                      |                   |
     *    Unused            |                   |
     *       |              |                   |
     * |-----+----------|---+---|---------------+--------| 
     *       |              |                   |
     * (S / 2) - K + 1    K - 1              (S / 2)
     * 
     * S = STD_BUFFER_SIZE
     * K = Length of current keyword
     * 
     * K - 1 is used instead of K
     */
    while(fgets(readContent, readContentMaxSize, file) != NULL){
        if(!caseSensitive) lowerCase(readContent, newContentMaxSize);
        
        char* foundStr = strstr(wholeContent, keyword);
        if(foundStr != NULL) { 
            fprintf(output, "%s", wholeContent);
        }

        size_t readContentSize = strlen(readContent);
        if(newContent[newContentSize - 1] != '\n'){
            wholeContent = readContent - keywordLen - 1;
            memcopy(readContent);
        }
    }

    fclose(file);
}

int main(int argc, char* argv[]){
    ProgramOptions options = parseArguments(argc, argv);

    if(!options.valid) return EXIT_FAILURE;

    FILE* output = stdout;
    if(options.outputFile != NULL){
        output = fopen(options.outputFile, "w");
    }
    
    if(output == NULL){
        perror("Could not open output file");
        return EXIT_FAILURE;
    }

    for(int i = 0; i < options.inputFilesCount; i++)
        searchInFile(
            options.inputFiles[i], 
            options.keyword, 
            output,
            options.caseSensitive
        );

    if(options.outputFile != NULL) fclose(output);
}