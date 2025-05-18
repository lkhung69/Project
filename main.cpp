#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include "Graphics.h"
#include "Defs.h"
#include "Logic.h"


using namespace std;

int main(int argc, char *argv[])
{
    Graphics graphics;
    graphics.init();
    graphics.loadTileTexture(graphics.renderer);

    SDL_Surface* icon = IMG_Load("Graphics/swastika.png");
    SDL_SetWindowIcon(graphics.window, icon);
    SDL_FreeSurface(icon);

    SDL_Texture* menuTexture = graphics.loadTexture("Graphics/menu.png");
    SDL_Texture* howToPlayTexture = graphics.loadTexture ("Graphics/howToPlay.png");

    bool quit = false;
    bool moveLeft = false, moveRight = false, moveUp = false, moveDown = false;
    SDL_Event event;

    bool inMenu = false;
    bool howToPlay = false;
    int mouseX, mouseY;
    //menu
    while (!inMenu){
        graphics.prepareScene();
        while (SDL_PollEvent(&event)){
            SDL_GetMouseState(&mouseX, &mouseY);
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (mouseX > (SCREEN_WIDTH/2 - 100) && mouseX < (SCREEN_WIDTH/2 + 100) && mouseY > (SCREEN_HEIGHT/2) && mouseY < (SCREEN_HEIGHT/2 + 50)){
                        inMenu = true;
                    }
                    if (mouseX > (SCREEN_WIDTH/2 - 100) && mouseX < (SCREEN_WIDTH/2 + 100) && mouseY > (SCREEN_HEIGHT/2 + 75) && mouseY < (SCREEN_HEIGHT/2 + 125)){
                        howToPlay = true;
                        while (howToPlay) {
                            while (SDL_PollEvent(&event)) {
                                SDL_GetMouseState(&mouseX, &mouseY);
                                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                                    if (mouseX > 304 && mouseX < 495 && mouseY > 513 && mouseY < 576){
                                        howToPlay = false;
                                    }
                                }
                            }

                            renderHowTo(graphics.renderer, howToPlayTexture);
                            graphics.presentScene();
                            SDL_Delay(100);
                        }

                    }
                    if (mouseX > (SCREEN_WIDTH/2 - 100) && mouseX < (SCREEN_WIDTH/2 + 100) && mouseY > (SCREEN_HEIGHT/2 + 150) && mouseY < (SCREEN_HEIGHT/2 + 200)){
                        quit = true;
                        inMenu = true;
                    }
                }
            }
        }
        renderMenu(graphics.renderer, menuTexture);
        graphics.presentScene();
        SDL_Delay(100);
    }

    SDL_DestroyTexture(menuTexture);

    Player player;
    graphics.loadPlayerTexture();

    //game loop
    while (!quit) {
        graphics.prepareScene();

        player.isMoving = false;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w: moveUp = true; break;
                    case SDLK_s: moveDown = true; break;
                    case SDLK_a: moveLeft = true; break;
                    case SDLK_d: moveRight = true; break;
                }
            }
            if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_w: moveUp = false; break;
                    case SDLK_s: moveDown = false; break;
                    case SDLK_a: moveLeft = false; break;
                    case SDLK_d: moveRight = false; break;
                }
            }
        }

        bool isMoving = false;

        if (moveLeft) {
            turnWest(player, MAP_WIDTH);
            isMoving = true;
        }
        if (moveRight) {
            turnEast(player, MAP_WIDTH);
            isMoving = true;
        }
        if (moveUp) {
            turnNorth(player, MAP_HEIGHT);
            isMoving = true;
        }
        if (moveDown) {
            turnSouth(player, MAP_HEIGHT);
            isMoving = true;
        }

        if (!isMoving) {
            player.isMoving = false;
            player.frameIndex = 0;
        }

        graphics.renderMap(graphics.renderer, player.camX, player.camY);
        graphics.renderPlayer(player);

        graphics.presentScene();
        SDL_Delay(16);
    }

    graphics.quit();
    return 0;
}
