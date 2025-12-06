#pragma once
#include <raylib.h>



typedef struct {
    Music menuMusic;      // Skillet - Monster
    Music raceMusic;      // Heavy Battle

    Sound countdown;
    Sound shoot;
    Sound damage;
} GameAudio;


GameAudio LoadGameAudio();              // Carrega todos os assets
void UnloadGameAudio(GameAudio *audio); // Libera memória
void UpdateGameMusic(GameAudio *audio, int gameState); // Gerencia troca de músicas baseada no estado
