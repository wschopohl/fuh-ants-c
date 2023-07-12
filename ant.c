#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "ant.h"
#include "map.h"
#include "config.h"

static inline void calculate_move_vector(Ant *ant) {
    ant->move = Vector2Rotate((Vector2){ant->speed, 0}, DEG2RAD * ant->direction);
}

static inline bool will_collide_with_wall(Ant *ant) {
    return WallAt(ant->position.x + ant->move.x, ant->position.y + ant->move.y);
}

static void handle_wall_collision(Ant *ant) {
    float old_direction = ant->direction;
    // float search_direction;
    int search_step = 1;
    while(true) {
        ant->direction = old_direction + search_step * ANT_WALL_SEARCH_ANGLE * ant->wall_search_start;
        calculate_move_vector(ant);
        if(! will_collide_with_wall(ant)) break;
        if(search_step > 0) search_step *= -1;
        else search_step = (search_step*-1)+1;
    }
}

static void change_direction(Ant *ant) {
    ant->direction += GetRandomValue(-ANT_ANGLE_VARIATION*10,ANT_ANGLE_VARIATION*10) / 10.0;
    calculate_move_vector(ant);
}

// def calculateMoveVectors(self):
//         olddirection = self.direction
//         search_angle = 0
//         search_invert = (1 if self.wallSearchStart == 0 else -1)
//         iteration = 0
//         while True:
//             if self.sprite == None: break
//             if self.nest.world.map == None: break
//             future_x = self.position[0] + Config.AntMoveDistance * Config.AntAngleStep * math.cos(math.radians(self.direction))
//             future_y = self.position[1] -Config.AntMoveDistance * Config.AntAngleStep * math.sin(math.radians(self.direction))
//             if self.nest.world.collision.checkPointMask((future_x, future_y), self.nest.world.map.sprite) == False: break
//             # search_angle = randint(1,24) * 15
//             # self.direction = (olddirection + (search_angle)) % 360
//             if iteration % 2 == 0: 
//                 search_angle += Config.AntWallSearchAngle
//                 self.direction = (olddirection + (search_angle * search_invert)) % 360
//             else:
//                 self.direction = (olddirection + (search_angle * search_invert * -1)) % 360
//             iteration += 1
//             if iteration > 24: 
//                 self.suicide()
//                 return
//         self.dx = Config.AntMoveDistance * math.cos(math.radians(self.direction))
//         self.dy = -Config.AntMoveDistance * math.sin(math.radians(self.direction))

Ant GetNewAnt(Vector2 position) {
    Ant ant = (Ant){.position = position,
        .direction = (float)GetRandomValue(0,360),
        .speed = ANT_MAX_SPEED,
        .angle_step = ANT_ANGLE_STEP,
        .food = 0,
        .spawned = false,
        .wall_search_start = (GetRandomValue(0,1)==0 ? 1 : -1),
    };
    calculate_move_vector(&ant);
    return ant;
}

void AntMove(Ant *ant) {
    if(!ant->spawned) return;
    ant->step++;
    if(ant->step % ant->angle_step == 0) change_direction(ant);
    if(will_collide_with_wall(ant)) handle_wall_collision(ant);
    ant->position = Vector2Add(ant->position, ant->move);
}