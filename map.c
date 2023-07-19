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
static float* sense_matrix[SENSE_MATRIX_ANGLES];
static float* calc_matrix[THREADS];

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

static inline int matrix_position_to_index(int x, int y) {
    return ((y + Config.ant_sense_distance) * (Config.ant_sense_distance * 2 + 1) + (x + + Config.ant_sense_distance));
}

static void print_matrix(float* matrix) {
    for(int y = -Config.ant_sense_distance; y <= Config.ant_sense_distance; y++) {
        for(int x = -Config.ant_sense_distance; x <= Config.ant_sense_distance; x++) {
            if(x == 0 && y == 0) {
                printf("+++ ");
                continue;
            }
            if(matrix[matrix_position_to_index(x,y)] == 0.0) {
                printf("    ");
                continue;
            }
            
            // printf("%03d ", (int)roundf(RAD2DEG * sense_matrix[angle_idx][matrix_position_to_index(x,y)]));
            printf("%.1f ", matrix[matrix_position_to_index(x,y)]);
        }
        printf("\n");
    }
}

static void generate_calculation_matrices() {
    for(int i=0; i < THREADS; i++) {
        calc_matrix[i] = MemAlloc(sizeof(float) * (Config.ant_sense_distance*2+1) * (Config.ant_sense_distance*2+1));
    }
}

void GeneratePheromoneSenseMatrices() {
    for(int angle_idx=0; angle_idx < SENSE_MATRIX_ANGLES; angle_idx++) {
        sense_matrix[angle_idx] = MemAlloc(sizeof(float) * (Config.ant_sense_distance*2+1) * (Config.ant_sense_distance*2+1));
    }

    float epsilon = 0.001;
    float distance, angle, factor;
    float angle_delta = DEG2RAD * (360.0 / SENSE_MATRIX_ANGLES);
    for(int y = -Config.ant_sense_distance; y <= Config.ant_sense_distance; y++) {
        for(int x = -Config.ant_sense_distance; x <= Config.ant_sense_distance; x++) {
            distance = sqrtf(powf(x,2.0)+powf(y,2.0));
            angle = (atan2f(y,x));
    
            for(int angle_idx=0; angle_idx < SENSE_MATRIX_ANGLES; angle_idx++) {
                float matrix_angle = angle; // fabsf(angle);
                matrix_angle = fabsf(matrix_angle - angle_idx * angle_delta);
                if(matrix_angle > PI) matrix_angle = fabs(2.0 * PI - matrix_angle);
                if(distance > Config.ant_sense_distance + epsilon) {
                    sense_matrix[angle_idx][matrix_position_to_index(x,y)] = 0.0;
                    continue;
                }
                if(matrix_angle > DEG2RAD * Config.ant_field_of_view + epsilon) {
                    sense_matrix[angle_idx][matrix_position_to_index(x,y)] = 0.0;
                    continue;
                }
                factor = 1.0 - (matrix_angle / (DEG2RAD * Config.ant_field_of_view));
                // factor = distance;
                sense_matrix[angle_idx][matrix_position_to_index(x,y)] = factor;
            }
        }
    }

    // debug print matrices
    // for(int angle_idx=0; angle_idx < SENSE_MATRIX_ANGLES; angle_idx++) {
    //     printf("Degree: %d\n", angle_idx);
    //     print_matrix(sense_matrix[angle_idx]);
    // }
    generate_calculation_matrices();
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

static void unload_calculation_matrices() {
    for(int i=0; i < THREADS; i++) {
        MemFree(calc_matrix[i]);
    }
}

void UnloadPheromoneSenseMatrices() {
    for(int angle_idx=0; angle_idx < SENSE_MATRIX_ANGLES; angle_idx++) {
        MemFree(sense_matrix[angle_idx]);
    }
    unload_calculation_matrices();
}

float SensePheromones(Vector2 position, float direction, int type, int pool_idx) {
    int direction_idx = (int)roundf(direction / (360 / SENSE_MATRIX_ANGLES)) % SENSE_MATRIX_ANGLES;
    int ant_x = (int)position.x;
    int ant_y = (int)position.y;
    float total_weight = 0.0;
    float* matrix = calc_matrix[pool_idx];
    float center_of_mass_x = 0.0, center_of_mass_y = 0.0;
    
    for(int y = -Config.ant_sense_distance; y <= Config.ant_sense_distance; y++) {
        int map_row_idx = ((ant_y + y) * walls.pixel_width + ant_x -Config.ant_sense_distance) * 4;
        for(int x = -Config.ant_sense_distance; x <= Config.ant_sense_distance; x++) {
            int idx = matrix_position_to_index(x,y);
            if(ant_x + x < 0 || ant_y + y < 0 || ant_x + x >= walls.pixel_width || ant_y + y >= walls.pixel_height) {
                matrix[idx] = 0.0;
            } else {
                uint8_t *map_intensity = (uint8_t*)pheromone_map_render[type].data + (map_row_idx + 3);
                matrix[idx] = sense_matrix[direction_idx][idx] * *map_intensity;
                total_weight += matrix[idx];
                center_of_mass_x += matrix[idx] * x;
                center_of_mass_y += matrix[idx] * y;
            }
            map_row_idx+=4;
        }
    }

    if(total_weight == 0.0) return -1.0;

    center_of_mass_x /= total_weight;
    center_of_mass_y /= total_weight;

    float center_of_mass_angle = atan2f(center_of_mass_y, center_of_mass_x);
    if(center_of_mass_angle < 0) center_of_mass_angle += 2 * PI;

    // debug print
    // print_matrix(matrix);
    // printf("total weight %f, com_x: %.1f, com_y: %.1f, view dir: %.1f, sense_dir: %.1f\n", total_weight, center_of_mass_x, center_of_mass_y, direction, RAD2DEG * center_of_mass_angle);

    return center_of_mass_angle;
}