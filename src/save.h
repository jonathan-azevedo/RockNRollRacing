#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "car.h"
#include "enemy.h"
#include "itens.h"

typedef struct {
    CAR player;
    ENEMY enemies[MAX_ENEMIES];
    ITEM_MANAGER itemManager;
    int selectedMapIndex;
    int numLaps;
    int numEnemies;
    int selectedCarIndex;
} SaveData;

bool SaveGame(CAR *player, ENEMY enemies[], ITEM_MANAGER *itemManager, int mapIndex, int laps, int numEnemies, int carIndex);
bool LoadGame(CAR *player, ENEMY enemies[], ITEM_MANAGER *itemManager, int *mapIndex, int *laps, int *numEnemies, int *carIndex);