#include <iostream>
#include<sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;
sem_t *sem;

int main() {
	int numWakeup;
	int i;
	sem = sem_open("Lab04_Semaphore", 0);
	cout << "PID: " << getpid() << endl;
	do {
		cout << "How many threads do you want to wake up(enter '0' to exit.)?"
				<< endl;
		cin >> numWakeup;
		for (i = 0; i < numWakeup; i++) {
			sem_post(sem);
		}
	} while (numWakeup != 0);
	sem_close(sem);
	return 0;
}
