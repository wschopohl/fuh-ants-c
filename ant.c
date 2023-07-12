#include "ant.h"

Ant GetNewAnt(Vector2 position) {
    return (Ant){.position = position,
        .direction = (float)GetRandomValue(0,360),
        .food = 0,
        .spawned = false
    };
}

void AntMove(Ant *ant) {
    if(!ant->spawned) return;
    ant->position.x++;
}