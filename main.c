#include <stdio.h>
#include <stdlib.h>
#include "world.h"
#include "render_engine.h"
#include "simulation.h"

int main(void)
{
    SetTraceLogLevel(LOG_ERROR);
    
    // WorldInit(1150, 680, "assets/maps/map_maze_2.png", "assets/maps/map_maze_food_3.png", (Vector2){.x=430,.y=390});
    WorldInit(1150, 680, "assets/maps/map_dyno.png", "assets/maps/map_dyno_food.png", (Vector2){.x=187,.y=121});

    SimulationStart();

    RenderEngineStart(); // sync call, only returns when raylib window is closed

    SimulationStop();
}