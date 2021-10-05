#include "./lib.h"

const char *CAT_STR = "Cat";
const char *DOG_STR = "Dog";
const char *NONE_STR = "None";

const char *animal_str(enum Animal animal) {
    switch (animal) {
        case Cat:
            return CAT_STR;
        case Dog:
            return DOG_STR;
        case None:
            return NONE_STR;
    }
}

PetShopMonitor *new_petshop_monitor(unsigned int max_pets_on_service_room, enum Animal prioritized_animal) {
    PetShopMonitor *monitor = malloc(sizeof(PetShopMonitor));
    if (max_pets_on_service_room <= 0) {
        perror("max_pets_on_service_room should be greater than 0!\n");
        exit(1);
    }
    monitor->animal_being_attended = None;
    monitor->prioritized_animal = prioritized_animal;
    monitor->max_pets_on_service_room = max_pets_on_service_room;
    monitor->pets_on_service_room = 0;
    monitor->being_attended_used_times = 0;
    monitor->dogs_waiting = 0;
    monitor->cats_waiting = 0;
    monitor->amount_of_cats_attended = 0;
    monitor->amount_of_dogs_attended = 0;
    sem_init(&monitor->inner_lock, 0, 1);
    sem_init(&monitor->waiting_lock, 0, 0);
    // sem_init(&monitor->can_dog_enter, 0, 1);
    // sem_init(&monitor->can_cat_enter, 0, 1);
    // sem_init(&monitor->is_service_room_available, 0, max_pets_on_service_room);
    return monitor;
}

void drop_petshop_monitor_info(PetShopMonitor *monitor) {
    sem_destroy(&monitor->inner_lock);
    // sem_destroy(&monitor->is_service_room_available);
    sem_destroy(&monitor->waiting_lock);
    // sem_destroy(&monitor->can_cat_enter);
    // sem_destroy(&monitor->can_dog_enter);
    free(monitor);
}

void sem_cond_wait(sem_t *cond, sem_t *lock) {
    sem_post(lock);
    sem_wait(cond);
    sem_wait(lock);
    sem_post(cond);
}


PetShopMonitorInfo *petshop_monitor_info(PetShopMonitor *monitor) {
    PetShopMonitorInfo *info = malloc(sizeof(PetShopMonitorInfo));
    sem_wait(&monitor->inner_lock);
    info->animal_being_attended = monitor->animal_being_attended;
    info->prioritized_animal = monitor->prioritized_animal;
    info->max_pets_on_service_room = monitor->max_pets_on_service_room;
    info->pets_on_service_room = monitor->pets_on_service_room;
    info->dogs_waiting = monitor->dogs_waiting;
    info->cats_waiting = monitor->cats_waiting;
    info->amount_of_dogs_attended = monitor->amount_of_dogs_attended;
    info->amount_of_cats_attended = monitor->amount_of_cats_attended;
    info->being_attended_used_times = monitor->being_attended_used_times;
    sem_post(&monitor->inner_lock);
    return info;
}

bool is_full(PetShopMonitor *monitor) {
    return monitor->pets_on_service_room == monitor->max_pets_on_service_room;
}

bool is_empty(PetShopMonitor *monitor) {
    return monitor->pets_on_service_room == 0;
}

void cat_wants_service(PetShopMonitor *monitor) {
    sem_wait(&monitor->inner_lock);
    switch (monitor->animal_being_attended) {
        case Cat:
            if (monitor->prioritized_animal == Dog || is_full(monitor)) {
                monitor->cats_waiting += 1;
                sem_post(&monitor->inner_lock);
                sem_wait(&monitor->waiting_lock);
                sem_wait(&monitor->inner_lock);
                monitor->cats_waiting -= 1;
                sem_post(&monitor->inner_lock);
                cat_wants_service(monitor);
            } else {
                monitor->pets_on_service_room += 1;
                monitor->being_attended_used_times += 1;
                if (is_full(monitor) && monitor->dogs_waiting) {
                    monitor->prioritized_animal = Dog;
                }
                sem_post(&monitor->inner_lock);
            }
            break;
        case Dog:
            monitor->cats_waiting += 1;
            sem_post(&monitor->inner_lock);
            sem_wait(&monitor->waiting_lock);
            sem_wait(&monitor->inner_lock);
            monitor->cats_waiting -= 1;
            sem_post(&monitor->inner_lock);
            cat_wants_service(monitor);
            break;
        case None:
            monitor->animal_being_attended = Cat;
            monitor->pets_on_service_room += 1;
            monitor->being_attended_used_times += 1;
            sem_post(&monitor->inner_lock);
            break;
    }
}

