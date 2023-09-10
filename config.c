#include "config.h"

Configuration Config = {
    .threads = 4,
    .max_ants = 5000,
    .spawn_rate = 10.0,
    .ant_angle_step = 5,
    .ant_angle_variation = 30.0,
    .sense_variation_factor = 0.5,
    .ant_max_speed = 1.0,
    .ant_wall_search_angle = 15.0,
    .ant_nest_radius = 20,
    .ant_nest_sense_radius = 100,
    .pheromone_map_scale = 1.0,
    .pheromone_drop_strength = 40,
    .pheromone_decline_step = 200,
    .pheromone_decay_strength = 2,
    .pheromone_decay_rate = 2,
    .ant_sense_distance = 30,
    .ant_field_of_view = 180.0
};