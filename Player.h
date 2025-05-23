#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "Defs.h"
#include "Sound.h"

struct Player{
    int camX = MAP_WIDTH/2 - SCREEN_WIDTH/2;
    int camY = MAP_HEIGHT/2 - SCREEN_HEIGHT/2;
    int playerMapX = MAP_WIDTH / 2 - PLAYER_WIDTH / 2;
    int playerMapY = MAP_HEIGHT / 2 - PLAYER_HEIGHT / 2;
    int health = PLAYER_MAX_HEALTH;
    int score = 0;

    int frameIndex = 0;
    int frameDelayCounter = 0;
    bool isMoving = false;
    bool facingLeft = false;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_Rect frames[FRAME_COUNT];

    int directionX = 1;
    int directionY = 0;

    bool isInvincible = false;
    Uint32 invincibleTimer = 0;
    static constexpr Uint32 INVINCIBLE_DURATION = 1000;

    Player() {
        for (int i = 0; i < FRAME_COUNT; ++i) {
            frames[i] = {i * PLAYER_WIDTH, 0, PLAYER_WIDTH, PLAYER_HEIGHT};
        }
    }

    void startInvincibility() {
        isInvincible = true;
        invincibleTimer = SDL_GetTicks();
    }

    void updateInvincibility() {
        if (isInvincible && SDL_GetTicks() - invincibleTimer > INVINCIBLE_DURATION) {
            isInvincible = false;
        }
    }

    SDL_Rect getCurrentFrame() const {
        return frames[frameIndex];
    }

    void updateDirection(bool movingLeft){
        if (movingLeft) {
            flip = SDL_FLIP_HORIZONTAL;
            facingLeft = true;
        }
        else if (!movingLeft && facingLeft) {
            flip = SDL_FLIP_NONE;
            facingLeft = false;
        }
    }

    void updateAnimation(bool isMoving) {
        if (isMoving) {
            frameDelayCounter++;
            if (frameDelayCounter >= FRAME_DELAY) {
                frameIndex = (frameIndex + 1) % FRAME_COUNT;
                frameDelayCounter = 0;
            }
        }
        else {
            frameIndex = 0;
        }
    }
};

#endif
