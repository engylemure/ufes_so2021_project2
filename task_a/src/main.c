#include "lib.h"
#include <stdint.h>

void vas_wants_in(void **args) {
    int idx = (uintptr_t) args[0];
    BathroomMonitor *monitor = args[1];
    // printf("vascaino id: %d goes to bathroom\n", idx);
    vascaino_wants_in(monitor);
    printf("vascaino id: %d is on the bathroom\n", idx);
    sleep(1);
    vascaino_goes_out(monitor);
    printf("vascaino id: %d is of the bathroom\n", idx);
}

void fla_wants_in(void **args) {
    int idx = (uintptr_t) args[0];
    BathroomMonitor *monitor = args[1];
    // printf("flamenguista id: %d goes to bathroom\n", idx);
    flamenguista_wants_in(monitor);
    printf("flamenguista id: %d is on the bathroom\n", idx);
    sleep(1);
    flamenguista_goes_out(monitor);
    printf("flamenguista id: %d is of the bathroom\n", idx);
}

unsigned int FOOTBALL_FANS_SIZE = 20;
unsigned int BATHROOM_SIZE = 3;

int main(void) {
    pthread_t football_fans[FOOTBALL_FANS_SIZE];
    int i;
    BathroomMonitor* monitor = new_bathroom_monitor(BATHROOM_SIZE);
    for (i = 0; i < FOOTBALL_FANS_SIZE; i++) {
        void* args[2];
        args[0] = (void*)(uintptr_t)i;
        args[1] = monitor;
        if (i % 2 == 0) {
            pthread_create(&football_fans[i], NULL, (void *(*)(void*)) fla_wants_in, args);
        } else {
            pthread_create(&football_fans[i], NULL, (void *(*)(void*)) vas_wants_in, args);
        }
    }
    for (i = 0; i < FOOTBALL_FANS_SIZE; i++) {
        pthread_join(football_fans[i], NULL);
    }
    drop_bathroom_monitor(monitor);
    return 0;
}