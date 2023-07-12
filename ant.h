#ifndef ANT_H
#define ANT_H

#include <raylib.h>

typedef struct Ant {
    bool spawned;
    Vector2 position;
    float direction;
    int food;
} Ant;

Ant GetNewAnt(Vector2 position);
void AntMove(Ant *ant);

#endif