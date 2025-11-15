#include "map.h"


MAP loadMap(const char *fileName){
    MAP map = {0};
    char path[60];
    snprintf(path, sizeof(path), "Assets/Tracks/%s", fileName);
    FILE *file = fopen(path, "r");
    if(file == NULL){
        printf("Failed to open map file");
        return map;
    }
    printf("MAPA CARREGADO\n");
    for(int y = 0; y < MAP_HEIGHT; y++){
        fgets(map.tiles[y], MAP_WIDTH + 2, file); 
    }
    fclose(file);
    return map;
}

void drawMap(MAP *map){
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char tile = map->tiles[y][x];
            if (tile == ' '){
                DrawRectangle(x * TILE_WIDTH, y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, BLACK);
            }
        }
    }
}