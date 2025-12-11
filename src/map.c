#include "map.h"


MAP loadMap(const char *fileName){
    //constroi o caminho do arquivo, abre o texto do mapa e preenche a matriz de tiles linha por linha 
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
        fgets(map.tiles[y], MAP_WIDTH + 5, file); 
    }
    fclose(file);
    return map;
}
void getInitialPosition(MAP *map, int *initialPosx, int *initialPosy){
    //varre a matriz em busca do marcador 'j' e converte sua posição para coordenadas de pixel para o spawn do carro
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if(map->tiles[y][x] == 'j'){
                *initialPosx = (x * TILE_SIZE) + (TILE_SIZE / 2);
                *initialPosy = (y * TILE_SIZE) + (TILE_SIZE / 2);
            }
        }
    }
}
void drawMap(MAP *map, GAME_TEXTURES *textures){
    //percorre a matriz e desenha a textura correspondente,asfalto ou linha de chegada,na posição correta da tela baseada no tamanho do tile
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char tile = map->tiles[y][x];
            int posX = x * TILE_SIZE;
            int posY = y * TILE_SIZE;
            switch(tile){
                case ' ':{
                    DrawTexture(textures->asphaltTexture, posX, posY, WHITE);
                    break;
                }
                case 'j':{
                    DrawTexture(textures->asphaltTexture, posX, posY, WHITE);
                    break;
                }
                case 'l':{
                    DrawTexture(textures->lineTexture, posX, posY, WHITE);
                    break;
                }
                case 'i':{
                    DrawTexture(textures->asphaltTexture, posX, posY, WHITE);
                    break;
                }
            }
        }
    }
}
