#include "./lib.h"


PetShopMonitor *new_petshop_monitor(unsigned int max_pets_on_service_room) {
    PetShopMonitor *monitor = malloc(sizeof(PetShopMonitor));
    monitor->animal_being_attended = None;
    monitor->prioritized_animal = None;
    monitor->max_pets_on_service_room = max_pets_on_service_room;
    monitor->pets_on_service_room = 0;
    monitor->dogs_waiting = 0;
    monitor->cats_waiting = 0;
    sem_init(&monitor->inner_lock, 0, 1);
    sem_init(&monitor->can_dog_enter, 0, 1);
    sem_init(&monitor->can_cat_enter, 0, 1);
    sem_init(&monitor->is_service_room_available, 0, max_pets_on_service_room);
    return monitor;
}

void sem_cond(sem_t* cond, sem_t* lock) {
    sem_post(lock);
    sem_wait(cond);
    sem_wait(lock);
}

void sem_cond_signal(sem_t * cond) {
    sem_post(cond);
}


PetShopMonitorInfo *petshop_monitor_info(PetShopMonitor *monitor);

void cat_wants_service(PetShopMonitor *monitor) {
    
}

void dog_wants_service(PetShopMonitor *monitor);

void cat_attended(PetShopMonitor *monitor);

void dog_attended(PetShopMonitorInfo *monitor);