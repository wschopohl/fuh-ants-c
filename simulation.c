#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "world.h"
#include "simulation.h"
#include "ant.h"
#include "timer.h"

static pthread_t simulation_thread;
static bool simulation_running = true;
static SimulationPool simulation_pools[THREADS];

static void* simulation_loop(void* args) {
    Timer spawn_timer = get_timer();

    while(simulation_running) {
        if(get_lap_time_us(&spawn_timer) > 1000000 / SPAWN_RATE) {
            WorldAddAnt(GetNewAnt(WorldRef()->nest_position));
            reset_timer(&spawn_timer);
        }
        usleep(1000000 / FPS);
    }
    stop_timer(&spawn_timer);
    return NULL;
}

static void* ant_pool_loop(void* pool_args) {
    SimulationPool *pool = (SimulationPool*)pool_args;
    while(pool->running) {
        for(int i=0;i<pool->count;i++) {
            AntMove(&pool->ants[i]);
        }
        usleep(1000000 / FPS);
    }
    return NULL;
}

void SimulationStart() {
    pthread_create(&simulation_thread, NULL, simulation_loop, NULL);
    for(int i=0;i<THREADS;i++) {
        simulation_pools[i].running = true;
        simulation_pools[i].ants = &WorldRef()->ants[i*ANTS_PER_POOL];
        if(i != THREADS-1) { 
            simulation_pools[i].count = ANTS_PER_POOL;
        } else { // last loop
            simulation_pools[i].count = MAX_ANTS - ((THREADS-1)*ANTS_PER_POOL);
        }
        pthread_create(&simulation_pools[i].thread, NULL, ant_pool_loop, &simulation_pools[i]);
    }
}

void SimulationStop() {
    for(int i=0;i<THREADS;i++) {
        simulation_pools[i].running = false;
        pthread_join(simulation_pools[i].thread, NULL); 
    }
    simulation_running = false;
    pthread_join(simulation_thread, NULL);
}