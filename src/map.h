#pragma once
#include <raylib.h>
#include <stdio.h>

#define MAP_WIDTH 80
#define MAP_HEIGHT 20
#define TILE_WIDTH   44 
#define TILE_HEIGHT   84

typedef struct{

    char tiles[MAP_HEIGHT][MAP_WIDTH+2];

}MAP;

MAP loadMap(const char *fileName);
void drawMap(MAP *map);