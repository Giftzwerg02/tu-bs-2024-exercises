#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef DEBUG
#define debug(format, ...)                                                     \
  fprintf(stderr, "%s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(format, ...)
#endif

char *strtolower(char *str) {
  char *lower = malloc(strlen(str) + 1);
  for (int i = 0; str[i]; i++) {
    lower[i] = tolower(str[i]);
  }
  return lower;
}

void print_usage(void) {
  printf("Ayo bro you ain't using this shit right.\n");
  printf("mygrep [-i] [-o outfile] keyword [file...]\n");
  printf("Options:\n");
  printf("> -i (case insensitive)\n");
  printf("> -o [outfile] (output file)\n");
}

void print_err_file_does_not_exist(void) {
	fprintf(stdout, "error: file does not exist\n");
}

void print_err_file_cannot_be_read(void) {
	fprintf(stdout, "error: file cannot be read\n");
}

void print_err_file_cannot_be_written_to(void) {
	fprintf(stdout, "error: file cannot be written to\n");
}

void grep_file(FILE *fp, char *keyword, bool is_case_insensitive, FILE *out) {
  char *line;
  size_t len = 0;
  ssize_t read;
  if (fp == NULL) {
    exit(EXIT_FAILURE);
  }

  if (is_case_insensitive) {
    while ((read = getline(&line, &len, fp)) != -1) {
      if (strstr(strtolower(line), keyword) != NULL) {
        fprintf(out, "%s", line);
      }
    }
  } else {
    while ((read = getline(&line, &len, fp)) != -1) {
      if (strstr(line, keyword) != NULL) {
        fprintf(out, "%s", line);
      }
    }
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage();
    return EXIT_FAILURE;
  }

  char c;
  bool is_case_insensitive = false;
  FILE *outfile = stdout;

  bool opt_i_already_set = false;
  bool opt_o_already_set = false;

  while ((c = getopt(argc, argv, "io:")) != -1) {
    switch (c) {
    case 'i':
      if (opt_i_already_set) {
        print_usage();
        return EXIT_FAILURE;
      }
      is_case_insensitive = true;
      opt_i_already_set = true;
      debug("%s", "enabled case insensitivity");
      break;
    case 'o':
      if (opt_o_already_set) {
        print_usage();
        return EXIT_FAILURE;
      }
      outfile = fopen(optarg, "w");
	  if(access(optarg, W_OK) != 0) {
		print_err_file_cannot_be_written_to();
        return EXIT_FAILURE;
	  }
      opt_o_already_set = true;
      break;
    case '?':
      print_usage();
      return EXIT_FAILURE;
    default:
      printf("Holy shit something just got nuked. (char: %c)", c);
      assert(0);
      break;
    }
  }

  char *keyword = argv[optind];
  debug("keyword: %s", keyword);

  const int file_count = argc - optind - 1;
  debug("File count: %d", file_count);

  char **files;
  files = &argv[optind + 1];

  if (is_case_insensitive) {
    keyword = strtolower(keyword);
  }

  if (file_count == 0) {
    grep_file(stdin, keyword, is_case_insensitive, outfile);
  } else {
    for (int i = 0; i < file_count; i++) {
	  if(access(files[i], F_OK) != 0) {
		print_err_file_does_not_exist();
		return EXIT_FAILURE;
	  }
	  if(access(files[i], R_OK) != 0) {
		print_err_file_cannot_be_read();
		return EXIT_FAILURE;
	  }
      FILE *fp = fopen(files[i], "r");
      grep_file(fp, keyword, is_case_insensitive, outfile);
      if (fclose(fp) != 0) {
        return EXIT_FAILURE;
      }
    }
  }

  return EXIT_SUCCESS;
}
