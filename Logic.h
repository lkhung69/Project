#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED

#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include "Defs.h"
#include "Graphics.h"

void renderMenu(SDL_Renderer* renderer, SDL_Texture* menuTexture) {
    SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
}

void renderHowTo(SDL_Renderer* renderer, SDL_Texture* howToPlayTexture) {
    SDL_RenderCopy(renderer, howToPlayTexture, NULL, NULL);
}

void updateCamera(Player& player, int mapWidth, int mapHeight) {
    player.camX = player.playerMapX - SCREEN_WIDTH / 2 + PLAYER_WIDTH / 2;
    player.camY = player.playerMapY - SCREEN_HEIGHT / 2 + PLAYER_HEIGHT / 2;

    if (player.camX < 0) player.camX = 0;
    if (player.camY < 0) player.camY = 0;
    if (player.camX > mapWidth - SCREEN_WIDTH) player.camX = mapWidth - SCREEN_WIDTH;
    if (player.camY > mapHeight - SCREEN_HEIGHT) player.camY = mapHeight - SCREEN_HEIGHT;
}

void turnWest(Player& player, int mapWidth) {
    player.isMoving = true;
    player.updateDirection(true);
    player.updateAnimation(true);
    player.playerMapX -= player.speed;
    if (player.playerMapX < 0) player.playerMapX = 0;
    updateCamera(player, mapWidth, MAP_HEIGHT);
}

void turnEast(Player& player, int mapWidth) {
    player.isMoving = true;
    player.updateDirection(false);
    player.updateAnimation(true);
    player.playerMapX += player.speed;
    if (player.playerMapX > mapWidth - PLAYER_WIDTH) player.playerMapX = mapWidth - PLAYER_WIDTH;
    updateCamera(player, mapWidth, MAP_HEIGHT);
}

void turnNorth(Player& player, int mapHeight) {
    player.playerMapY -= player.speed;
    player.updateAnimation(true);
    if (player.playerMapY < 0) player.playerMapY = 0;
    updateCamera(player, MAP_WIDTH, mapHeight);
    player.isMoving = true;
}

void turnSouth(Player& player, int mapHeight) {
    player.playerMapY += player.speed;
    player.updateAnimation(true);
    if (player.playerMapY > mapHeight - PLAYER_HEIGHT) player.playerMapY = mapHeight - PLAYER_HEIGHT;
    updateCamera(player, MAP_WIDTH, mapHeight);
    player.isMoving = true;
}

void updateEnemies(std::vector<Enemy>& enemies, Player& player) {
    for (auto& enemy : enemies) {
        enemy.moveTowards(player.playerMapX, player.playerMapY);
        enemy.updateEnemyAnimation(true);
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) {
            return e.enemyX < -100 || e.enemyX > MAP_WIDTH + 100 || e.enemyY < -100 || e.enemyY > MAP_HEIGHT + 100;
    }),
    enemies.end());
}

#endif
