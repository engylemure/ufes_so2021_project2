#include "lib.h"
#include <stdint.h>
#include <time.h>
#include <string.h>

#define AMOUNT_OF_TIME_INSIDE_BATHROOM 3

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

void fill_with_spaces(char* str, unsigned int size) {
    int len = strlen(str);
    int i;
    for(i = len; i < size; i++) {
        str[i] = i == size - 1 ? '\0' : ' '; 
    }
}

void *monitor_info(void *arg) {
    BathroomMonitor *monitor = arg;
    clock_t started_at = clock();
    while (true) {
        usleep(16 * 1000);
        // char *info = bathroom_monitor_fmt(monitor);
        BathroomMonitorInfo *m_info = bathroom_monitor_info(monitor);
        printf("\033[A");
        printf("\033[A");
        printf("%c[2K", 27);
        printf("\rTime since it started: %.2f s\n", 100*((double)(clock() - started_at)) / CLOCKS_PER_SEC);
        printf("%c[2K", 27);
        printf("\r\toccupied_by | amount_on_bathroom | flamenguistas_waiting | vascainos_waiting\n");
        printf("%c[2K", 27);
        char occupied_by[12];
        switch (m_info->occupied_by) {
            case Flamengo:
                sprintf(occupied_by, "Flamengo");
                break;
            case Vasco:
                sprintf(occupied_by, "Vasco");
                break;
            default:
                sprintf(occupied_by, "None");
        }
        fill_with_spaces(occupied_by, 12);
        
        char amount_on_bathroom[19];
        sprintf(amount_on_bathroom, "%d", m_info->amount_on_bathroom);
        fill_with_spaces(amount_on_bathroom, 19);
        char flamenguistas_waiting[22];
        sprintf(flamenguistas_waiting, "%d", m_info->flamenguistas_waiting);
        fill_with_spaces(flamenguistas_waiting, 22);
        char vascainos_waiting[18];
        sprintf(vascainos_waiting, "%d", m_info->vascainos_waiting);
        fill_with_spaces(vascainos_waiting, 18);
        printf("\r\t%s | %s | %s | %s", occupied_by, amount_on_bathroom, flamenguistas_waiting, vascainos_waiting);
        // printf("%c[2K", 27);
        // printf("\r%s", info);

        fflush(stdout);
        // free(info);
        free(m_info);
    }
}


unsigned int FOOTBALL_FANS_SIZE = 20;
unsigned int BATHROOM_SIZE = 3;
unsigned int TIME_BETWEEN_FAN_ARRIVAL = 1;
int main(void) {
    pthread_t football_fans[FOOTBALL_FANS_SIZE];
    pthread_t info_thread;
    int i;
    printf("Starting monitor and test!\n\n");
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