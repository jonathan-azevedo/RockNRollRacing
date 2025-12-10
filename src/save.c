#include "save.h"

bool SaveGame(CAR *player, ENEMY enemies[], ITEM_MANAGER *itemManager, int mapIndex, int laps, int numEnemies, int carIndex) {
    SaveData data = {0};
    
    data.player = *player;
    for(int i = 0; i < MAX_ENEMIES; i++) {
        data.enemies[i] = enemies[i];
    }
    data.itemManager = *itemManager;
    data.selectedMapIndex = mapIndex;
    data.numLaps = laps;
    data.numEnemies = numEnemies;
    data.selectedCarIndex = carIndex;

    FILE *file = fopen("savegame.dat", "wb");
    if (file != NULL) {
        size_t written = fwrite(&data, sizeof(SaveData), 1, file);
        fclose(file);
        // Verifica se escreveu 1 elemento do tamanho da struct
        return (written == 1);
    } 
    return false; // Erro ao abrir arquivo
}

bool LoadGame(CAR *player, ENEMY enemies[], ITEM_MANAGER *itemManager, int *mapIndex, int *laps, int *numEnemies, int *carIndex) {
    FILE *file = fopen("savegame.dat", "rb");
    if (file == NULL) {
        return false;
    }

    SaveData data;
    size_t result = fread(&data, sizeof(SaveData), 1, file);
    fclose(file);

    if (result != 1) {
        return false;
    }

    *player = data.player;
    for(int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i] = data.enemies[i];
    }
    *itemManager = data.itemManager;
    *mapIndex = data.selectedMapIndex;
    *laps = data.numLaps;
    *numEnemies = data.numEnemies;
    *carIndex = data.selectedCarIndex;

    return true;
}