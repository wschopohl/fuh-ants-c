#ifndef ANT_H
#define ANT_H

#include <raylib.h>
#include "config.h"

typedef struct Ant {
    bool spawned;
    Vector2 position;
    Vector2 move;
    float speed;
    float direction;
    int food;
    int step;
    int angle_step;
    int pheromone_strength;
    int wall_search_start;
    Vector2 nest_position;
} Ant;

Ant GetNewAnt(Vector2 position);
void AntUpdate(Ant *ant, int pool_idx);

#endif