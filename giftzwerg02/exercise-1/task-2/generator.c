#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>

#include "lib.h"

void write_solution(graph_buffer *buf, graph* val, sem_t *free, sem_t *used, sem_t *mutex) {
	sem_wait(mutex);
	sem_wait(free);

	push(buf, val);

	sem_post(used);
	sem_post(mutex);
}

int cleanup(int buffd, graph_buffer *buffer, sem_t *s1, sem_t *s2, sem_t *s3) {
	if(munmap(buffer, sizeof(*buffer)) == -1) {
		error("munmap");
		return EXIT_FAILURE;
	}

	if(close(buffd) == -1) {
		error("close buffd");
		return EXIT_FAILURE;
	}

	if(sem_close(s1) == -1 || sem_close(s2) == -1 || sem_close(s3)) {
		error("sem_close");
		return EXIT_FAILURE;
	}

	if(sem_unlink(EB_MUTEX_SEM) == -1) {
		error("sem_unlink");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
	srand(time(NULL));
	int buffd = shm_open(SHM_NAME, O_RDWR, file_permissions);
	if(buffd == -1) {
		error("shm_open");
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
	sem_t *eb_mutex_sem = sem_open(EB_MUTEX_SEM, O_CREAT, file_permissions, 1);

	char **edges_str = argv;
	edges_str++;

	graph *g = parse_graph(edges_str, argc - 1);

	debug("%s", "start writing\n");
	int n = 0;
	while(0 == 0) {
		n++;
		debug("generator iteration: %d", n);
		if(buffer->stop_flag) {
			break;
		}
		color_graph(g);
		debug_graph(g);
		graph *rem = remove_eq_col_edges(g);
		write_solution(buffer, rem, eb_free_sem, eb_used_sem, eb_mutex_sem);
	}

	return cleanup(buffd, buffer, eb_free_sem, eb_used_sem, eb_mutex_sem);
}
