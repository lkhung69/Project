#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Graphics.h"
#include "Defs.h"
#include "Logic.h"
#include "Sound.h"
#include "Player.h"
#include "Txt.h"

using namespace std;

enum GameState {
    MAIN_MENU,
    PLAYING,
    PAUSED,
    HOW_TO_PLAY,
    GAME_OVER
};

int main(int argc, char *argv[]) {
    Graphics graphics;
    graphics.init();
    graphics.loadTileTexture(graphics.renderer);

    graphics.loadBulletTexture();

    loadPlayerHurtEffect();
    loadButtonEffect();
    loadBulletEffects();
    loadGameOverEffect();

    SDL_Surface* icon = IMG_Load("Graphics/avatar.png");
    SDL_SetWindowIcon(graphics.window, icon);
    SDL_FreeSurface(icon);

    SDL_Texture* menuTexture = graphics.loadTexture("Graphics/menu.png");
    SDL_Texture* Play = graphics.loadTexture ("Graphics/play.png");
    SDL_Texture* Play2 = graphics.loadTexture ("Graphics/play2.png");
    SDL_Texture* howTo = graphics.loadTexture ("Graphics/how.png");
    SDL_Texture* howTo2 = graphics.loadTexture ("Graphics/how2.png");
    SDL_Texture* Quit = graphics. loadTexture ("Graphics/quit.png");
    SDL_Texture* Quit2 = graphics. loadTexture ("Graphics/quit2.png");
    SDL_Texture* howToPlayTexture = graphics.loadTexture("Graphics/howToPlay.png");
    SDL_Texture* inGameMenuTexture = graphics.loadTexture("Graphics/inGameMenu.png");
    SDL_Texture* gameOverTexture = graphics.loadTexture("Graphics/gameOver.png");
    SDL_Texture* playAgain = graphics.loadTexture("Graphics/play_again.png");
    SDL_Texture* playAgain2 = graphics.loadTexture("Graphics/play_again2.png");
    SDL_Texture* gameOverQuit = graphics.loadTexture("Graphics/game_over_quit.png");
    SDL_Texture* gameOverQuit2 = graphics.loadTexture("Graphics/game_over_quit2.png");

    TTF_Font* font = TTF_OpenFont("Text/arial.ttf", 24);
    SDL_Color white = {255, 255, 255, 255};

    bool quit = false;
    bool moveLeft = false, moveRight = false, moveUp = false, moveDown = false;
    SDL_Event event;

    GameState gameState = MAIN_MENU;
    Player player;
    Uint32 lastSpawnTime = 0;
    const Uint32 SPAWN_INTERVAL = 3000;

    bool wasHovering = false;

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
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);

                        if (mouseX >= SCREEN_WIDTH/2 - 100 && mouseX <= SCREEN_WIDTH/2 + 100) {
                            if (mouseY >= SCREEN_HEIGHT/2 && mouseY <= SCREEN_HEIGHT/2 + 50) {
                                gameState = PLAYING;
                                player = Player();
                                graphics.enemies.clear();
                                graphics.loadPlayerTexture();
                                graphics.loadEnemyTexture();
                                srand(time(NULL));
                            }
                            else if (mouseY >= SCREEN_HEIGHT/2 + 75 && mouseY <= SCREEN_HEIGHT/2 + 125) {
                                gameState = HOW_TO_PLAY;
                            }
                            else if (mouseY >= SCREEN_HEIGHT/2 + 150 && mouseY <= SCREEN_HEIGHT/2 + 200) {
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
                            case SDLK_SPACE: {
                                Uint32 currentTime = SDL_GetTicks();
                                if (currentTime - graphics.lastFireTime > FIRE_DELAY) {
                                    graphics.fireBullet(player);
                                    playShootEffect();
                                    graphics.lastFireTime = currentTime;
                                }
                                break;
                            }
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

                    updateBullets(graphics.bullets);
                    handleEnemyBulletCollision(player, graphics.enemies, graphics.bullets);
                    player.updateInvincibility();
                    updateEnemies(graphics.enemies, player);
                    handlePlayerEnemyCollision(player, graphics.enemies);

                    if (player.health <= 0){
                        gameState = GAME_OVER;
                        playGameOver();
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

               case GAME_OVER:
                   if (event.type == SDL_KEYUP) {
                        switch (event.key.keysym.sym) {
                            case SDLK_w: moveUp = false; break;
                            case SDLK_s: moveDown = false; break;
                            case SDLK_a: moveLeft = false; break;
                            case SDLK_d: moveRight = false; break;
                        }
                    }
                    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
                        if(mouseX > 200 && mouseX < 600 && mouseY > 300 && mouseY < 350){
                            player = Player();
                            graphics.enemies.clear();
                            graphics.bullets.clear();
                            graphics.lastFireTime = 0;
                            gameState = PLAYING;
                        }
                        else if (mouseX > 200 && mouseX < 600 && mouseY > 400 && mouseY < 450){
                            quit = true;
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
                updateBullets(graphics.bullets);
                handleEnemyBulletCollision(player, graphics.enemies, graphics.bullets);
                updateEnemies(graphics.enemies, player);
                break;
            }
            case PAUSED:
                break;
            case GAME_OVER:
                break;
            default:
                break;
        }

        switch (gameState) {
            case MAIN_MENU: {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                int hoverItem = -1;
                bool isHoveringPlay = (mouseX >= SCREEN_WIDTH/2 - 100 && mouseX <= SCREEN_WIDTH/2 + 100 && mouseY >= SCREEN_HEIGHT/2 && mouseY <= SCREEN_HEIGHT/2 + 50);
                bool isHoveringHowTo = (mouseX >= SCREEN_WIDTH/2 - 100 && mouseX <= SCREEN_WIDTH/2 + 100 && mouseY >= SCREEN_HEIGHT/2 + 75 && mouseY <= SCREEN_HEIGHT/2 + 125);
                bool isHoveringQuit = (mouseX >= SCREEN_WIDTH/2 - 100 && mouseX <= SCREEN_WIDTH/2 + 100 && mouseY >= SCREEN_HEIGHT/2 + 150 && mouseY <= SCREEN_HEIGHT/2 + 200);

                if (isHoveringPlay) {
                    if (!wasHovering){
                        playPlayButtonEffect();
                    }

                    wasHovering = isHoveringPlay;
                    hoverItem = 0;
                }
                else if (isHoveringHowTo) {
                    if (!wasHovering) {
                        playHowToButtonEffect();
                    }

                    wasHovering = isHoveringHowTo;
                    hoverItem = 1;
                }
                else if (isHoveringQuit) {
                    if (!wasHovering){
                        playQuitButtonEffect();
                    }

                    wasHovering = isHoveringQuit;
                    hoverItem = 2;
                }
                else {
                    wasHovering = false;
                }

                graphics.renderMenu(graphics.renderer, menuTexture, Play, Play2, howTo, howTo2, Quit, Quit2, hoverItem);
                break;
            }

            case PLAYING:

                graphics.renderMap(graphics.renderer, player.camX, player.camY);
                updateBullets(graphics.bullets);
                handleEnemyBulletCollision(player, graphics.enemies, graphics.bullets);
                graphics.renderEnemies(player.camX, player.camY);
                graphics.renderPlayer(player);
                graphics.renderBullets(player.camX, player.camY);
                graphics.renderHealthBar(graphics.renderer, player.health, PLAYER_MAX_HEALTH);
                renderScore(graphics.renderer, font, player.score, 10, 10, white);

                if (player.health <= 0) {
                    gameState = GAME_OVER;
                }
                break;
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
            case GAME_OVER:

                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                int hoverItem = -1;
                bool isHoveringPlayAgain = (mouseX > SCREEN_WIDTH/2 - 200 && mouseX < SCREEN_WIDTH/2 + 200 && mouseY > SCREEN_HEIGHT/2 && mouseY < SCREEN_HEIGHT/2 + 50);
                bool isHoveringQuit = (mouseX > SCREEN_WIDTH/2 - 200 && mouseX < SCREEN_WIDTH/2 + 200 && mouseY > SCREEN_HEIGHT/2 + 100 && mouseY < SCREEN_HEIGHT/2 + 150);
                if (isHoveringPlayAgain) {
                    if (!wasHovering){
                        playPlayAgainButtonEffect();
                    }

                    wasHovering = isHoveringPlayAgain;
                    hoverItem = 0;
                }
                else if (isHoveringQuit) {
                    if (!wasHovering) {
                        playQuitButtonEffect();
                    }

                    wasHovering = isHoveringQuit;
                    hoverItem = 1;
                }
                else {
                    wasHovering = false;
                }
                graphics.renderGameOver(graphics.renderer, gameOverTexture, playAgain, gameOverQuit, playAgain2, gameOverQuit2, hoverItem);
                renderScore(graphics.renderer, font, player.score, SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 - 100, white);
                break;
        }

        graphics.presentScene();
        SDL_Delay(16);
    }

    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(howToPlayTexture);
    SDL_DestroyTexture(inGameMenuTexture);
    SDL_DestroyTexture(Play);
    SDL_DestroyTexture(Play2);
    SDL_DestroyTexture(howTo);
    SDL_DestroyTexture(howTo2);
    SDL_DestroyTexture(Quit);
    SDL_DestroyTexture(Quit2);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(playAgain);
    SDL_DestroyTexture(playAgain2);
    SDL_DestroyTexture(gameOverQuit);
    SDL_DestroyTexture(gameOverQuit2);
    graphics.quit();

    return 0;
}
