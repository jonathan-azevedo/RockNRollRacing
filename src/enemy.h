#pragma once
#include "car.h"
#include "map.h"

typedef struct {
    CAR vehicle;
    float turnTimer;
    float turnDuration;
    int turnDirection;
    //  um ID para diferenciar os inimigos se precisar
    int id;
} ENEMY;

//  par�metro 'offsetIndex' para eles n�o nascerem grudados
void initEnemy(ENEMY *enemy, MAP *map, Texture2D texture, int offsetIndex);
void updateEnemy(ENEMY *enemy, MAP *map);
void drawEnemy(ENEMY *enemy);
float getWallDistance(MAP *map, float startX, float startY, float angleDeg);
