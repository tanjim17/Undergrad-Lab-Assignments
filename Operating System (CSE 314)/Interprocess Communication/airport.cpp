#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>

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
int channel_forward_count;
int channel_backward_count;
int m, n, p; //kiosk count, belt count, no. of psngr each belt can serve
int w, x, y, z; //kiosk time, belt time, boarding time, channel time
default_random_engine generator;
bernoulli_distribution bernouli_dist_for_pass_loss(0.3);
chrono::time_point<chrono::steady_clock> start;

int get_time() {
	chrono::duration<double> end = chrono::steady_clock::now() - start;
	return (int)end.count();
}

void get_boarding_pass(int id, const char* vip_tag) {
	printf("Passenger %d%s has arrived at the airport at time %d\n", id, vip_tag, get_time());
	int curr_kiosk_idx;
	sem_wait(&kiosk_empty);
	pthread_mutex_lock(&kiosk_mtx);
	for (int i = 0; i < kiosk_stats.size(); i++) {
		if (!kiosk_stats[i]) {
			kiosk_stats[i] = true;
			curr_kiosk_idx = i;
			break;
		}
	}
	pthread_mutex_unlock(&kiosk_mtx);
	printf("Passenger %d%s has started self check-in at kiosk %d at time %d\n", id, vip_tag, curr_kiosk_idx, get_time());
	sleep(w);
	printf("Passenger %d%s has finished check-in at time %d\n", id, vip_tag, get_time());
	pthread_mutex_lock(&kiosk_mtx);
	kiosk_stats[curr_kiosk_idx] = false;
	pthread_mutex_unlock(&kiosk_mtx);
	sem_post(&kiosk_empty);
}

void security_check(int id, const char* vip_tag) {
	int belt_idx = rand() % belts_empty.size();
	printf("Passenger %d%s has started waiting for security check in belt %d from time %d\n", id, vip_tag, belt_idx, get_time());
	sem_wait(&belts_empty[belt_idx]);
	printf("Passenger %d%s has started security check at time %d\n", id, vip_tag, get_time());
	sleep(x);
	printf("Passenger %d%s has crossed the security check at %d\n", id, vip_tag, get_time());
	sem_post(&belts_empty[belt_idx]);
}

bool board_plane(int id, const char* vip_tag) {
	printf("Passenger %d%s has started waiting to be boarded at time %d\n", id, vip_tag, get_time());
	if (bernouli_dist_for_pass_loss(generator)) {
		printf("Passenger %d%s has lost boarding pass!\n", id, vip_tag);
		sleep(1);
		return false;
	}
	pthread_mutex_lock(&board_mtx);
	printf("Passenger %d%s has started boarding the plane at time %d\n", id, vip_tag, get_time());
	sleep(y);
	printf("Passenger %d%s has boarded the plane at time %d\n", id, vip_tag, get_time());
	pthread_mutex_unlock(&board_mtx);
	return true;
}

void use_channel_forward(int id, const char* vip_tag) {
	printf("Passenger %d%s has started wating to cross VIP channel at time %d\n", id, vip_tag, get_time());
	pthread_mutex_lock(&channel_forward_mtx);
	channel_forward_count++;
	if (channel_forward_count == 1) {
		pthread_mutex_lock(&vip_mtx);
		pthread_mutex_lock(&channel_mtx);
	}
	pthread_mutex_unlock(&channel_forward_mtx);
	printf("Passenger %d%s has entered the VIP channel at time %d\n", id, vip_tag, get_time());
	sleep(z);
	printf("Passenger %d%s has crossed the VIP channel at time %d\n", id, vip_tag, get_time());
	pthread_mutex_lock(&channel_forward_mtx);
	channel_forward_count--;
	if (channel_forward_count == 0) {
		pthread_mutex_unlock(&vip_mtx);
		pthread_mutex_unlock(&channel_mtx);
	}
	pthread_mutex_unlock(&channel_forward_mtx);
}

void use_channel_backward(int id, const char* vip_tag) {
	printf("Passenger %d%s has started wating to cross VIP channel (backward) at time %d\n", id, vip_tag, get_time());
	pthread_mutex_lock(&vip_mtx);
	pthread_mutex_unlock(&vip_mtx);
	pthread_mutex_lock(&channel_backward_mtx);
	channel_backward_count++;
	if (channel_backward_count == 1) {
		pthread_mutex_lock(&channel_mtx);
	}
	pthread_mutex_unlock(&channel_backward_mtx);
	printf("Passenger %d%s has entered the VIP channel (backward) at time %d\n", id, vip_tag, get_time());
	sleep(z);
	printf("Passenger %d%s has crossed the VIP channel (backward) at time %d\n", id, vip_tag, get_time());
	pthread_mutex_lock(&channel_backward_mtx);
	channel_backward_count--;
	if (channel_backward_count == 0) {
		pthread_mutex_unlock(&channel_mtx);
	}
	pthread_mutex_unlock(&channel_backward_mtx);
}

void get_special_pass(int id, const char* vip_tag) {
	printf("Passenger %d%s has arrived at special kiosk at time %d\n", id, vip_tag, get_time());
	pthread_mutex_lock(&special_kiosk_mtx);
	printf("Passenger %d%s has started self check-in at special kiosk at time %d\n", id, vip_tag, get_time());
	sleep(w);
	printf("Passenger %d%s has finished check-in at special kiosk at %d\n", id, vip_tag, get_time());
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

int main() {
	ifstream fin("input.txt");
	fin >> m >> n >> p;
	fin >> w >> x >> y >> z;

	start = chrono::steady_clock::now();
	channel_forward_count = 0;
	channel_backward_count = 0;

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

	int psngr_count = 10;
	int id = 0;
	exponential_distribution<double> exp_dist(1);
	bernoulli_distribution bernouli_dist_for_vip(0.5);
	random_device rd;
 	mt19937 rng(rd());
	for (int i = 0; i < psngr_count; i++) {
		double inter_arrival_time =  exp_dist.operator()(rng);
		sleep(inter_arrival_time);
		pair<int, bool>* psngr_info = new pair<int, bool>({id++, bernouli_dist_for_vip(generator)});
		pthread_create(new pthread_t, NULL, psngr_activity, (void*)psngr_info);
	}

	pthread_exit(NULL);
	return 0;
}
