#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "ant.h"
#include "map.h"
#include "config.h"
#include "world.h"

static inline void calculate_move_vector(Ant *ant) {
    ant->move = Vector2Rotate((Vector2){ant->speed, 0}, DEG2RAD * ant->direction);
}

static inline bool will_collide_with_wall(Ant *ant) {
    return WallAt(ant->position.x + ant->move.x, ant->position.y + ant->move.y);
}

static inline void set_direction(Ant *ant, float direction) {
    ant->direction = direction;
    if(ant->direction < 0) ant->direction += 360.0;
    else if(ant->direction >= 360.0) ant->direction -= 360.0;
}

static void handle_wall_collision(Ant *ant) {
    float old_direction = ant->direction;
    // float search_direction;
    int search_step = 1;
    while(true) {
        set_direction(ant, old_direction + search_step * Config.ant_wall_seach_angle * ant->wall_search_start);
        calculate_move_vector(ant);
        if(! will_collide_with_wall(ant)) break;
        if(search_step > 0) search_step *= -1;
        else search_step = (search_step*-1)+1;
    }
}

static void change_direction(Ant *ant, int pool_idx) {
    int type = P_NEST;
    if(ant->food == 0) type = P_FOOD;
    float pheromone_direction = SensePheromones(ant->position, ant->direction, type, pool_idx) * RAD2DEG;
    if(pheromone_direction >= 0.0) set_direction(ant, pheromone_direction + GetRandomValue(-Config.ant_angle_variation*10,Config.ant_angle_variation*10) / 100.0);
    else set_direction(ant, ant->direction + GetRandomValue(-Config.ant_angle_variation*10,Config.ant_angle_variation*10) / 10.0);
    calculate_move_vector(ant);
}

static inline void move(Ant *ant, int pool_idx) {
    ant->step++;
    if(ant->step % ant->angle_step == 0) change_direction(ant, pool_idx);
    if(will_collide_with_wall(ant)) handle_wall_collision(ant);
    ant->position = Vector2Add(ant->position, ant->move);
}

static inline void turnaround(Ant *ant) {
    set_direction(ant, ant->direction + 180.0);
    calculate_move_vector(ant);
}

static inline void check_for_food(Ant *ant) {
    if(FoodAt(ant->position.x, ant->position.y)) {
        // printf("food\n");
        ant->food = 1;
        turnaround(ant);
        RemoveFood(ant->position.x, ant->position.y);
    }
}

static inline void check_for_nest(Ant *ant) {
    float distance = Vector2Distance(ant->position, ant->nest_position);
    if(distance > Config.ant_nest_sense_radius) return;
    NestAddFood(ant->food);
    ant->food = 0;
    turnaround(ant);
}

Ant GetNewAnt(Vector2 nest_position) {
    Ant ant = (Ant){
        .position = nest_position,
        .nest_position = nest_position,
        .direction = (float)GetRandomValue(0,359),
        .speed = Config.ant_max_speed,
        .angle_step = Config.ant_angle_step,
        .food = 0,
        .spawned = false,
        .wall_search_start = (GetRandomValue(0,1)==0 ? 1 : -1),
    };
    calculate_move_vector(&ant);
    return ant;
}

void AntUpdate(Ant *ant, int pool_idx) {
    if(!ant->spawned) return;
    move(ant, pool_idx);
    if(ant->food == 0) { 
        check_for_food(ant);
        DropPheromone(ant->position, P_NEST, Config.pheromone_drop_strength);
    } else {
        check_for_nest(ant);
        DropPheromone(ant->position, P_FOOD, Config.pheromone_drop_strength);
    }
}

