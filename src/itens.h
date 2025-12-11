#pragma once
#include <raylib.h>
#include "car.h"
#include "map.h"
#include "audio.h"
#include "textures.h"
#include "enemy.h" 

#define MAX_ITEMS 5
#define MAX_BOMBS 10
#define MAX_PROJECTILES 4
#define MAX_EFFECTS 5


typedef struct {
    Rectangle rect;
    bool active;
    float respawnTimer;
} LOOT_BOX;


typedef struct {
    float x, y;
    bool active;
    int ID;        
    float safeTimer;    
} BOMB;


typedef struct {
    float x, y;
    float dx, dy;
    float angle;
    bool active;
    bool firedByPlayer;
} PROJECTILE;


typedef struct {
    float x, y;
    float timer;
    float maxTime;
    bool active;
} VISUAL_EFFECT;

//gerenciador central dos itens
typedef struct {
    LOOT_BOX items[MAX_ITEMS];
    BOMB bombs[MAX_BOMBS];
    PROJECTILE projectiles[MAX_PROJECTILES];
    VISUAL_EFFECT effects[MAX_EFFECTS];
} ITEM_MANAGER;

void initItems(ITEM_MANAGER *manager, MAP *map);//// Inicializa o sistema e espalha as caixas pelo mapa
void spawnProjectile(ITEM_MANAGER *manager, float x, float y, float angle, bool isPlayer); /// Cria um novo projétil no pool se houver vaga
void spawnExplosion(ITEM_MANAGER *manager, float x, float y); //cria a explosao
void spawnBomb(ITEM_MANAGER *manager, float x, float y, int ID); //planta a bomba no chao,atribuindo um ID de dono
void updateItems(ITEM_MANAGER *manager, CAR *player, ENEMY enemies[], int enemyCount, float dt, GameAudio *audio, MAP *map); //movimento e temporizadores de todos os objetos ativos e verifica colisões com os veículos para aplicar danos ou coletar itens
void drawItems(ITEM_MANAGER *manager, GAME_TEXTURES *textures); //desenha todos os objetos ativos
