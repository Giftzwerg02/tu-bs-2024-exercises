#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

#include "lib.h"

graph read_solution(graph_buffer *buf, sem_t *free, sem_t *used) {
	sem_wait(used);

	graph res = pop(buf);

	sem_post(free);

	return res;
}

int cleanup(int buffd, graph_buffer *buffer, sem_t *s1, sem_t *s2) {
	if(munmap(buffer, sizeof(*buffer)) == -1) {
		error("munmap");
		return EXIT_FAILURE;
	}

	if(close(buffd) == -1) {
		error("buffd");
		return EXIT_FAILURE;
	}

	if(shm_unlink(SHM_NAME) == -1) {
		error("shm_unlink");
		return EXIT_FAILURE;
	}

	if(sem_close(s1) == -1 || sem_close(s2) == -1) {
		error("sem_close");
		return EXIT_FAILURE;
	}

	if(sem_unlink(EB_USED_SEM) == -1 || sem_unlink(EB_FREE_SEM) == -1) {
		error("sem_unlink");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
	debug("%s", "start supervisor\n");

	char c;
	bool has_limit = false;
	int limit;
	while((c = getopt(argc, argv, "n:w:")) != -1) {
		switch (c) {
			case 'n':
				has_limit = true;
				limit = (int) strtol(optarg, NULL, 10);
				debug("limit set: %d", limit);
				break;
			case 'w':
				break;
			case '?':
				return EXIT_FAILURE;
			default:
				return EXIT_FAILURE;
		}
	}

	int buffd = shm_open(SHM_NAME, O_RDWR | O_CREAT, file_permissions);
	if(buffd == -1) {
		error("shm_open");
		return EXIT_FAILURE;
	}

	if(ftruncate(buffd, sizeof(graph_buffer)) < 0) {
		error("ftruncate");
		return EXIT_FAILURE;
	}

	graph_buffer *buffer;
	buffer = mmap(NULL, sizeof(*buffer), PROT_READ | PROT_WRITE, MAP_SHARED, buffd, 0);
	if(buffer == MAP_FAILED) {
		error("mmap");
		return EXIT_FAILURE;
	}

	sem_t *eb_free_sem = sem_open(EB_FREE_SEM, O_CREAT, file_permissions, MAX_BUFFER_SIZE);
	sem_t *eb_used_sem = sem_open(EB_USED_SEM, O_CREAT, file_permissions, 0);

	if(eb_free_sem == SEM_FAILED || eb_used_sem == SEM_FAILED) {
		error("sem_open");
		return EXIT_FAILURE;
	}


	debug("%s", "start reading\n");
	int best_count = INT_MAX;
	int n = 0;
	while(!has_limit || n++ < limit) {
		debug("supervisor iteration: %d", n);
		graph val = read_solution(buffer, eb_free_sem, eb_used_sem);
		debug_graph(&val);
		if(val.count < best_count) {
			best_count = val.count;
			print_solution(&val, stdout);
		}

		if(best_count == 0) {
			break;
		}
	}

	if(best_count > 0) {
		fprintf(stdout, "The graph might not be 3-colorable, best solution removes %d edges.\n", best_count);
		fflush(stdout);
	}

	return cleanup(buffd, buffer, eb_free_sem, eb_used_sem);
}
