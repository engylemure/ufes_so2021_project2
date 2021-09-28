#ifndef LIB_H
#define LIB_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>

enum Animal {
    None,
    Cat,
    Dog,
};

/**
 * @brief object for the pet shop monitor
 * 
 */
typedef struct petShopMonitor {
    enum Animal animal_being_attended;
    enum Animal prioritized_animal;
    unsigned int max_pets_on_service_room;
    unsigned int pets_on_service_room;
    unsigned int dogs_waiting;
    unsigned int cats_waiting;
    sem_t inner_lock;
    sem_t can_dog_enter;
    sem_t can_cat_enter;
    sem_t is_service_room_available;
} PetShopMonitor;

typedef struct petShopMonitorInfo {

} PetShopMonitorInfo;

/**
 * @brief 
 * 
 * @param max_pets_on_service_room 
 * @return PetShopMonitor* 
 */
PetShopMonitor* new_petshop_monitor(unsigned int max_pets_on_service_room);

/**
 * @brief 
 * 
 * @param monitor 
 * @return PetShopMonitorInfo* 
 */
PetShopMonitorInfo* petshop_monitor_info(PetShopMonitor* monitor);

/**
 * @brief 
 * 
 * @param monitor 
 */
void cat_wants_service(PetShopMonitor* monitor);

/**
 * @brief 
 * 
 * @param monitor 
 */

void dog_wants_service(PetShopMonitor* monitor);
/**
 * @brief 
 * 
 * @param monitor 
 */
void cat_attended(PetShopMonitor* monitor);

/**
 * @brief 
 * 
 * @param monitor 
 */
void dog_attended(PetShopMonitorInfo* monitor);

#endif