#define ALPHA_THRESHOLD 100
#define GROUND_IMAGE "assets/ground/ground.png"

typedef struct Bitmap {
    int *data;
    int int_width;
    int pixel_width;
    int pixel_height;
} Bitmap;


void LoadFoodBitMap(const char* filename);
void LoadWallBitMap(const char* filename);
void UnloadWallBitMap();
Image GetWallImage();
Image GetFoodImage();
Image GetPheromoneImage(int idx);
bool WallAt(int x, int y);
bool FoodAt(int x, int y);
void RemoveFood(int x, int y);
void GeneratePheromoneMaps();
void UnloadPheromoneMaps();
void DropPheromone(Vector2 position, int type, int strength);
float SenseFood(Vector2 position, int pool_idx);
float SensePheromones(Vector2 position, float direction, int type, int pool_idx);
void PheromoneDecay(int strength);
void GeneratePheromoneSenseMatrices();
void UnloadPheromoneSenseMatrices();
void SetNest(Vector2 position);