#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

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

char* s(char* str) {
	size_t len = strlen(str);
	char *ret = malloc(len + 1 + 1);
	strcpy(ret, str);
	ret[len] = newline;
	ret[len + 1] = 0;
	return ret;
}

void error(char *scope) {
	perror(s(scope));
}

char* read_solution(int *pos, struct edge_buffer *buf, sem_t *free, sem_t *used) {
	printf("%s", s("trying to read"));
	sem_wait(used);
	char *res = buf->data[*pos];
	char *cpy = malloc(strlen(res) + 1);
	strcpy(cpy, res);
	cpy[strlen(res) + 1] = 0;

	sem_post(free);
	*pos += 1;
	*pos %= sizeof(buf->data);
	printf("%s", s("done reading"));


	return cpy;
}

int cleanup(int buffd, struct edge_buffer *buffer, sem_t *s1, sem_t *s2) {
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
	printf("%s", s("start supervisor"));
	int buffd = shm_open(SHM_NAME, O_RDWR | O_CREAT, file_permissions);
	if(buffd == -1) {
		error("shm_open");
		return EXIT_FAILURE;
	}

	if(ftruncate(buffd, sizeof(struct edge_buffer)) < 0) {
		error("ftruncate");
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

	if(eb_free_sem == SEM_FAILED || eb_used_sem == SEM_FAILED) {
		error("sem_open");
		return EXIT_FAILURE;
	}


	printf("%s", s("start reading"));
	int pos = 0;
	for(;;) {
		char *val = read_solution(&pos, buffer, eb_free_sem, eb_used_sem);
		printf(s("i just read: %s"), val);
	}

	return cleanup(buffd, buffer, eb_free_sem, eb_used_sem);
}
