// int main(void)
// {	
// 	int  kioskCount, beltCount, beltServeCount;
// 	int kioskTime, beltTime, boardingTime, vipTime;
// 	ifstream fin("input.txt");
// 	fin >> kioskCount >> beltCount >> beltServeCount;
// 	fin >> kioskTime >> beltTime >> boardingTime >> vipTime;
// }

#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CAPACITY 100

using namespace std;

sem_t kiosk_empty;
pthread_mutex_t kiosk_mtx;
vector<bool> kiosk_stats;
vector<sem_t> belts_empty;
int time_passed;

void get_boarding_pass(int id) {
	int curr_kiosk_idx;
	sem_wait(&kiosk_empty);
	pthread_mutex_lock(&kiosk_mtx);
	for (int i = 0; i < kiosk_stats.size(); i++) {
		if (!kiosk_stats[i]) {
			kiosk_stats[i] = true;
			curr_kiosk_idx = i;
			printf("Passenger %d has started waiting in kiosk %d at %d\n", id, curr_kiosk_idx, 
			time_passed);
			break;
		}
	}
	pthread_mutex_unlock(&kiosk_mtx);
	sleep(5);
	pthread_mutex_lock(&kiosk_mtx);
	kiosk_stats[curr_kiosk_idx] = false;
	printf("Passenger %d has got his boarding pass at %d\n", id, time_passed);
	pthread_mutex_unlock(&kiosk_mtx);
	sem_post(&kiosk_empty);
}

void security_check(int id) {
	int belt_idx = rand() % belts_empty.size();
	printf("Passenger %d has started waiting for security check in belt %d at %d\n", id, belt_idx, 
		time_passed);
	sem_wait(&belts_empty[belt_idx]);
	sleep(5);
	printf("Passenger %d has crossed the security check at %d\n", id, time_passed);
	sem_post(&belts_empty[belt_idx]);
}

void* psngr_activity(void* arg) {
	int id = *((int*)arg);
	get_boarding_pass(id);
	security_check(id);
	pthread_exit(NULL);
}

void* time_count(void* arg) {
	while(1) {
		sleep(1);
		time_passed++;
	}
}

int main() {
	time_passed = 0;
	pthread_t time_thread;
	pthread_create(&time_thread, NULL, time_count, NULL);

	kiosk_stats.resize(5, false);
	belts_empty.resize(2);
	sem_init(&kiosk_empty, 0, 5);
	for (int i = 0; i < belts_empty.size(); i++) {
		sem_init(&belts_empty[i], 0, 2);
	}
	pthread_mutex_init(&kiosk_mtx, NULL);

	default_random_engine generator;
  	poisson_distribution<int> distribution(14);
	int id = 0;
	pthread_t psngr_threads[CAPACITY];
  	while (1) {
    	int psngr_count = distribution(generator);
		cout << psngr_count << " passengers entered airport\n";
		for (int i = 0; i < psngr_count; i++) {
        	int* arg = new int(id++);
			pthread_create(&psngr_threads[id], NULL, psngr_activity, (void*)arg);
		}
		sleep(60);
  	}
	return 0;
}
