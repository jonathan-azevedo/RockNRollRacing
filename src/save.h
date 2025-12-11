#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "car.h"
#include "enemy.h"
#include "itens.h"

//estrutura de dados para salvamento
typedef struct {
    CAR player;
    ENEMY enemies[MAX_ENEMIES];
    ITEM_MANAGER itemManager;
    int selectedMapIndex;
    int numLaps;
    int numEnemies;
    int selectedCarIndex;
} SaveData;

bool SaveGame(CAR *player, ENEMY enemies[], ITEM_MANAGER *itemManager, int mapIndex, int laps, int numEnemies, int carIndex); //cria uma cópia local de todos os dados do jogo, empacota em uma estrutura única e grava tudo de uma vez em um arquivo binário
bool LoadGame(CAR *player, ENEMY enemies[], ITEM_MANAGER *itemManager, int *mapIndex, int *laps, int *numEnemies, int *carIndex); //tenta abrir o arquivo de save,le o bloco de memória binária para uma estrutura temporária e distribui os dados de volta para as variáveis do jogo