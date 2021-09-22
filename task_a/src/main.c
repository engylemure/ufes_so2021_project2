#include "lib.h"
#include <stdint.h>
#include <time.h>
#define AMOUNT_OF_TIME_INSIDE_BATHROOM 5

void vas_wants_in(void **args) {
    int idx = (uintptr_t) args[0];
    BathroomMonitor *monitor = args[1];
    vascaino_wants_in(monitor);
    sleep(AMOUNT_OF_TIME_INSIDE_BATHROOM);
    vascaino_goes_out(monitor);
}

void fla_wants_in(void **args) {
    int idx = (uintptr_t) args[0];
    BathroomMonitor *monitor = args[1];
    flamenguista_wants_in(monitor);
    sleep(AMOUNT_OF_TIME_INSIDE_BATHROOM);
    flamenguista_goes_out(monitor);
}

void *monitor_info(void *arg) {
    BathroomMonitor *monitor = arg;
    clock_t started_at = clock();
    while (true) {
        usleep(16 * 1000);
        char *info = bathroom_monitor_fmt(monitor);
        printf("\033[A");
        printf("%c[2K", 27);
        printf("\rTime since it started: %.2f s\n", (clock() - started_at)/1000.0);
        printf("%c[2K", 27);
        printf("\r%s", info);
        fflush(stdout);
        free(info);
    }
}


unsigned int FOOTBALL_FANS_SIZE = 20;
unsigned int BATHROOM_SIZE = 3;
unsigned int TIME_BETWEEN_FAN_ARRIVAL = 1;
int main(void) {
    pthread_t football_fans[FOOTBALL_FANS_SIZE];
    pthread_t info_thread;
    int i;
    printf("Starting monitor and test!\n");
    BathroomMonitor *monitor = new_bathroom_monitor(BATHROOM_SIZE);
    pthread_create(&info_thread, NULL, monitor_info, monitor);
    for (i = 0; i < FOOTBALL_FANS_SIZE; i++) {
        void *args[2];
        args[0] = (void *) (uintptr_t) i;
        args[1] = monitor;
        if (i % 2 == 0) {
            pthread_create(&football_fans[i], NULL, (void *(*) (void *) ) fla_wants_in, args);
        } else {
            pthread_create(&football_fans[i], NULL, (void *(*) (void *) ) vas_wants_in, args);
        }
        sleep(TIME_BETWEEN_FAN_ARRIVAL);
    }
    for (i = 0; i < FOOTBALL_FANS_SIZE; i++) {
        pthread_join(football_fans[i], NULL);
    }
    sleep(1);
    pthread_cancel(info_thread);
    printf("\n");
    drop_bathroom_monitor(monitor);
    return 0;
}