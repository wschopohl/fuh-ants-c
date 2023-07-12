#include <stdio.h>
#include "world.h"
#include "render_engine.h"
#include "simulation.h"

int main(void)
{
    WorldInit(1280, 720, "assets/maps/map_maze.png", (Vector2){.x=100,.y=100});
    WorldAddFood((FoodCluster){.position={.x=1000,.y=500},.amount=500});

    SimulationStart();
    
    RenderEngineStart(); // sync call, only returns when raylib window is closed

    SimulationStop();
}
