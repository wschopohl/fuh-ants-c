#ifndef WORLD_H
#define WORLD_H

#include <raylib.h>
#include "config.h"
#include "ant.h"

typedef struct FoodCluster {
    Vector2 position;
    int amount;
} FoodCluster;

typedef struct World {
    int width;
    int height;
    Vector2 nest_position;
    const char* map_filename;
    FoodCluster food_clusters[MAX_FOOD_CLUSTERS];
    int food_cluster_count;
    Ant ants[MAX_ANTS];
    int ant_count;
} World;

void WorldInit(int width, int height, const char* map_file, Vector2 nest_position);
void WorldAddFood(FoodCluster food);
void WorldAddAnt(Ant ant);
Vector2 WorldNestPosition();
int WorldWidth();
int WorldHeight();
World* WorldRef();

#endif