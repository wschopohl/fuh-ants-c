#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ANTS 500
#define FPS 60
#define THREADS 4

typedef struct Configuration {
    int threads;
    int fps;
    int max_ants;
    int spawn_rate; // ants per simulation second
    int ant_angle_step; // update ant direction every x simulation steps
    float ant_angle_variation; // max angle change
    float ant_max_speed; // pixel per simulation step
    float ant_wall_seach_angle; // increments in which to search for new direction when hitting a wall
    float ant_nest_sense_radius; // distance in which the nest is directly sensed
} Configuration;

#define PHEROMONE_TYPES 2
#define P_FOOD 0
#define P_NEST 1

#define P_FOOD_COLOR (Color){0,255,0,255}
#define P_NEST_COLOR (Color){255,0,0,255}


extern Configuration Config;

#endif