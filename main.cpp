#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include "Graphics.h"
#include "Defs.h"
#include "Logic.h"


using namespace std;

enum GameState {
    MAIN_MENU,
    PLAYING,
    PAUSED,
    HOW_TO_PLAY
};

int main(int argc, char *argv[]) {
    Graphics graphics;
    graphics.init();
    graphics.loadTileTexture(graphics.renderer);

    SDL_Surface* icon = IMG_Load("Graphics/swastika.png");
    SDL_SetWindowIcon(graphics.window, icon);
    SDL_FreeSurface(icon);

    SDL_Texture* menuTexture = graphics.loadTexture("Graphics/menu.png");
    SDL_Texture* howToPlayTexture = graphics.loadTexture("Graphics/howToPlay.png");
    SDL_Texture* inGameMenuTexture = graphics.loadTexture("Graphics/inGameMenu.png");

    bool quit = false;
    bool moveLeft = false, moveRight = false, moveUp = false, moveDown = false;
    SDL_Event event;

    GameState gameState = MAIN_MENU;
    Player player;
    Uint32 lastSpawnTime = 0;
    const Uint32 SPAWN_INTERVAL = 3000;

    while (!quit) {
        graphics.prepareScene();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }

            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            switch (gameState) {
                case MAIN_MENU:
                    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                        if (mouseX > (SCREEN_WIDTH/2 - 100) && mouseX < (SCREEN_WIDTH/2 + 100)) {
                            if (mouseY > (SCREEN_HEIGHT/2) && mouseY < (SCREEN_HEIGHT/2 + 50)) {
                                gameState = PLAYING;
                                player = Player();
                                graphics.enemies.clear();
                                graphics.loadPlayerTexture();
                                graphics.loadEnemyTexture();
                                srand(time(NULL));
                            }
                            else if (mouseY > (SCREEN_HEIGHT/2 + 75) && mouseY < (SCREEN_HEIGHT/2 + 125)) {
                                gameState = HOW_TO_PLAY;
                            }
                            else if (mouseY > (SCREEN_HEIGHT/2 + 150) && mouseY < (SCREEN_HEIGHT/2 + 200)) {
                                quit = true;
                            }
                        }
                    }
                    break;

                case PLAYING:
                    if (event.type == SDL_KEYDOWN) {
                        switch (event.key.keysym.sym) {
                            case SDLK_ESCAPE:
                                gameState = PAUSED;
                                break;
                            case SDLK_w: moveUp = true; break;
                            case SDLK_s: moveDown = true; break;
                            case SDLK_a: moveLeft = true; break;
                            case SDLK_d: moveRight = true; break;
                        }
                    }
                    else if (event.type == SDL_KEYUP) {
                        switch (event.key.keysym.sym) {
                            case SDLK_w: moveUp = false; break;
                            case SDLK_s: moveDown = false; break;
                            case SDLK_a: moveLeft = false; break;
                            case SDLK_d: moveRight = false; break;
                        }
                    }
                    break;

                case PAUSED:
                    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                        gameState = PLAYING;
                    }
                    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                        if (mouseX > 197 && mouseX < 603 && mouseY > 227 && mouseY < 273) {
                            gameState = PLAYING;
                        }
                        if (mouseX > 197 && mouseX < 603 && mouseY > 297 && mouseY < 343) {
                            gameState = HOW_TO_PLAY;
                        }
                        if (mouseX > 197 && mouseX < 603 && mouseY > 367 && mouseY < 413) {
                            quit = true;
                        }
                    }
                    break;

                case HOW_TO_PLAY:
                    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                        if (mouseX > 304 && mouseX < 495 && mouseY > 513 && mouseY < 576) {
                            gameState = MAIN_MENU;
                        }
                    }
                    break;
            }
        }

        switch (gameState) {
            case PLAYING: {
                Uint32 currentTime = SDL_GetTicks();
                if (currentTime - lastSpawnTime > SPAWN_INTERVAL) {
                    graphics.spawnEnemy(player.playerMapX, player.playerMapY);
                    lastSpawnTime = currentTime;
                }

                bool isMoving = false;
                if (moveLeft) { turnWest(player, MAP_WIDTH); isMoving = true; player.updateAnimation(isMoving);}
                if (moveRight) { turnEast(player, MAP_WIDTH); isMoving = true; player.updateAnimation(isMoving);}
                if (moveUp) { turnNorth(player, MAP_HEIGHT); isMoving = true; player.updateAnimation(isMoving);}
                if (moveDown) { turnSouth(player, MAP_HEIGHT); isMoving = true; player.updateAnimation(isMoving);}

                if(!isMoving){
                    player.isMoving = false;
                    player.frameIndex = 0;
                }
                updateEnemies(graphics.enemies, player);
                break;
            }
            case PAUSED:
                break;
            default:
                break;
        }

        switch (gameState) {
            case MAIN_MENU:
                graphics.renderMenu(graphics.renderer, menuTexture);
                break;

            case PLAYING:
            case PAUSED:
                graphics.renderMap(graphics.renderer, player.camX, player.camY);
                graphics.renderPlayer(player);
                graphics.renderEnemies(player.camX, player.camY);

                if (gameState == PAUSED) {
                    SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 180);
                    SDL_RenderFillRect(graphics.renderer, NULL);

                    graphics.renderInGameMenu(graphics.renderer, inGameMenuTexture);
                }
                break;

            case HOW_TO_PLAY:
                graphics.renderHowTo(graphics.renderer, howToPlayTexture);
                break;
        }

        graphics.presentScene();
        SDL_Delay(16);
    }

    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(howToPlayTexture);
    SDL_DestroyTexture(inGameMenuTexture);
    graphics.quit();

    return 0;
}
