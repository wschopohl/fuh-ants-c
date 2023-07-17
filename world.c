#include <stdio.h>
#include <raylib.h>
#include "world.h"

static World world;

void WorldInit(int width, int height, const char* map_file, const char* map_food, Vector2 nest_position) {
    world.width = width;
    world.height = height;
    world.map_filename = map_file;
    world.food_filename = map_food;
    world.nest_position = nest_position;
    for(int i=0;i<MAX_ANTS;i++) {
        world.ants[i].spawned = false;
    }
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

void NestAddFood(int amount) {
    world.nest_food += amount;
    // printf("Nest Food: %d\n", world.nest_food);
}