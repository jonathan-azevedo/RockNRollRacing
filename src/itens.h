#pragma once
#include <raylib.h>
#include "car.h"
#include "map.h"
#include "audio.h"
#include "textures.h"
#include "enemy.h" // <--- Importante incluir isso agora

#define MAX_ITEMS 5
#define MAX_BOMBS 10
#define MAX_PROJECTILES 20
#define MAX_EFFECTS 20

typedef struct {
    Rectangle rect;
    bool active;
    float respawnTimer;
} LOOT_BOX;

typedef struct {
    float x, y;
    bool active;
    float timer;
    bool droppedByPlayer;
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

typedef struct {
    LOOT_BOX items[MAX_ITEMS];
    BOMB bombs[MAX_BOMBS];
    PROJECTILE projectiles[MAX_PROJECTILES];
    VISUAL_EFFECT effects[MAX_EFFECTS];
} ITEM_MANAGER;

void initItems(ITEM_MANAGER *manager, MAP *map);
void spawnProjectile(ITEM_MANAGER *manager, float x, float y, float angle, bool isPlayer);
void spawnExplosion(ITEM_MANAGER *manager, float x, float y);
void spawnBomb(ITEM_MANAGER *manager, float x, float y, bool isPlayer);

// MUDANÇA: Agora recebe 'ENEMY enemies[]' e 'int enemyCount'
void updateItems(ITEM_MANAGER *manager, CAR *player, ENEMY enemies[], int enemyCount, float dt, GameAudio *audio, MAP *map);

void drawItems(ITEM_MANAGER *manager, GAME_TEXTURES *textures);
