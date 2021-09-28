#ifndef LIB_H
#define LIB_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>

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
    enum TeamOnBathroom prioritized_team;
    unsigned int size;
    unsigned int flamenguistas_waiting;
    unsigned int vascainos_waiting;
    pthread_mutex_t lock;
    unsigned int amount_on_bathroom;
    unsigned int amount_of_fans_that_used_the_bathroom;
    unsigned int occupied_by_used_times;
    pthread_cond_t is_bathroom_available;
    pthread_cond_t can_flamenguista_enter;
    pthread_cond_t can_vascaino_enter;
} BathroomMonitor;

typedef struct bathroomMonitorInfo {
    enum TeamOnBathroom occupied_by;
    enum TeamOnBathroom prioritized_team;
    unsigned int amount_on_bathroom;
    unsigned int amount_of_fans_that_used_the_bathroom;
    unsigned int flamenguistas_waiting;
    unsigned int vascainos_waiting;
    unsigned int occupied_by_used_times;
} BathroomMonitorInfo;

/**
 * @brief Creates new BathroomMonitor object
 * 
 * @return BathroomMonitor* 
 */
BathroomMonitor *new_bathroom_monitor(unsigned int size);
BathroomMonitorInfo* bathroom_monitor_info(BathroomMonitor  *monitor);
char* bathroom_monitor_fmt(BathroomMonitor* monitor);
const char* team_on_bathroom_str(enum TeamOnBathroom value);
void drop_bathroom_monitor(BathroomMonitor* monitor);

/**
 * @brief function used by a Flamenguista to enter into the bathroom
 * this will cause the thread to be blocked if either the bathroom is full,
 * the other team is currently ocuppying the bathroom or the bathroom is 
 * occupied by Flamengo and the currently prioritized team is Vasco
 * @param monitor 
 */
void flamenguista_wants_in(BathroomMonitor* monitor);

/**
 * @brief function used by a Flamenguista to goes out of the bathroom
 * this will update the current stored information and it will exit the current
 * thread or process if the current occupying team on the bathroom is not Flamengo
 * 
 * @param monitor 
 */
void flamenguista_goes_out(BathroomMonitor* monitor);

/**
 * @brief function used by a Vascaino to enter into the bathroom
 * this will cause the thread to be blocked if either the bathroom is full,
 * the other team is currently ocuppying the bathroom or the bathroom is 
 * occupied by Vasco and the currently prioritized team is Flamengo
 * @param monitor 
 */
void vascaino_wants_in(BathroomMonitor* monitor);

/**
 * @brief function used by a Vascaino to goes out of the bathroom
 * this will update the current stored information and it will exit the current
 * thread or process if the current occupying team on the bathroom is not Vasco
 * 
 * @param monitor 
 */
void vascaino_goes_out(BathroomMonitor *monitor);

#endif