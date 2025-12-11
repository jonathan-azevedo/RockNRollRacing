#include "save.h"

bool SaveGame(CAR *player, ENEMY enemies[], ITEM_MANAGER *itemManager, int mapIndex, int laps, int numEnemies, int carIndex) {
    //salva num arquivo binario
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
        return (written == 1);
    } 
    return false; 
}

bool LoadGame(CAR *player, ENEMY enemies[], ITEM_MANAGER *itemManager, int *mapIndex, int *laps, int *numEnemies, int *carIndex) {
    //carrega do arquivo binario salvo
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