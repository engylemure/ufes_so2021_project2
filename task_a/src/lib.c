#include "./lib.h"

BathroomMonitor *new_bathroom_monitor(unsigned int size) {
    if (size == 0) {
        perror("number of bathrooms on monitor should be greater than 0!\n");
        exit(1);
    }
    BathroomMonitor *monitor = (BathroomMonitor *) malloc(sizeof(BathroomMonitor));
    monitor->occupied_by = None;
    monitor->prioritized_team = None;
    monitor->size = size;
    pthread_cond_init(&monitor->can_flamenguista_enter, NULL);
    pthread_cond_init(&monitor->can_vascaino_enter, NULL);
    pthread_cond_init(&monitor->is_bathroom_available, NULL);
    pthread_mutex_init(&monitor->lock, NULL);
    monitor->amount_on_bathroom = 0;
    monitor->vascainos_waiting = 0;
    monitor->flamenguistas_waiting = 0;
    return monitor;
}

void drop_bathroom_monitor(BathroomMonitor *monitor) {
    free(monitor);
}

void flamenguista_wants_in(BathroomMonitor *monitor) {
    pthread_mutex_lock(&monitor->lock);
    switch (monitor->occupied_by) {
        case Flamengo:
            if (monitor->prioritized_team == Vasco) {
                monitor->flamenguistas_waiting += 1;
                pthread_cond_wait(&monitor->can_flamenguista_enter, &monitor->lock);
                monitor->flamenguistas_waiting -= 1;
                pthread_mutex_unlock(&monitor->lock);
                flamenguista_wants_in(monitor);
            } else if (monitor->amount_on_bathroom < monitor->size) {
                monitor->amount_on_bathroom += 1;
                if (monitor->amount_on_bathroom == monitor->size && monitor->vascainos_waiting) {
                    monitor->prioritized_team = Vasco;
                }
                pthread_mutex_unlock(&monitor->lock);
            } else {
                monitor->flamenguistas_waiting += 1;
                pthread_cond_wait(&monitor->is_bathroom_available, &monitor->lock);
                monitor->flamenguistas_waiting -= 1;
                pthread_mutex_unlock(&monitor->lock);
                flamenguista_wants_in(monitor);
            }
            break;
        case Vasco:
            monitor->flamenguistas_waiting += 1;
            pthread_cond_wait(&monitor->can_flamenguista_enter, &monitor->lock);
            monitor->flamenguistas_waiting -= 1;
            pthread_mutex_unlock(&monitor->lock);
            flamenguista_wants_in(monitor);
            break;
        case None:
            monitor->occupied_by = Flamengo;
            monitor->amount_on_bathroom += 1;
            pthread_mutex_unlock(&monitor->lock);
            break;
    }
}

void flamenguista_goes_out(BathroomMonitor *monitor) {
    pthread_mutex_lock(&monitor->lock);
    if (monitor->occupied_by == Flamengo) {
        bool was_bathroom_full = monitor->amount_on_bathroom == monitor->size;
        monitor->amount_on_bathroom -= 1;
        if (monitor->prioritized_team == Vasco) {
            if (monitor->amount_on_bathroom == 0) {
                pthread_cond_broadcast(&monitor->can_vascaino_enter);
                monitor->occupied_by = None;
            }
        } else {
            if (monitor->amount_on_bathroom == 0) {
                monitor->occupied_by = None;
                monitor->prioritized_team = None;
                pthread_cond_broadcast(&monitor->can_vascaino_enter);
            }
            pthread_cond_broadcast(&monitor->can_flamenguista_enter);
        }
    } else {
        pthread_mutex_unlock(&monitor->lock);
        perror("flamenguistas can't go out when the bathroom is occupied by vascainos!\n");
        exit(1);
    }
    pthread_cond_broadcast(&monitor->is_bathroom_available);
    pthread_mutex_unlock(&monitor->lock);
}

