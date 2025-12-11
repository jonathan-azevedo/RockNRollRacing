#include "audio.h"
#include <stdio.h>

// Enumeração local de estados para controle 
typedef enum {MENU, GAME_SETUP, PLAYING, VICTORY, PAUSE, COUNTDOWN} State;


GameAudio LoadGameAudio() {
    //carrega os arquivos de audio
    GameAudio audio = {0};
    audio.menuMusic = LoadMusicStream("Assets/Music/Skillet-Monster.mp3");
    SetMusicVolume(audio.menuMusic, 0.5f);
    audio.menuMusic.looping = true; 
    audio.raceMusic = LoadMusicStream("Assets/Music/heavy_battle.mp3");
    SetMusicVolume(audio.raceMusic, 0.4f);
    audio.raceMusic.looping = true;

    // carrega efeitos sonoros
    audio.countdown = LoadSound("Assets/Sounds/countdown.ogg");
    audio.damage = LoadSound("Assets/Sounds/damage.mp3");
    audio.shoot = LoadSound("Assets/Sounds/shoot.mp3");

    return audio;
}

void UnloadGameAudio(GameAudio *audio) {
    //funcao de limpeza de memoria
    UnloadMusicStream(audio->menuMusic);
    UnloadMusicStream(audio->raceMusic);
    
    UnloadSound(audio->countdown);
    UnloadSound(audio->shoot);
    UnloadSound(audio->damage);
}

void UpdateGameMusic(GameAudio *audio, int gameState) {
    static State lastState = -1; 
    //mudancas na musica de acordo com o estado do jogo
    if (gameState != lastState) {
        if (gameState == PAUSE) { 
            PauseMusicStream(audio->raceMusic);
        }
        else if (gameState == PLAYING || gameState == COUNTDOWN) {
            StopMusicStream(audio->menuMusic); 
            
            if (lastState == PAUSE) { 
                ResumeMusicStream(audio->raceMusic);
            } else { 
                
                StopMusicStream(audio->raceMusic);
                PlayMusicStream(audio->raceMusic);
            }
        }
        else if(gameState == MENU || gameState == GAME_SETUP || gameState == VICTORY){
            StopMusicStream(audio->raceMusic);
            
            if(!IsMusicStreamPlaying(audio->menuMusic)){
                PlayMusicStream(audio->menuMusic);
            }
        }
        lastState = gameState;
    }
    // Mantém a música rodando
    if (gameState == MENU || gameState == GAME_SETUP || gameState == VICTORY) {
        UpdateMusicStream(audio->menuMusic);
    } 
    else if (gameState == PLAYING || gameState == COUNTDOWN) {
        UpdateMusicStream(audio->raceMusic);
    }
}