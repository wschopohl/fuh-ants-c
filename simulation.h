#include <pthread.h>
#include "world.h"
#include "config.h"
#include "ant.h"

#define ANTS_PER_POOL ((int)(MAX_ANTS / THREADS))

typedef struct SimulationPool {
    pthread_t thread;
    bool running;
    Ant *ants;
    int count;
} SimulationPool;

void SimulationSetWorld(World *w);
void SimulationStart(); // Starts the simulation thread
void SimulationStop();
float GetSimulationSpeed();
void SetSimulationSpeed(float s);