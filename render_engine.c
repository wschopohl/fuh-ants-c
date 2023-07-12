#include <stdio.h>
#include <raylib.h>
#include "render_engine.h"
#include "world.h"

static Texture2D map;
static Texture2D nest;
static Texture2D ant;

static void init() {
    InitWindow(WorldRef()->width, WorldRef()->height, "MAS Ant Simulation");
    SetTargetFPS(FPS);
    map = LoadTexture(WorldRef()->map_filename);
    ant = LoadTexture(ANT_SPRITE_FILE);
    nest = LoadTexture(NEST_SPRITE_FILE);
}

static void render_loop() {
    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(LIGHTGRAY);
            DrawTexture(map, 0, 0, WHITE);
            DrawTexture(nest, WorldRef()->nest_position.x - nest.width/2, WorldRef()->nest_position.y - nest.height/2, WHITE);
            for(int i=0;i<MAX_ANTS;i++) {
                Ant *a = &WorldRef()->ants[i];
                if(a->spawned) {
                    // DrawTexture(ant, a->position.x, a->position.y, WHITE);
                    DrawTextureEx(ant, a->position, a->direction, 1.0, WHITE);
                }
            }
        }
        EndDrawing();
    }
    
    CloseWindow();
}

void RenderEngineStart() { 
    init();
    render_loop();
}