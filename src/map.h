#pragma once
#include <raylib.h>
#include <stdio.h>
#include "textures.h"

#define MAP_WIDTH 80
#define MAP_HEIGHT 20
#define TILE_SIZE 54


typedef struct{

    char tiles[MAP_HEIGHT][MAP_WIDTH+5];

}MAP;

MAP loadMap(const char *fileName); //le um arquivo track.txt e preenche a matriz do mapa
void drawMap(MAP *map, GAME_TEXTURES *textures); // percorre a matriz e desenha a textura correspondente para cada letra
void getInitialPosition(MAP *map, int *initialPosx, int *initialPosy); //acha o 'j' e calcula as coordenadas em pixels do jogador