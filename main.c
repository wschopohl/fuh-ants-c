#include <stdio.h>
#include <stdlib.h>
#include "world.h"
#include "render_engine.h"
#include "simulation.h"

int main(void)
{
    WorldInit(1150, 680, "assets/maps/map_maze.png", "assets/maps/map_maze_food.png", (Vector2){.x=430,.y=390});

    SimulationStart();

    RenderEngineStart(); // sync call, only returns when raylib window is closed

    SimulationStop();
}