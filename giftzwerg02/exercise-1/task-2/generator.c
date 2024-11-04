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
#define MAX_DATA (50)

const int file_permissions = 0600;
const char newline = 10;

#define EB_FREE_SEM "/sem-eb-free"
#define EB_USED_SEM "/sem-eb-used"

struct edge_buffer {
	int state;
	char* data[MAX_DATA];
};

void write_solution(int *pos, struct edge_buffer *buf, char* val, sem_t *free, sem_t *used) {
	printf("Trying to write...\n");
	sem_wait(free);
	buf->data[*pos] = val;
	sem_post(used);
	*pos += 1;
	*pos %= sizeof(buf->data);
	printf("done writing\n");
}

void error(char* scope) {
	perror(scope);
}

int cleanup(int buffd, struct edge_buffer *buffer, sem_t *s1, sem_t *s2) {
	if(munmap(buffer, sizeof(*buffer)) == -1) {
		error("munmap");
		return EXIT_FAILURE;
	}

	if(close(buffd) == -1) {
		error("close buffd");
		return EXIT_FAILURE;
	}

	if(sem_close(s1) == -1 || sem_close(s2) == -1) {
		error("sem_close");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
	int buffd = shm_open(SHM_NAME, O_RDWR | O_CREAT, file_permissions);
	if(buffd == -1) {
		error("shm_open");
		return EXIT_FAILURE;
	}


	struct edge_buffer *buffer;
	buffer = mmap(NULL, sizeof(*buffer), PROT_READ | PROT_WRITE, MAP_SHARED, buffd, 0);
	if(buffer == MAP_FAILED) {
		error("mmap");
		return EXIT_FAILURE;
	}

	sem_t *eb_free_sem = sem_open(EB_FREE_SEM, O_CREAT, file_permissions, MAX_DATA);
	sem_t *eb_used_sem = sem_open(EB_USED_SEM, O_CREAT, file_permissions, 0);

	printf("start writing\n");
	int pos = 0;
	while(0 == 0) {
		char text = 'a';
		write_solution(&pos, buffer, &text, eb_free_sem, eb_used_sem);
		sleep(1);
	}

	return cleanup(buffd, buffer, eb_free_sem, eb_used_sem);
}
