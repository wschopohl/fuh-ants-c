#include "config.h"

Configuration Config = {
    .threads = 4,
    .max_ants = 500,
    .spawn_rate = 1.0,
    .ant_angle_step = 10,
    .ant_angle_variation = 30.0,
    .ant_max_speed = 1.0,
    .ant_wall_seach_angle = 15.0,
    .ant_nest_sense_radius = 20,
    .pheromone_drop_strength = 20,
    .pheromone_decay_strength = 1,
    .pheromone_decay_rate = 10,
};