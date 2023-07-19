#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include "render_engine.h"
#include "simulation.h"
#include "map.h"
#include "world.h"



static Texture2D map_texture;
static Texture2D walls_texture;
static Texture2D nest_texture;
static Texture2D ant_texture;
static Texture2D food_texture;
static Texture2D pheromone_texture[PHEROMONE_TYPES];
Vector2 ant_center;

static void init() { 
    InitWindow(WorldRef()->width, WorldRef()->height, "MAS Ant Simulation");
    SetTargetFPS(FPS);
    map_texture = LoadTexture(WorldRef()->map_filename);
    ant_texture = LoadTexture(ANT_SPRITE_FILE);
    nest_texture = LoadTexture(NEST_SPRITE_FILE);
    walls_texture = LoadTextureFromImage(GetWallImage());  
    food_texture = LoadTextureFromImage(GetFoodImage());
    for(int i=0; i < PHEROMONE_TYPES; i++) {
        pheromone_texture[i] = LoadTextureFromImage(GetPheromoneImage(i));
    }
    ant_center.x = ant_texture.width / 2;
    ant_center.y = ant_texture.height / 2;
}

static void draw_ants() {
    for(int i=0;i<MAX_ANTS;i++) {
        Ant *a = &WorldRef()->ants[i];
        if(a->spawned) {
            DrawTexturePro(ant_texture, (Rectangle){0, 0, ant_texture.width, ant_texture.height}, 
                (Rectangle){a->position.x, a->position.y, ant_texture.width, ant_texture.height},
                (Vector2){6,5}, a->direction, WHITE);
            if(a->food > 0) {
                DrawCircleV(a->position, 1.5, GREEN);
            }
        }
    }
}

float simulation_speed = 1.0;

static void print_stats() {
    char buffer[50];
    snprintf(buffer, 50, "FPS: %d\nSpeed: %.1f\nFood: %d", GetFPS(), simulation_speed, WorldRef()->nest_food);
    DrawRectangle(0,0,140,120,(Color){0,0,0,100});
    DrawText(buffer, 20, 20, 20, WHITE);
}

static void render_loop() {
    

    while (!WindowShouldClose())
    {
        // UpdateTexture(walls_texture, GetWallBitMap().data);
        UpdateTexture(food_texture, GetFoodImage().data);
        for(int i=0; i < PHEROMONE_TYPES; i++) {
            UpdateTexture(pheromone_texture[i], GetPheromoneImage(i).data);
        }
        // if (IsKeyDown(KEY_RIGHT)) ref.x += 1;
        // if (IsKeyDown(KEY_LEFT)) ref.x -= 1;
        if (IsKeyDown(KEY_UP)) simulation_speed += 0.1;
        if (IsKeyDown(KEY_DOWN)) simulation_speed -= 0.1;
        if(simulation_speed < 0.1) simulation_speed = 0.1; // some division by zero bug or something
        SetSimulationSpeed(simulation_speed);

        
        BeginDrawing();
        {
            ClearBackground(LIGHTGRAY);
            // DrawTexture(map_texture, 0, 0, WHITE);
            DrawTexture(pheromone_texture[P_FOOD], 0, 0, WHITE);
            DrawTexture(pheromone_texture[P_NEST], 0, 0, WHITE);
            DrawTexture(walls_texture, 0, 0, WHITE);
            DrawTexture(food_texture, 0, 0, WHITE);
            DrawTexture(nest_texture, WorldRef()->nest_position.x - nest_texture.width/2, WorldRef()->nest_position.y - nest_texture.height/2, WHITE);
            draw_ants();
            print_stats();
        }
        
        EndDrawing();
    }
    
    CloseWindow();
}

void RenderEngineStart() { 
    init();
    render_loop();
}