#include "config.h"

Configuration Config = {
    .threads = 4,
    .max_ants = 500,
    .spawn_rate = 10.0,
    .ant_angle_step = 10,
    .ant_angle_variation = 30.0,
    .sense_variation_factor = 1,
    .ant_max_speed = 1.0,
    .ant_wall_search_angle = 15.0,
    .ant_nest_radius = 20,
    .ant_nest_sense_radius = 100,
    .pheromone_map_scale = 2.0,
    .pheromone_drop_strength = 40,
    .pheromone_decline_step = 500,
    .pheromone_decay_strength = 1,
    .pheromone_decay_rate = 5,
    .ant_sense_distance = 50,
    .ant_field_of_view = 70.0
};