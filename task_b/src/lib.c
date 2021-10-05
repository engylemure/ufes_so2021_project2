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
    monitor->dogs_waiting = 0;
    monitor->cats_waiting = 0;
    monitor->amount_of_cats_attended = 0;
    monitor->amount_of_dogs_attended = 0;
    sem_init(&monitor->inner_lock, 0, 1);
    sem_init(&monitor->can_dog_enter, 0, max_pets_on_service_room);
    sem_init(&monitor->can_cat_enter, 0, max_pets_on_service_room);
    return monitor;
}

void drop_petshop_monitor_info(PetShopMonitor *monitor) {
    sem_destroy(&monitor->inner_lock);
    sem_destroy(&monitor->can_cat_enter);
    sem_destroy(&monitor->can_dog_enter);
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
    sem_post(&monitor->inner_lock);
    return info;
}

void sem_post_amount(sem_t* sem, unsigned int amount) {
    int i;
    for(i = 0; i < amount; i++) {
        sem_post(sem);
    }
}

void cat_wants_service(PetShopMonitor *monitor) {
    sem_wait(&monitor->inner_lock);
    switch (monitor->animal_being_attended) {
        case Cat:
        case Dog:
            monitor->cats_waiting += 1;
            sem_post(&monitor->inner_lock);
            sem_wait(&monitor->can_cat_enter);
            sem_wait(&monitor->inner_lock);
            monitor->animal_being_attended = Cat;
            monitor->cats_waiting -= 1;
            monitor->pets_on_service_room+= 1;
            sem_post(&monitor->inner_lock);
            break;
        case None:
            monitor->animal_being_attended = Cat;
            monitor->pets_on_service_room+= 1;
            sem_init(&monitor->can_dog_enter, 0, 0);
            sem_wait(&monitor->can_cat_enter);
            sem_post(&monitor->inner_lock);
            break;
    }
}

void dog_wants_service(PetShopMonitor *monitor) {
    sem_wait(&monitor->inner_lock);
    switch (monitor->animal_being_attended) {
        case Dog:
        case Cat:
            monitor->dogs_waiting += 1;
            sem_post(&monitor->inner_lock);
            sem_wait(&monitor->can_dog_enter);
            sem_wait(&monitor->inner_lock);
            monitor->animal_being_attended = Dog;
            monitor->dogs_waiting -= 1;
            monitor->pets_on_service_room+= 1;
            sem_post(&monitor->inner_lock);
            break;
        case None:
            monitor->animal_being_attended = Dog;
            monitor->pets_on_service_room+= 1;
            sem_init(&monitor->can_cat_enter, 0, 0);
            sem_wait(&monitor->can_dog_enter);
            sem_post(&monitor->inner_lock);
            break;
    }
}

void cat_attended(PetShopMonitor *monitor) {
    sem_wait(&monitor->inner_lock);
    if (monitor->animal_being_attended == Cat) {
       monitor->pets_on_service_room -= 1;
       monitor->amount_of_cats_attended += 1;
       if (monitor->prioritized_animal ==  Dog && monitor->dogs_waiting) {
           if (monitor->pets_on_service_room == 0) {
               monitor->animal_being_attended = None;
               sem_post_amount(&monitor->can_dog_enter, monitor->max_pets_on_service_room);
           }
       } else {
           sem_post(&monitor->can_cat_enter);
           if (monitor->pets_on_service_room == 0) {
               monitor->animal_being_attended = None;
               sem_post_amount(&monitor->can_dog_enter, monitor->max_pets_on_service_room);
           }
       }
    } else {
        sem_post(&monitor->inner_lock);
        perror("cats can't go out when the service room is currently occupied by dogs!\n");
        exit(1);
    }
    sem_post(&monitor->inner_lock);
}

void dog_attended(PetShopMonitor *monitor) {
    sem_wait(&monitor->inner_lock);
    if (monitor->animal_being_attended == Dog) {
        monitor->pets_on_service_room -= 1;
        monitor->amount_of_dogs_attended += 1;
        if (monitor->prioritized_animal == Cat && monitor->cats_waiting) {
            if (monitor->pets_on_service_room == 0) {
                monitor->animal_being_attended = None;
                sem_post_amount(&monitor->can_cat_enter, monitor->max_pets_on_service_room);
            }
        } else {
            sem_post(&monitor->can_dog_enter);
            if (monitor->pets_on_service_room == 0) {
               monitor->animal_being_attended = None;
               sem_post_amount(&monitor->can_cat_enter, monitor->max_pets_on_service_room);
           }
        }
    } else {
        sem_post(&monitor->inner_lock);
        perror("dogs can't go out when the service room is currently occupied by cats!\n");
        exit(1);
    }
    sem_post(&monitor->inner_lock);
}