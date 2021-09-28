#include "lib.h"
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

void vas_wants_in(void **args) {
    int idx = (uintptr_t) args[0];
    BathroomMonitor *monitor = args[1];
    int amount_of_time_inside_bathroom = (uintptr_t) args[2];
    bool should_loop_thread = (uintptr_t) args[3];
    if (should_loop_thread) {
        while (should_loop_thread) {
            vascaino_wants_in(monitor);
            sleep(amount_of_time_inside_bathroom);
            vascaino_goes_out(monitor);
            sleep(amount_of_time_inside_bathroom);
        }
    } else {
        vascaino_wants_in(monitor);
        sleep(amount_of_time_inside_bathroom);
        vascaino_goes_out(monitor);
    }
}

void fla_wants_in(void **args) {
    int idx = (uintptr_t) args[0];
    BathroomMonitor *monitor = args[1];
    int amount_of_time_inside_bathroom = (uintptr_t) args[2];
    bool should_loop_thread = (uintptr_t) args[3];
    if (should_loop_thread) {
        while (true) {
            flamenguista_wants_in(monitor);
            sleep(amount_of_time_inside_bathroom);
            flamenguista_goes_out(monitor);
            sleep(amount_of_time_inside_bathroom);
        }
    } else {
        flamenguista_wants_in(monitor);
        sleep(amount_of_time_inside_bathroom);
        flamenguista_goes_out(monitor);
    }
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
        printf("\r\toccupied_by | occupied_by_times | prioritized_team | amount_on_bathroom | flamenguistas_waiting | vascainos_waiting | times_used \n");
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
        char amount_used[11];
        sprintf(amount_used, "%d", m_info->amount_of_fans_that_used_the_bathroom);
        fill_with_spaces(amount_used, 11);
        char occupied_by_times[18];
        sprintf(occupied_by_times, "%d", m_info->occupied_by_used_times);
        fill_with_spaces(occupied_by_times, 18);
        printf("\r\t%s | %s | %s | %s | %s | %s | %s\n\n\n",
               occupied_by,
               occupied_by_times,
               prioritized_team,
               amount_on_bathroom,
               flamenguistas_waiting,
               vascainos_waiting,
               amount_used);

        fflush(stdout);
        free(m_info);
    }
}


unsigned int FOOTBALL_FANS_SIZE = 20;
unsigned int BATHROOM_SIZE = 3;
unsigned int TIME_BETWEEN_FAN_ARRIVAL = 1;
unsigned int AMOUNT_OF_TIME_INSIDE_BATHROOM = 1;

char *get_arg(char *key, char *arg) {
    char *start_of_key = strstr(arg, key);
    if (start_of_key != NULL) {
        return (start_of_key) + strlen(key);
    }
    return NULL;
}


int main(int argc, char *argv[]) {
    pthread_t info_thread;
    unsigned int fans_size = FOOTBALL_FANS_SIZE;
    unsigned int bathroom_size = BATHROOM_SIZE;
    unsigned int time_between_fans_arrival = TIME_BETWEEN_FAN_ARRIVAL;
    unsigned int time_inside_bathroom = AMOUNT_OF_TIME_INSIDE_BATHROOM;
    bool should_loop_thread = true;
    enum TeamOnBathroom initial_team = Flamengo;
    int i;
    for (i = 1; i < argc; i++) {
        char *arg;
        int val;
        if ((arg = get_arg("fans_size=", argv[i])) != NULL && (val = atoi(arg))) {
            fans_size = val;
        }
        if ((arg = get_arg("bathroom_size=", argv[i])) != NULL && (val = atoi(arg))) {
            bathroom_size = val;
        }
        if ((arg = get_arg("time_between_fans_arrival=", argv[i])) != NULL && (val = atoi(arg))) {
            time_between_fans_arrival = val;
        }
        if ((arg = get_arg("time_inside_bathroom=", argv[i])) != NULL && (val = atoi(arg))) {
            time_inside_bathroom = val;
        }
        if ((arg = get_arg("initial_team=", argv[i])) != NULL) {
            if (strcmp(arg, "v") == 0 || strcmp(arg, "vasco") == 0) {
                initial_team = Vasco;
            }
        }
        if ((arg = get_arg("loop_thread=", argv[i]))) {
            if (strcmp(arg, "0") == 0 || strcmp(arg, "false") == 0) {
                should_loop_thread = false;
            }
        }
    }
    printf("\t\t\t\tStarting monitor and test!\n\n");
    char *should_loop_thread_str = should_loop_thread ? "true" : "false";
    printf("\t{ fans_size: %d, bathroom_size: %d, time_between_fans_arrival: %d sec,",
        fans_size,
        bathroom_size,
        time_between_fans_arrival);
    printf(" time_inside_bathroom: %d sec, initial_team: %s, loop: %s }\n\n\n\n\n\n\n\n",
        time_inside_bathroom,
        team_on_bathroom_str(initial_team),
        should_loop_thread_str);
    BathroomMonitor *monitor = new_bathroom_monitor(bathroom_size, Vasco);
    pthread_create(&info_thread, NULL, monitor_info, monitor);
    void *(*initial_thread_cb)(void *) = (void *(*) (void *) )(initial_team == Flamengo ? fla_wants_in : vas_wants_in);
    void *(*second_thread_cb)(void *) = (void *(*) (void *) )(initial_team == Flamengo ? vas_wants_in : fla_wants_in);
    pthread_t football_fans[fans_size];
    for (i = 0; i < fans_size; i++) {
        void *args[4];
        args[0] = (void *) (uintptr_t) i;
        args[1] = monitor;
        args[2] = (void *) (uintptr_t) time_inside_bathroom;
        args[3] = (void *) (uintptr_t) should_loop_thread;
        if (i % 2 == 0) {
            pthread_create(&football_fans[i], NULL, initial_thread_cb, args);
        } else {
            pthread_create(&football_fans[i], NULL, second_thread_cb, args);
        }
        sleep(time_between_fans_arrival);
    }
    for (i = 0; i < fans_size; i++) {
        pthread_join(football_fans[i], NULL);
    }
    sleep(1);
    pthread_cancel(info_thread);
    pthread_join(info_thread, NULL);
    printf("\n");
    drop_bathroom_monitor(monitor);
    return 0;
}