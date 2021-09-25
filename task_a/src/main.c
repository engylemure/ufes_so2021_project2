#include "lib.h"
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

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

void fill_with_spaces(char *str, unsigned int size) {
    int len = strlen(str);
    int i;
    for (i = len; i < size; i++) {
        str[i] = i == size - 1 ? '\0' : ' ';
    }
}

void *monitor_info(void *arg) {
    BathroomMonitor *monitor = arg;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    while (true) {
        gettimeofday(&end, NULL);
        usleep(16 * 1000);
        // char *info = bathroom_monitor_fmt(monitor);
        BathroomMonitorInfo *m_info = bathroom_monitor_info(monitor);
        printf("\033[A");
        printf("\033[A");
        printf("\033[A");
        printf("\033[A");
        printf("\033[A");
        printf("\033[A");
        printf("%c[2K", 27);
        double time_taken;
        time_taken = (end.tv_sec - start.tv_sec) * 1e6;
        time_taken = (time_taken + (end.tv_usec -
                                    start.tv_usec)) *
                     1e-6;
        printf("\r\t\t\tTime since it started: %.2f s\n\n", time_taken);
        printf("%c[2K", 27);
        printf("\r\toccupied_by | prioritized_team | amount_on_bathroom | flamenguistas_waiting | vascainos_waiting\n");
        printf("%c[2K", 27);
        char occupied_by[12];
        char prioritized_team[17];
        sprintf(prioritized_team, "%s", team_on_bathroom_str(m_info->prioritized_team));
        sprintf(occupied_by, "%s", team_on_bathroom_str(m_info->occupied_by));
        fill_with_spaces(occupied_by, 12);
        fill_with_spaces(prioritized_team, 17);
        char amount_on_bathroom[19];
        sprintf(amount_on_bathroom, "%d", m_info->amount_on_bathroom);
        fill_with_spaces(amount_on_bathroom, 19);
        char flamenguistas_waiting[22];
        sprintf(flamenguistas_waiting, "%d", m_info->flamenguistas_waiting);
        fill_with_spaces(flamenguistas_waiting, 22);
        char vascainos_waiting[18];
        sprintf(vascainos_waiting, "%d", m_info->vascainos_waiting);
        fill_with_spaces(vascainos_waiting, 18);
        printf("\r\t%s | %s | %s | %s | %s\n\n\n", occupied_by, prioritized_team, amount_on_bathroom, flamenguistas_waiting, vascainos_waiting);
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
    printf("\t\t\t\tStarting monitor and test!\n\n\n\n\n\n\n\n");
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