void dog_wants_service(PetShopMonitor *monitor) {
    sem_wait(&monitor->inner_lock);
    switch (monitor->animal_being_attended) {
        case Dog:
            if (monitor->prioritized_animal == Cat || is_full(monitor)) {
                monitor->dogs_waiting += 1;
                sem_post(&monitor->inner_lock);
                sem_wait(&monitor->waiting_lock);
                sem_wait(&monitor->inner_lock);
                monitor->dogs_waiting -= 1;
                sem_post(&monitor->inner_lock);
                cat_wants_service(monitor);
            } else {
                monitor->pets_on_service_room += 1;
                monitor->being_attended_used_times += 1;
                if (is_full(monitor) && monitor->cats_waiting) {
                    monitor->prioritized_animal = Cat;
                }
                sem_post(&monitor->inner_lock);
            }
            break;
        case Cat:
            monitor->dogs_waiting += 1;
            sem_post(&monitor->inner_lock);
            sem_wait(&monitor->waiting_lock);
            sem_wait(&monitor->inner_lock);
            monitor->dogs_waiting -= 1;
            sem_post(&monitor->inner_lock);
            cat_wants_service(monitor);
            break;
        case None:
            monitor->animal_being_attended = Dog;
            monitor->pets_on_service_room += 1;
            monitor->being_attended_used_times += 1;
            sem_post(&monitor->inner_lock);
            break;
    }
}


void cat_attended(PetShopMonitor *monitor) {
    sem_wait(&monitor->inner_lock);
    if (monitor->animal_being_attended == Cat) {
        monitor->pets_on_service_room -= 1;
        if (monitor->prioritized_animal == Dog && monitor->dogs_waiting) {
            if (monitor->pets_on_service_room == 0) {
                monitor->animal_being_attended = None;
                monitor->being_attended_used_times = 0;
            }
        } else {
            if (monitor->pets_on_service_room == 0) {
                monitor->animal_being_attended = None;
                monitor->prioritized_animal = None;
                monitor->being_attended_used_times = 0;
            }
        }
        monitor->amount_of_cats_attended += 1;
    } else {
        sem_post(&monitor->inner_lock);
        perror("cats can't go out when the service room is currently occupied by dogs!\n");
        exit(1);
    }
    sem_post(&monitor->waiting_lock);
    sem_post(&monitor->inner_lock);
}

void dog_attended(PetShopMonitor *monitor) {
    sem_wait(&monitor->inner_lock);
    if (monitor->animal_being_attended == Dog) {
        monitor->pets_on_service_room -= 1;
        if (monitor->prioritized_animal == Cat && monitor->cats_waiting) {
            if (monitor->pets_on_service_room == 0) {
                monitor->animal_being_attended = None;
                monitor->being_attended_used_times = 0;
            }
        } else {
            if (monitor->pets_on_service_room == 0) {
                monitor->animal_being_attended = None;
                monitor->prioritized_animal = None;
                monitor->being_attended_used_times = 0;
            }
        }
        monitor->amount_of_dogs_attended += 1;
    } else {
        sem_post(&monitor->inner_lock);
        perror("dogs can't go out when the service room is currently occupied by cats!\n");
        exit(1);
    }
    sem_post(&monitor->waiting_lock);
    sem_post(&monitor->inner_lock);
}