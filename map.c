#include <raylib.h>
#include <math.h>
#include <inttypes.h>
#include <stdio.h>
#include <pthread.h>
#include "map.h"
#include "config.h"

#define BITS_PER_INT (sizeof(int) * 8)

static Bitmap walls;
static Image walls_render;
static Bitmap food;
static Image food_render;
static Image pheromone_map_render[PHEROMONE_TYPES];
static const Color pheromone_color[PHEROMONE_TYPES] = {P_FOOD_COLOR, P_NEST_COLOR};

// pthread_mutex_t pheromone_map_lock[PHEROMONE_TYPES];

static inline int byte_index(int x) {
    return x / BITS_PER_INT;
}

static inline int bit_index(int x) {
    return (x % BITS_PER_INT);
}

static inline bool bit_at(Bitmap bitmap, int x, int y) {
    return (bitmap.data[y*bitmap.int_width+byte_index(x)] & (1 << bit_index(x)));
}

bool WallAt(int x, int y) {
    return (walls.data[y*walls.int_width+byte_index(x)] & (1 << bit_index(x)));
}

bool FoodAt(int x, int y) {
    return (food.data[y*food.int_width+byte_index(x)] & (1 << bit_index(x)));
}

void RemoveFood(int x, int y) {
    food.data[y*food.int_width+byte_index(x)] &= ~(1 << bit_index(x));
    ImageDrawPixel(&food_render, x, y, (Color){0,0,0,0});
}

static Bitmap load_bitmap(const char* filename) {
    Bitmap bitmap;
    Image map = LoadImage(filename);
    bitmap.pixel_width = map.width;
    bitmap.pixel_height = map.height;
    bitmap.int_width = ceil((float)map.width / BITS_PER_INT);
    bitmap.data = MemAlloc(bitmap.int_width * sizeof(int) * bitmap.pixel_height);
    Color *map_pixel = map.data;
    for(int y = 0; y < map.height; y++) {
        for(int x = 0; x < map.width; x++) {
            if(map_pixel->a > ALPHA_THRESHOLD) bitmap.data[y*bitmap.int_width+byte_index(x)] |= (1 << bit_index(x));
            map_pixel++;
        }
    }
    UnloadImage(map);
    return bitmap;
}

static Image from_bitmap(Bitmap bitmap, Color color) {
    Image image = GenImageColor(bitmap.pixel_width, bitmap.pixel_height, (Color){0,0,0,0});
    for(int y = 0; y < bitmap.pixel_height; y++) {
        for(int x = 0; x < bitmap.pixel_width; x++) {
            if(bit_at(bitmap,x,y)) ImageDrawPixel(&image, x, y, color);
        }
    }
    return image;
}

void LoadWallBitMap(const char* filename) {
    walls = load_bitmap(filename);
    Image map = LoadImage(filename);
    Image ground = LoadImage(GROUND_IMAGE);
    walls_render = GenImageColor(map.width, map.height, (Color){0,0,0,0});
    Color *ground_pixel = ground.data;
    Color *map_pixel = map.data;
    for(int y = 0; y < map.height; y++) {
        for(int x = 0; x < map.width; x++) {
            Color combined = *(ground_pixel+y*ground.width+x);
            combined.a = map_pixel->a;
            ImageDrawPixel(&walls_render, x, y, combined);
            map_pixel++;
        }
    }

    UnloadImage(map);
    UnloadImage(ground);
}

void LoadFoodBitMap(const char* filename) {
    food = load_bitmap(filename);
    food_render = from_bitmap(food, GREEN);
}

static inline int position_to_index(Vector2 position) {
    return (int)((int)position.y * walls.pixel_width + (int)position.x);
}

void GeneratePheromoneMaps() {
    for(int type=0; type<PHEROMONE_TYPES; type++) {
        pheromone_map_render[type] = GenImageColor(walls.pixel_width, walls.pixel_height, pheromone_color[type]);
    }
}

void DropPheromone(Vector2 position, int type, int strength) {
    // pthread_mutex_lock(&pheromone_map_lock[type]);
    int idx = position_to_index(position);
    uint8_t *alpha = (uint8_t*)pheromone_map_render[type].data + (idx*4+3);
    if(*alpha + strength > 255) *alpha = 255;
    else *alpha += strength;
}

void PheromoneDecay(int strength) {
    for(int type=0; type < PHEROMONE_TYPES; type++) {
        for(int idx = 0; idx < walls.pixel_width * walls.pixel_height; idx++) {
            uint8_t *alpha = (uint8_t*)pheromone_map_render[type].data + (idx*4+3);
            if(*alpha - strength < 0) *alpha = 0;
            else *alpha -= strength;
        }
    }
}

Image GetWallImage() {
    // for(int y = 0; y < walls_render.height; y++) {
    //     for(int x = 0; x < walls_render.width; x++) {
    //         if(WallAt(x,y)) ImageDrawPixel(&walls_render, x, y, RED); // color for debug purpose
    //         else ImageDrawPixel(&walls_render, x, y, WHITE); // color for debug purpose
    //     }
    // }
    return walls_render;
}

Image GetFoodImage() {
    return food_render;
}

Image GetPheromoneImage(int idx) {
    return pheromone_map_render[idx];
}

void UnloadWallBitMap() {
    MemFree(walls.data);
    MemFree(food.data);
    UnloadImage(walls_render);
    UnloadImage(food_render);
}

void UnloadPheromoneMaps() {
    for(int type=0; type<PHEROMONE_TYPES; type++) {
        UnloadImage(pheromone_map_render[type]);
        // pthread_mutex_destroy(&pheromone_map_lock[type]);
    }
}