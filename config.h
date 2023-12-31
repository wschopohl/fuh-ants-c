#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ANTS 500
#define FPS 60
#define THREADS 4
#define SENSE_MATRIX_ANGLES 36

typedef struct Configuration {
    int threads;
    int max_ants;
    float spawn_rate; // ants per simulation second
    int ant_angle_step; // update ant direction every x simulation steps
    float ant_angle_variation; // max angle change
    float sense_variation_factor; // max angle change when sensing pheromones
    float ant_max_speed; // pixel per simulation step
    float ant_wall_search_angle; // increments in which to search for new direction when hitting a wall
    float ant_nest_radius; // radius of the nest
    float ant_nest_sense_radius; // distance in which the nest is directly sensed
    float pheromone_map_scale;
    int pheromone_drop_strength; // strength that ants drop pheromones at
    int pheromone_decline_step; // pheromone strength declines every x steps by one
    int pheromone_decay_strength; // strength that pheromones decay with
    float pheromone_decay_rate; // pheromone decay calls per simulation second
    float ant_field_of_view; // angle in which pheromones are detected
    int ant_sense_distance; // distance up to which pheromones are detected
} Configuration;

#define PHEROMONE_TYPES 2
#define P_FOOD 0
#define P_NEST 1

#define P_FOOD_COLOR (Color){0,0,255,0}
#define P_NEST_COLOR (Color){255,0,0,0}


extern Configuration Config;

#endif