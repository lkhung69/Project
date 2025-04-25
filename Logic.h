#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED
#define INITIAL_SPEED 5

#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include "Defs.h"
#include "Graphics.h"

struct PlayerMoves {
    int camX = 0, camY = 0;
    int playerMapX = SCREEN_WIDTH / 2 - PLAYER_SIZE/2;
    int playerMapY = SCREEN_HEIGHT / 2 - PLAYER_SIZE/2;
    int speed = INITIAL_SPEED;
};

void updateCamera(PlayerMoves& player, int mapWidth, int mapHeight) {
    const int halfScreenW = SCREEN_WIDTH / 2;
    const int halfScreenH = SCREEN_HEIGHT / 2;

    player.camX = player.playerMapX - halfScreenW + PLAYER_SIZE/2;
    player.camY = player.playerMapY - halfScreenH + PLAYER_SIZE/2;

    if (player.camX < 0) player.camX = 0;
    if (player.camY < 0) player.camY = 0;
    if (player.camX > mapWidth - SCREEN_WIDTH) player.camX = mapWidth - SCREEN_WIDTH;
    if (player.camY > mapHeight - SCREEN_HEIGHT) player.camY = mapHeight - SCREEN_HEIGHT;
}

void turnNorth(PlayerMoves& player, int mapHeight) {
    if (player.playerMapY > 0)
        player.playerMapY -= player.speed;

    updateCamera(player, MAP_WIDTH, mapHeight);
}

void turnSouth(PlayerMoves& player, int mapHeight) {
    if (player.playerMapY < mapHeight - PLAYER_SIZE)
        player.playerMapY += player.speed;

    updateCamera(player, MAP_WIDTH, mapHeight);
}

void turnWest(PlayerMoves& player, int mapWidth) {
    if (player.playerMapX > 0)
        player.playerMapX -= player.speed;

    updateCamera(player, mapWidth, MAP_HEIGHT);
}

void turnEast(PlayerMoves& player, int mapWidth) {
    if (player.playerMapX < mapWidth - PLAYER_SIZE)
        player.playerMapX += player.speed;

    updateCamera(player, mapWidth, MAP_HEIGHT);
}

bool gameOver(const PlayerMoves& player) {
    //return player.x < 0 || player.x >= SCREEN_WIDTH || player.y < 0 || player.y >= SCREEN_HEIGHT;
    return false;
}

void renderPlayer(const PlayerMoves& player, const Graphics& graphics) {
    SDL_Rect filled_rect;
    filled_rect.x = player.playerMapX - player.camX;
    filled_rect.y = player.playerMapY - player.camY;
    filled_rect.w = PLAYER_SIZE;
    filled_rect.h = PLAYER_SIZE;

    SDL_SetRenderDrawColor(graphics.renderer, 0, 255, 0, 255); // Green
    SDL_RenderFillRect(graphics.renderer, &filled_rect);
}

void renderMenu(SDL_Renderer* renderer, SDL_Texture* menuTexture){
    SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
}

void renderHowTo(SDL_Renderer* renderer, SDL_Texture* howToPlayTexture){
    SDL_RenderCopy(renderer, howToPlayTexture, NULL, NULL);
}

#endif // LOGIC_H_INCLUDED
