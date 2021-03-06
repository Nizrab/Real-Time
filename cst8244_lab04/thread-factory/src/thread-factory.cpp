#include <iostream>
#include<sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;
volatile sig_atomic_t usr1Happened = false;
sem_t *sem;
void sigint_handler(int sig);
void* threadFunction(void *arg);

int main() {
	int i;
	int hRun = 0;
	int nThreads;
	pthread_attr_t attr;
	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		perror("sigaction");
	}

	cout << "Enter the number of Child Threads: ";
	cin >> nThreads;

	sem = sem_open("Lab04_Semaphore", O_CREAT, S_IWOTH, 0);

	for (i = 0; i < nThreads; ++i) {
		pthread_attr_init(&attr);
		pthread_create(NULL, &attr, &threadFunction, NULL);
		pthread_attr_destroy(&attr);
	}
	while (usr1Happened != 1) {
	}
	i = sem_close(sem);
	printf("sem_close: %d\n", i);
	i = sem_unlink("Lab04_Semaphore");
	printf("sem_unlink: %d\n", i);

	return 0;
}
void* threadFunction(void *arg) {
	int semvalue = 1;
	printf("Child Thread %-2d created %15s %d\n", pthread_self(), "Status: ",
			sem_getvalue(sem, &semvalue));

	while (1) {
		sem_wait(sem);
		printf("Child Thread %-2d unblocked %15s %d\n", pthread_self(),
				"Status: ", sem_getvalue(sem, &semvalue));
		sleep(5);
	}
	return NULL;
}

void sigint_handler(int sig) {
	usr1Happened = 1;
	return;
}

