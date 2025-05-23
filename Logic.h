#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED

#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Defs.h"
#include "Graphics.h"
#include "Player.h"
#include "Sound.h"

void updateCamera(Player& player, int mapWidth, int mapHeight) {
    player.camX = player.playerMapX - SCREEN_WIDTH / 2 + PLAYER_WIDTH / 2;
    player.camY = player.playerMapY - SCREEN_HEIGHT / 2 + PLAYER_HEIGHT / 2;

    if (player.camX < 0) player.camX = 0;
    if (player.camY < 0) player.camY = 0;
    if (player.camX > mapWidth - SCREEN_WIDTH) player.camX = mapWidth - SCREEN_WIDTH;
    if (player.camY > mapHeight - SCREEN_HEIGHT) player.camY = mapHeight - SCREEN_HEIGHT;
}

void turnWest(Player& player, int mapWidth) {
    player.directionX = -1;
    player.directionY = 0;
    player.isMoving = true;
    player.updateDirection(true);
    player.updateAnimation(true);
    player.playerMapX -= PLAYER_SPEED;
    if (player.playerMapX < 0) player.playerMapX = 0;
    updateCamera(player, mapWidth, MAP_HEIGHT);
}

void turnEast(Player& player, int mapWidth) {
    player.directionX = 1;
    player.directionY = 0;
    player.isMoving = true;
    player.updateDirection(false);
    player.updateAnimation(true);
    player.playerMapX += PLAYER_SPEED;
    if (player.playerMapX > mapWidth - PLAYER_WIDTH) player.playerMapX = mapWidth - PLAYER_WIDTH;
    updateCamera(player, mapWidth, MAP_HEIGHT);
}

void turnNorth(Player& player, int mapHeight) {
    player.directionX = 0;
    player.directionY = -1;
    player.playerMapY -= PLAYER_SPEED;
    player.updateAnimation(true);
    if (player.playerMapY < 0) player.playerMapY = 0;
    updateCamera(player, MAP_WIDTH, mapHeight);
    player.isMoving = true;
}

void turnSouth(Player& player, int mapHeight) {
    player.directionX = 0;
    player.directionY = 1;
    player.playerMapY += PLAYER_SPEED;
    player.updateAnimation(true);
    if (player.playerMapY > mapHeight - PLAYER_HEIGHT) player.playerMapY = mapHeight - PLAYER_HEIGHT;
    updateCamera(player, MAP_WIDTH, mapHeight);
    player.isMoving = true;
}

void updateEnemies(std::vector<Enemy>& enemies, Player& player) {
    for (auto& enemy : enemies) {
        enemy.moveTowards(player.playerMapX, player.playerMapY);
        if (enemy.enemyX < player.playerMapX){
            enemy.flip = SDL_FLIP_HORIZONTAL;
        }
        if (enemy.enemyX > player.playerMapX){
            enemy.flip = SDL_FLIP_NONE;
        }
        enemy.updateEnemyAnimation(true);
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) {
            return e.enemyX < -100 || e.enemyX > MAP_WIDTH + 100 || e.enemyY < -100 || e.enemyY > MAP_HEIGHT + 100;
    }),
    enemies.end());
}

void updateBullets(std::vector<Bullet>& bullets) {
    for (auto& bullet : bullets) {
        if (bullet.active) {
            bullet.update();
        }
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.active; }),
        bullets.end());
}


bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y);
}

void handlePlayerEnemyCollision(Player& player, std::vector<Enemy>& enemies) {
    if (player.isInvincible) return;

    SDL_Rect playerRect = {
        player.playerMapX,
        player.playerMapY,
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    };

    for (auto& enemy : enemies) {
        if (checkCollision(playerRect, enemy.enemyRect)) {
            player.health -= 100;
            playPlayerHurtEffect();
            player.startInvincibility();

            int knockbackX = (player.playerMapX < enemy.enemyX) ? -50 : 50;
            enemy.enemyX -= knockbackX;

            break;
        }
    }
}

void handleEnemyBulletCollision(Player& player, std::vector<Enemy>& enemies, std::vector<Bullet>&bullets){
    for (auto& bullet : bullets){
        SDL_Rect bulletRect{
            bullet.x,
            bullet.y,
            BULLET_WIDTH,
            BULLET_HEIGHT
        };
        for (auto& enemy : enemies){
            SDL_Rect enemyRect{
                enemy.enemyX,
                enemy.enemyY,
                ENEMY_WIDTH,
                ENEMY_HEIGHT
            };
            if (checkCollision(enemyRect, bulletRect)){
                bullet.active = false;
                enemy.enemyHealth -= 100;
                player.score += 10;
                playHitEffect();
                if (enemy.enemyHealth <= 0) {
                    enemy.enemyHealth = 0;
                    enemy.enemyX = -9999;
                    enemy.enemyY = -9999;
                }
            }
        }
    }
}

#endif
