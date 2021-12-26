#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

sem_t kiosk_empty;
pthread_mutex_t kiosk_mtx;
pthread_mutex_t board_mtx;
pthread_mutex_t channel_mtx;
pthread_mutex_t channel_forward_mtx;
pthread_mutex_t channel_backward_mtx;
pthread_mutex_t vip_mtx;
pthread_mutex_t special_kiosk_mtx;
vector<bool> kiosk_stats;
vector<sem_t> belts_empty;
int time_passed;
int channel_forward_count;
int channel_backward_count;
int m, n, p; //kiosk count, belt count, no. of psngr each belt can serve
int w, x, y, z; //kiosk time, belt time, boarding time, channel time
default_random_engine generator;
bernoulli_distribution bernouli_dist_for_pass_loss(0.3);

void get_boarding_pass(int id, const char* vip_tag) {
	int curr_kiosk_idx;
	sem_wait(&kiosk_empty);
	pthread_mutex_lock(&kiosk_mtx);
	for (int i = 0; i < kiosk_stats.size(); i++) {
		if (!kiosk_stats[i]) {
			kiosk_stats[i] = true;
			curr_kiosk_idx = i;
			printf("Passenger %d%s has started waiting in kiosk %d at %d\n", id,
				vip_tag, curr_kiosk_idx, time_passed);
			break;
		}
	}
	pthread_mutex_unlock(&kiosk_mtx);
	sleep(w);
	pthread_mutex_lock(&kiosk_mtx);
	kiosk_stats[curr_kiosk_idx] = false;
	printf("Passenger %d%s has got his boarding pass at %d\n", id, vip_tag, time_passed);
	pthread_mutex_unlock(&kiosk_mtx);
	sem_post(&kiosk_empty);
}

void security_check(int id, const char* vip_tag) {
	int belt_idx = rand() % belts_empty.size();
	sem_wait(&belts_empty[belt_idx]);
	printf("Passenger %d%s has started waiting for security check in belt %d at %d\n", id,
		vip_tag, belt_idx, time_passed);
	sleep(x);
	printf("Passenger %d%s has crossed the security check at %d\n", id, vip_tag, time_passed);
	sem_post(&belts_empty[belt_idx]);
}

bool board_plane(int id, const char* vip_tag) {
	if (bernouli_dist_for_pass_loss(generator)) {
		printf("Passenger %d%s has lost his boarding pass!\n", id, vip_tag);
		return false;
	}
	printf("Passenger %d%s has started waiting for boarding at %d\n", id, vip_tag, time_passed);
	pthread_mutex_lock(&board_mtx);
	sleep(y);
	printf("Passenger %d%s has boarded the plane at %d\n", id, vip_tag, time_passed);
	pthread_mutex_unlock(&board_mtx);
	return true;
}

void use_channel_forward(int id, const char* vip_tag) {
	printf("Passenger %d%s has arrived at VIP channel at %d\n", id, vip_tag, time_passed);
	pthread_mutex_lock(&channel_forward_mtx);
	channel_forward_count++;
	if (channel_forward_count == 1) {
		pthread_mutex_lock(&vip_mtx);
		pthread_mutex_lock(&channel_mtx);
	}
	pthread_mutex_unlock(&channel_forward_mtx);
	sleep(z);
	printf("Passenger %d%s has crossed the VIP channel at %d\n", id, vip_tag, time_passed);
	pthread_mutex_lock(&channel_forward_mtx);
	channel_forward_count--;
	if (channel_forward_count == 0) {
		pthread_mutex_unlock(&vip_mtx);
		pthread_mutex_unlock(&channel_mtx);
	}
	pthread_mutex_unlock(&channel_forward_mtx);
}

void use_channel_backward(int id, const char* vip_tag) {
	printf("Passenger %d%s has arrived at VIP channel (backward) at %d\n", id, vip_tag, time_passed);
	pthread_mutex_lock(&vip_mtx);
	pthread_mutex_unlock(&vip_mtx);
	pthread_mutex_lock(&channel_backward_mtx);
	channel_backward_count++;
	if (channel_backward_count == 1) {
		pthread_mutex_lock(&channel_mtx);
	}
	pthread_mutex_unlock(&channel_backward_mtx);
	sleep(z);
	printf("Passenger %d%s has crossed the VIP channel (backward) at %d\n", id, vip_tag, time_passed);
	pthread_mutex_lock(&channel_backward_mtx);
	channel_backward_count--;
	if (channel_backward_count == 0) {
		pthread_mutex_unlock(&channel_mtx);
	}
	pthread_mutex_unlock(&channel_backward_mtx);
}

void get_special_pass(int id, const char* vip_tag) {
	printf("Passenger %d%s has started waiting in special kiosk at %d\n", id, vip_tag, time_passed);
	pthread_mutex_lock(&special_kiosk_mtx);
	sleep(w);
	printf("Passenger %d%s has got his special pass at %d\n", id, vip_tag, time_passed);
	pthread_mutex_unlock(&special_kiosk_mtx);
}

void* psngr_activity(void* arg) {
	pair<int, bool>* psngr_info = (pair<int, bool>*) arg;
	int id = psngr_info->first;
	bool is_vip = psngr_info->second;
	const char* vip_tag = "";
	if (is_vip) vip_tag = " (VIP)";
	get_boarding_pass(id, vip_tag);
	if (is_vip)
		use_channel_forward(id, vip_tag);
	else
		security_check(id, vip_tag);
	while (!board_plane(id, vip_tag)) {
		use_channel_backward(id, vip_tag);
		get_special_pass(id, vip_tag);
		use_channel_forward(id, vip_tag);
	}
	pthread_exit(NULL);
}

void* time_count(void* arg) {
	while (1) {
		sleep(1);
		time_passed++;
	}
}

int main() {	
	ifstream fin("input.txt");
	fin >> m >> n >> p;
	fin >> w >> x >> y >> z;

	time_passed = 0;
	channel_forward_count = 0;
	channel_backward_count = 0;
	pthread_t time_thread;
	pthread_create(&time_thread, NULL, time_count, NULL);

	kiosk_stats.resize(m, false);
	belts_empty.resize(n);
	sem_init(&kiosk_empty, 0, m);
	for (int i = 0; i < belts_empty.size(); i++) {
		sem_init(&belts_empty[i], 0, p);
	}
	pthread_mutex_init(&kiosk_mtx, NULL);
	pthread_mutex_init(&board_mtx, NULL);
	pthread_mutex_init(&channel_mtx, NULL);
	pthread_mutex_init(&channel_forward_mtx, NULL);
	pthread_mutex_init(&channel_backward_mtx, NULL);
	pthread_mutex_init(&vip_mtx, NULL);
	pthread_mutex_init(&special_kiosk_mtx, NULL);

  	poisson_distribution<int> poisson_dist(14);
	bernoulli_distribution bernouli_dist_for_vip(0.2);
	int id = 0;
  	while (1) {
    	int psngr_count = poisson_dist(generator);
		printf("%d passengers entered airport at %d\n", psngr_count, time_passed);
		for (int i = 0; i < psngr_count; i++) {
			pair<int, bool>* psngr_info = new pair<int, bool>({id++, bernouli_dist_for_vip(generator)});
			pthread_create(new pthread_t, NULL, psngr_activity, (void*)psngr_info);
		}
		sleep(60);
  	}
	return 0;
}
