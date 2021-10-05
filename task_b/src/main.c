#include "lib.h"
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>

void cat_wants_in(void **args) {
    int idx = (uintptr_t) args[0];
    PetShopMonitor *monitor = args[1];
    int amount_of_time_inside_service = (uintptr_t) args[2];
    bool should_loop_thread = (uintptr_t) args[3];
    if (should_loop_thread) {
        while (should_loop_thread) {
            cat_wants_service(monitor);
            // printf("cat %d in\n", idx);
            sleep(amount_of_time_inside_service);
            cat_attended(monitor);
            // printf("cat %d out\n", idx);
            sleep(amount_of_time_inside_service);
        }
    } else {
        cat_wants_service(monitor);
        sleep(amount_of_time_inside_service);
        cat_attended(monitor);
    }
}

void dog_wants_in(void **args) {
    int idx = (uintptr_t )args[0];
    PetShopMonitor *monitor = args[1];
    int amount_of_time_inside_service = (uintptr_t) args[2];
    bool should_loop_thread = (uintptr_t) args[3];
    if (should_loop_thread) {
        while (true) {
            dog_wants_service(monitor);
            // printf("dog %d in\n", idx);
            sleep(amount_of_time_inside_service);
            dog_attended(monitor);
            // printf("dog %d out\n", idx);
            sleep(amount_of_time_inside_service);
        }
    } else {
        dog_wants_service(monitor);
        sleep(amount_of_time_inside_service);
        dog_attended(monitor);
    }
}

void fill_with_spaces(char *str, unsigned int size) {
    int len = strlen(str);
    int i;
    for (i = len; i < size; i++) {
        str[i] = i == size - 1 ? '\0' : ' ';
    }
}

bool loop_monitor_info = true;
void *monitor_info(void *arg) {
    PetShopMonitor *monitor = arg;
   struct timeval start, end;
   gettimeofday(&start, NULL);
 while (loop_monitor_info) {
     gettimeofday(&end, NULL);
     usleep(16 * 1000);
     PetShopMonitorInfo *m_info = petshop_monitor_info(monitor);
      printf("\033[A");
      printf("\033[A");
      printf("\033[A");
      printf("\033[A");
      printf("\033[A");
      printf("\033[A");
      printf("%c[2K", 27);
      double time_taken;
      time_taken = (double)(end.tv_sec - start.tv_sec) * 1e6;
      time_taken = (time_taken + (double)(end.tv_usec -
                                  start.tv_usec)) *
                   1e-6;
      printf("\r\t\t\t\t\t\tTime since it started: %.2f s\n\n", time_taken);
      printf("%c[2K", 27);
      printf("\r\ton_service | prior_animal | amount_on_service | dogs_waiting | cats_waiting | cats_attended | dogs_attended \n");
      printf("%c[2K", 27);
      char on_service[11];
      char prior_animal[13];
      sprintf(prior_animal, "%s", animal_str(m_info->prioritized_animal));
      sprintf(on_service, "%s", animal_str(m_info->animal_being_attended));
      fill_with_spaces(on_service, 11);
      fill_with_spaces(prior_animal, 13);
      char amount_on_service[18];
      sprintf(amount_on_service, "%d", m_info->pets_on_service_room);
      fill_with_spaces(amount_on_service, 18);
      char dogs_waiting[13];
      sprintf(dogs_waiting, "%d", m_info->dogs_waiting);
      fill_with_spaces(dogs_waiting, 13);
      char cats_waiting[13];
      sprintf(cats_waiting, "%d", m_info->cats_waiting);
      fill_with_spaces(cats_waiting, 13);
      char cats_attended[14];
      char dogs_attended[14];
      sprintf(cats_attended, "%d", m_info->amount_of_cats_attended);
      sprintf(dogs_attended, "%d", m_info->amount_of_dogs_attended);
      fill_with_spaces(cats_attended, 14);
      fill_with_spaces(cats_attended, 14);
      printf("\r\t%s | %s | %s | %s | %s | %s | %s\n\n\n",
             on_service,
             prior_animal,
             amount_on_service,
             dogs_waiting,
             cats_waiting,
             cats_attended,
             dogs_attended);

     fflush(stdout);
     free(m_info);
 }
  return NULL;
}


