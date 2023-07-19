#ifndef WORLD_H
#define WORLD_H

#include <raylib.h>
#include "config.h"
#include "ant.h"

typedef struct World {
    int width;
    int height;
    Vector2 nest_position;
    int nest_food;
    const char* map_filename;
    const char* food_filename;
    Ant ants[MAX_ANTS];
    int ant_count;
} World;

void WorldInit(int width, int height, const char* map_file, const char* map_food, Vector2 nest_position);
void WorldAddAnt(Ant ant);
Vector2 WorldNestPosition();
int WorldWidth();
int WorldHeight();
void NestAddFood(int amount);
World* WorldRef();
void KillAnt(Ant *ant);

#endif