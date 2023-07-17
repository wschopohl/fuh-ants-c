#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "world.h"
#include "simulation.h"
#include "ant.h"
#include "map.h"
#include "timer.h"

static pthread_t simulation_thread;
static bool simulation_running = true;
static SimulationPool simulation_pools[THREADS];

static float speed = 1.0, next_speed = 1.0;
static Timer simulation_loop_timer, ant_loop_timer;

static void inline sleep_or_pause(Timer *timer) {
    speed = next_speed;
    if(speed > 0.0) {
        int sleeptime = (1000000 / (FPS * speed)) - get_lap_time_us(timer);
        if(sleeptime < 0) sleeptime = 0;
        usleep(sleeptime);
        reset_timer(timer);
    }
    else while(speed <= 0.0 && simulation_running) {
        usleep(50000);
        speed = next_speed;
    }
}

static void* simulation_loop(void* args) {
    Timer spawn_timer = get_timer();
    Timer decay_timer = get_timer();

    while(simulation_running) {
        if(get_lap_time_us(&spawn_timer) > 1000000 / (Config.spawn_rate * speed)) {
            WorldAddAnt(GetNewAnt(WorldRef()->nest_position));
            reset_timer(&spawn_timer);
        }
        if(get_lap_time_us(&decay_timer) > 1000000 / (Config.pheromone_decay_rate * speed)) {
            PheromoneDecay(1);
            reset_timer(&decay_timer);
        }
        sleep_or_pause(&simulation_loop_timer);
    }
    stop_timer(&simulation_loop_timer); // just here to ignore compiler warning, unused function
    return NULL;
}

static void* ant_pool_loop(void* pool_args) {
    SimulationPool *pool = (SimulationPool*)pool_args;
    while(simulation_running) {
        for(int i=0;i<pool->count;i++) {
            AntUpdate(&pool->ants[i]);
        }
        sleep_or_pause(&ant_loop_timer);
    }
    return NULL;
}

void SimulationStart() {
    LoadWallBitMap(WorldRef()->map_filename);
    LoadFoodBitMap(WorldRef()->food_filename);
    GeneratePheromoneMaps();
    GeneratePheromoneSenseMatrices();

    pthread_create(&simulation_thread, NULL, simulation_loop, NULL);
    for(int i=0;i<THREADS;i++) {
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
    simulation_running = false;
    for(int i=0;i<THREADS;i++) {
        pthread_join(simulation_pools[i].thread, NULL); 
    }
    pthread_join(simulation_thread, NULL);
    UnloadWallBitMap();
    UnloadPheromoneMaps();
    UnloadPheromoneSenseMatrices();
}

float GetSimulationSpeed() {
    return speed;
}

void SetSimulationSpeed(float s) {
    next_speed = s;
    if(next_speed < 0.0) next_speed = 0.0;
}