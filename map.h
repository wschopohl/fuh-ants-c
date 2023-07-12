#define ALPHA_THRESHOLD 100
#define GROUND_IMAGE "assets/ground/ground.png"

typedef struct Bitmap {
    int *data;
    int width;
} Bitmap;


void LoadWallBitMap(const char* filename);
void UnloadWallBitMap();
Image GetRenderBitMap();
bool WallAt(int x, int y);