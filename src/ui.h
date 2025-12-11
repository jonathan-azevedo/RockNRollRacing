#pragma once
#include <raylib.h>
#include <math.h>
#include "car.h"
#include "textures.h"
#include "enemy.h"

//agrupa a textura virtual e as coordenadas de escala calculadas para adaptar a resolução do jogo a qualquer tamanho de monitor
typedef struct{
    RenderTexture2D screen;
    int virtualScreenWidth;
    int virtualScreenHeight;
    Rectangle sourceRec;
    Rectangle destRec;
} RENDER_SETUP;

RENDER_SETUP calculateScreenSetup(RENDER_SETUP renderSetup); //analisa a resolução real do monitor e calcula a escala e o posicionamento necessários para renderizar a tela do jogo 
void drawHUD(CAR *player, ENEMY enemies[], int enemyCount, GAME_TEXTURES *textures, int screenW, int screenH);//renderiza a interface, itens equipados, contagem de voltas,status de vida e progresso de todos os inimigos
