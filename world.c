#include <raylib.h>
#include "world.h"

static World world;

void WorldInit(int width, int height, const char* map_file, Vector2 nest_position) {
    world.width = width;
    world.height = height;
    world.map_filename = map_file;
    world.nest_position = nest_position;
    for(int i=0;i<MAX_ANTS;i++) {
        world.ants[i].spawned = false;
    }
}

void WorldAddFood(FoodCluster food_cluster) {
    if(world.food_cluster_count >= MAX_FOOD_CLUSTERS) return;
    world.food_clusters[world.food_cluster_count++] = food_cluster;
}

void WorldAddAnt(Ant ant) {
    if(world.ant_count >= MAX_ANTS) return;
    for(int i=0;i<MAX_ANTS;i++) {
        if(world.ants[i].spawned == false) {
            world.ants[i] = ant;
            world.ants[i].spawned = true;
            world.ant_count++;
            return;
        }
    }
}

void WorldKillAnt(Ant *ant) {
    ant->spawned = false;
    world.ant_count--;
}

World* WorldRef() {
    return &world;
}