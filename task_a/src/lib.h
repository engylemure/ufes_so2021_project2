#ifndef LIB_H
#define LIB_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

/**
 * @brief enumeration to describe the available teams
 * 
 */
enum TeamOnBathroom {
    None,
    Flamengo,
    Vasco,
};

/**
 * @brief structure to describe the bathroom monitor object
 * 
 */
typedef struct bathroomMonitor {
    enum TeamOnBathroom occupied_by;
    unsigned int size;
    sem_t available_bathrooms;
    pthread_mutex_t lock;
    pthread_cond_t can_flamenguista_enter;
    pthread_cond_t can_vascaino_enter;
} BathroomMonitor;

/**
 * @brief Creates new BathroomMonitor object
 * 
 * @return BathroomMonitor* 
 */
BathroomMonitor *new_bathroom_monitor(unsigned int size);

void drop_bathroom_monitor(BathroomMonitor* monitor);

void flamenguista_wants_in(BathroomMonitor* monitor);

void flamenguista_goes_out(BathroomMonitor* monitor);

void vascaino_goes_out(BathroomMonitor *monitor);

void vascaino_wants_in(BathroomMonitor* monitor);


#endif