char *get_arg(char *key, char *arg) {
    char *start_of_key = strstr(arg, key);
    if (start_of_key != NULL) {
        return (start_of_key) + strlen(key);
    }
    return NULL;
}

unsigned int AMOUNT_OF_ANIMALS = 20;
unsigned int SERVICE_ROOM_SIZE = 3;
unsigned int TIME_BETWEEN_ANIMAL_ARRIVAL = 1;
unsigned int AMOUNT_OF_TIME_INSIDE_SERVICE = 3;

int main(int argc, char *argv[]) {
    pthread_t info_thread;
    unsigned int amount_of_animals = AMOUNT_OF_ANIMALS;
    unsigned int service_room_size = SERVICE_ROOM_SIZE;
    unsigned int time_between_animals_arrival = TIME_BETWEEN_ANIMAL_ARRIVAL;
    unsigned int time_inside_service = AMOUNT_OF_TIME_INSIDE_SERVICE;
    bool should_loop_thread = true;
    enum Animal initial_animal = Dog;
    int i;
    for (i = 1; i < argc; i++) {
        char *arg;
        int val;
        if ((arg = get_arg("amount_of_animals=", argv[i])) != NULL && (val = atoi(arg))) {
            amount_of_animals = val;
        }
        if ((arg = get_arg("room_size=", argv[i])) != NULL && (val = atoi(arg))) {
            service_room_size = val;
        }
        if ((arg = get_arg("animals_arrival_period=", argv[i])) != NULL && (val = atoi(arg))) {
            time_between_animals_arrival = val;
        }
        if ((arg = get_arg("time_on_service=", argv[i])) != NULL && (val = atoi(arg))) {
            time_inside_service = val;
        }
        if ((arg = get_arg("first_animal=", argv[i])) != NULL) {
            for(i = 0; i < strlen(arg); i++) {
                arg[i] = (char)tolower(arg[i]);
            }
            if (strcmp(arg, "c") == 0 || strcmp(arg, "cat") == 0) {
                initial_animal = Cat;
            }
        }
        if ((arg = get_arg("loop=", argv[i]))) {
            if (strcmp(arg, "0") == 0 || strcmp(arg, "false") == 0) {
                should_loop_thread = false;
            }
        }
    }
    printf("\t\t\t\t\t\tStarting monitor and test!\n\n");
    char *should_loop_thread_str = should_loop_thread ? "true" : "false";
    printf("\t{ amount_of_animals: %d, room_size: %d, animals_arrival_period: %d sec,",
           amount_of_animals,
           service_room_size,
           time_between_animals_arrival);
    printf(" time_on_service: %d sec, first_animal: %s, loop: %s }\n\n\n\n\n\n\n\n",
           time_inside_service,
           animal_str(initial_animal),
           should_loop_thread_str);
    PetShopMonitor *monitor = new_petshop_monitor(service_room_size, Cat);
    pthread_create(&info_thread, NULL, monitor_info, monitor);
    void *(*initial_thread_cb)(void *) = (void *(*) (void *) )(initial_animal == Dog ? dog_wants_in : cat_wants_in);
   void *(*second_thread_cb)(void *) = (void *(*) (void *) )(initial_animal == Dog ? cat_wants_in : dog_wants_in);
    pthread_t animal_threads[amount_of_animals];
    for (i = 0; i < amount_of_animals; i++) {
        void *args[4];
        args[0] = (void *) (uintptr_t) i;
        args[1] = monitor;
        args[2] = (void *) (uintptr_t) time_inside_service;
        args[3] = (void *) (uintptr_t) should_loop_thread;
        if (i % 2 == 0) {
            pthread_create(&animal_threads[i], NULL, initial_thread_cb, args);
        } else {
            pthread_create(&animal_threads[i], NULL, second_thread_cb, args);
        }
        sleep(time_between_animals_arrival);
    }
    for (i = 0; i < amount_of_animals; i++) {
        pthread_join(animal_threads[i], NULL);
    }
    sleep(1);
    loop_monitor_info = false;
    pthread_join(info_thread, NULL);
    printf("\n");
    drop_petshop_monitor_info(monitor);
    return 0;
}