void vascaino_wants_in(BathroomMonitor *monitor) {
    pthread_mutex_lock(&monitor->lock);
    switch (monitor->occupied_by) {
        case Vasco:
            if (monitor->prioritized_team == Flamengo) {
                monitor->vascainos_waiting += 1;
                pthread_cond_wait(&monitor->can_vascaino_enter, &monitor->lock);
                monitor->vascainos_waiting -= 1;
                pthread_mutex_unlock(&monitor->lock);
                vascaino_wants_in(monitor);
            } else if (monitor->amount_on_bathroom < monitor->size) {
                monitor->amount_on_bathroom += 1;
                if (monitor->amount_on_bathroom == monitor->size && monitor->flamenguistas_waiting) {
                    monitor->prioritized_team = Flamengo;
                }
                pthread_mutex_unlock(&monitor->lock);
            } else {
                monitor->vascainos_waiting += 1;
                pthread_cond_wait(&monitor->is_bathroom_available, &monitor->lock);
                monitor->vascainos_waiting -= 1;
                pthread_mutex_unlock(&monitor->lock);
                vascaino_wants_in(monitor);
            }
            break;
        case Flamengo:
            monitor->vascainos_waiting += 1;
            pthread_cond_wait(&monitor->can_vascaino_enter, &monitor->lock);
            monitor->vascainos_waiting -= 1;
            pthread_mutex_unlock(&monitor->lock);
            vascaino_wants_in(monitor);
            break;
        case None:
            monitor->occupied_by = Vasco;
            monitor->amount_on_bathroom += 1;
            pthread_mutex_unlock(&monitor->lock);
            break;
    }
}

void vascaino_goes_out(BathroomMonitor *monitor) {
    pthread_mutex_lock(&monitor->lock);
    if (monitor->occupied_by == Vasco) {
        bool was_bathroom_full = monitor->amount_on_bathroom == monitor->size;
        monitor->amount_on_bathroom -= 1;
        if (monitor->prioritized_team == Flamengo) {
            if (monitor->amount_on_bathroom == 0) {
                pthread_cond_broadcast(&monitor->can_flamenguista_enter);
                monitor->occupied_by = None;
            }
        } else {
            if (monitor->amount_on_bathroom == 0) {
                monitor->occupied_by = None;
                monitor->prioritized_team = None;
                pthread_cond_broadcast(&monitor->can_flamenguista_enter);
            }
            pthread_cond_broadcast(&monitor->can_vascaino_enter);
        }
    } else {
        pthread_mutex_unlock(&monitor->lock);
        perror("vascainos can't go out when the bathroom is occupied by flamenguistas!\n");
        exit(1);
    }
    pthread_cond_broadcast(&monitor->is_bathroom_available);
    pthread_mutex_unlock(&monitor->lock);
}

BathroomMonitorInfo *bathroom_monitor_info(BathroomMonitor *monitor) {
    pthread_mutex_lock(&monitor->lock);
    BathroomMonitorInfo *info = malloc(sizeof(BathroomMonitorInfo));
    int amount_on_bathroom = monitor->amount_on_bathroom;
    info->amount_on_bathroom = amount_on_bathroom;
    info->flamenguistas_waiting = monitor->flamenguistas_waiting;
    info->vascainos_waiting = monitor->vascainos_waiting;
    info->occupied_by = monitor->occupied_by;
    info->prioritized_team = monitor->prioritized_team;
    pthread_mutex_unlock(&monitor->lock);
    return info;
}

const char *FLAMENGO_STR = "Flamengo";
const char *VASCO_STR = "Vasco";
const char *NONE_STR = "None";
const char *team_on_bathroom_str(enum TeamOnBathroom value) {
    switch (value) {
        case Flamengo:
            return FLAMENGO_STR;
        case Vasco:
            return VASCO_STR;
        case None:
            return NONE_STR;
    }
}
char *bathroom_monitor_fmt(BathroomMonitor *monitor) {
    BathroomMonitorInfo *info = bathroom_monitor_info(monitor);
    char *monitor_fmt = malloc(sizeof(char) * 200);
    sprintf(
            monitor_fmt,
            "BathroomMonitor { occupied_by: %s, amount_on_bathroom: %d, flamenguistas_waiting: %d, vascainos_waiting: %d }",
            team_on_bathroom_str(info->occupied_by),
            info->amount_on_bathroom,
            info->flamenguistas_waiting,
            info->vascainos_waiting);
    return monitor_fmt;
}