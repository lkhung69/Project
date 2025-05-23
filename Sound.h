#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include <bits/stdc++.h>
#include <SDL_mixer.h>
#include <SDL.h>

Mix_Chunk* playerHurtSound = nullptr;
Mix_Chunk* playButtonEffect = nullptr;
Mix_Chunk* howToButtonEffect = nullptr;
Mix_Chunk* quitButtonEffect = nullptr;

Mix_Chunk* shootEffect = nullptr;
Mix_Chunk* hitEffect = nullptr;

Mix_Chunk* gameOverEffect = nullptr;
Mix_Chunk* playAgainButtonEffect = nullptr;

Mix_Chunk* loadEffect(const char* path) {
    Mix_Chunk* effect = Mix_LoadWAV(path);
    if (effect == nullptr) {
        SDL_Log("Failed to load sound effect: %s\n", Mix_GetError());
    }
    return effect;
}

void playEffect(Mix_Chunk* effect) {
    if (effect) {
        Mix_PlayChannel(-1, effect, 0);
    }
}

void loadGameOverEffect(){
    gameOverEffect = loadEffect("Sound Effects/game_over.mp3");
}

void playGameOver(){
    playEffect(gameOverEffect);
}

void loadBulletEffects(){
    shootEffect = loadEffect("Sound Effects/shoot.mp3");
    hitEffect = loadEffect("Sound Effects/bullet_hit.mp3");
}

void playShootEffect(){
    playEffect(shootEffect);
}

void playHitEffect(){
    playEffect(hitEffect);
}

void loadPlayerHurtEffect() {
    playerHurtSound = loadEffect("Sound Effects/player_hurt.mp3");
}

void playPlayerHurtEffect() {
    playEffect(playerHurtSound);
}

void loadButtonEffect(){
    playButtonEffect = loadEffect("Sound Effects/play_button_effect.oga");
    howToButtonEffect = loadEffect("Sound Effects/howTo_button_effect.oga");
    quitButtonEffect = loadEffect("Sound Effects/quit_button_effect.oga");
    playAgainButtonEffect = loadEffect("Sound Effects/play_again.oga");
}

void playPlayButtonEffect(){
    playEffect(playButtonEffect);
}

void playHowToButtonEffect(){
    playEffect(howToButtonEffect);
}

void playQuitButtonEffect(){
    playEffect(quitButtonEffect);
}

void playPlayAgainButtonEffect(){
    playEffect(playAgainButtonEffect);
}

void playGameOverQuitButtonEffect(){
    playEffect(quitButtonEffect);
}

#endif
