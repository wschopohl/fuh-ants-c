#include <raylib.h>
#include <math.h>
#include <inttypes.h>
#include <stdio.h>
#include "map.h"

#define BITS_PER_INT (sizeof(int) * 8)

static int *map_bits;
static int int_width;
static Image map_render;

static inline int byte_index(int x) {
    return x / BITS_PER_INT;
}

static inline int bit_index(int x) {
    return (x % BITS_PER_INT);
}

bool WallAt(int x, int y) {
    return (map_bits[y*int_width+byte_index(x)] & (1 << bit_index(x)));
}

void LoadWallBitMap(const char* filename) {
    Image map = LoadImage(filename);
    Image ground = LoadImage(GROUND_IMAGE);
    map_render = GenImageColor(map.width, map.height, (Color){0,0,0,0});
    int_width = ceil((float)map.width / BITS_PER_INT);
    map_bits = MemAlloc(int_width * sizeof(int) * map.height);
    Color *ground_pixel = ground.data;
    Color *map_pixel = map.data;
    for(int y = 0; y < map.height; y++) {
        for(int x = 0; x < map.width; x++) {
            Color combined = *(ground_pixel+y*ground.width+x);
            combined.a = map_pixel->a;
            ImageDrawPixel(&map_render, x, y, combined);
            if(map_pixel->a > ALPHA_THRESHOLD) map_bits[y*int_width+byte_index(x)] |= (1 << bit_index(x));
            map_pixel++;
        }
    }

    UnloadImage(map);
    UnloadImage(ground);
}

Image GetRenderBitMap() {
    // for(int y = 0; y < map_render.height; y++) {
    //     for(int x = 0; x < map_render.width; x++) {
    //         if(WallAt(x,y)) ImageDrawPixel(&map_render, x, y, RED); // color for debug purpose
    //         else ImageDrawPixel(&map_render, x, y, WHITE); // color for debug purpose
    //     }
    // }
    return map_render;
}

void UnloadWallBitMap() {
    MemFree(map_bits);
    UnloadImage(map_render);
}