#include "./lib.h"

BathroomMonitor *new_bathroom_monitor(unsigned int size) {
    if (size == 0) {
        perror("number of bathrooms on monitor should be greater than 0!\n");
        exit(1);
    }
    BathroomMonitor *monitor = (BathroomMonitor *) malloc(sizeof(BathroomMonitor));
    monitor->occupied_by = None;
    monitor->size = size;
    pthread_cond_init(&monitor->can_flamenguista_enter, NULL);
    pthread_cond_init(&monitor->can_vascaino_enter, NULL);
    pthread_mutex_init(&monitor->lock, NULL);
    sem_init(&monitor->available_bathrooms, 0, size);
    return monitor;
}

void drop_bathroom_monitor(BathroomMonitor *monitor) {
    sem_destroy(&monitor->available_bathrooms);
    free(monitor);
}

void flamenguista_wants_in(BathroomMonitor *monitor) {
    pthread_mutex_lock(&monitor->lock);
    switch (monitor->occupied_by) {
        case Flamengo:
            pthread_mutex_unlock(&monitor->lock);
            sem_wait(&monitor->available_bathrooms);
            break;
        case Vasco:
            pthread_cond_wait(&monitor->can_flamenguista_enter, &monitor->lock);
            pthread_mutex_unlock(&monitor->lock);
            vascaino_wants_in(monitor);
            break;
        case None:
            monitor->occupied_by = Flamengo;
            sem_wait(&monitor->available_bathrooms);
            pthread_mutex_unlock(&monitor->lock);
            break;
    }
}

void flamenguista_goes_out(BathroomMonitor *monitor) {
    pthread_mutex_lock(&monitor->lock);
    if (monitor->occupied_by == Flamengo) {
        sem_post(&monitor->available_bathrooms);
        int sem_val;
        sem_getvalue(&monitor->available_bathrooms, &sem_val);
        if (sem_val == monitor->size) {
            monitor->occupied_by = None;
            pthread_cond_broadcast(&monitor->can_vascaino_enter);
        }
    }

    pthread_mutex_unlock(&monitor->lock);
}


void vascaino_goes_out(BathroomMonitor *monitor) {
    pthread_mutex_lock(&monitor->lock);
    if (monitor->occupied_by == Vasco) {
        sem_post(&monitor->available_bathrooms);
        int sem_val;
        sem_getvalue(&monitor->available_bathrooms, &sem_val);
        if (sem_val == monitor->size) {
            monitor->occupied_by = None;
            pthread_cond_broadcast(&monitor->can_flamenguista_enter);
        }
    }
    pthread_mutex_unlock(&monitor->lock);
}

void vascaino_wants_in(BathroomMonitor *monitor) {
    pthread_mutex_lock(&monitor->lock);
    switch (monitor->occupied_by) {
        case Flamengo:
            pthread_cond_wait(&monitor->can_vascaino_enter, &monitor->lock);
            pthread_mutex_unlock(&monitor->lock);
            vascaino_wants_in(monitor);
            break;
        case Vasco:
            pthread_mutex_unlock(&monitor->lock);
            sem_wait(&monitor->available_bathrooms);
            break;
        case None:
            monitor->occupied_by = Vasco;
            sem_wait(&monitor->available_bathrooms);
            pthread_mutex_unlock(&monitor->lock);
            break;
    }
}
