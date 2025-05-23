#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "Defs.h"
#include "Sound.h"
#include "Player.h"

;

struct Enemy{
    int enemyX, enemyY;
    int enemySpeed = 2;
    int enemyHealth = 100;
    int enemyFrameIndex = 0;
    int enemyFrameDelayCounter = 0;
    bool enemyIsMoving = false;
    bool facingRight = false;
    SDL_Rect enemyRect {0,0,ENEMY_WIDTH, ENEMY_HEIGHT};
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_Rect enemyFrames[FRAME_COUNT];

    Enemy(){
        for (int i = 0; i < FRAME_COUNT; i++){
            enemyFrames[i] = {i*ENEMY_WIDTH, 0, ENEMY_WIDTH, ENEMY_HEIGHT};
        }
    }

    SDL_Rect getEnemyCurrentFrame() const{
        return enemyFrames[enemyFrameIndex];
    }

    void updateEnemyDirection(bool movingRight) {
        if (movingRight) {
            flip = SDL_FLIP_NONE;
        } else {
            flip = SDL_FLIP_HORIZONTAL;
        }
    }

    void updateEnemyAnimation(bool enemyIsMoving) {
        if (enemyIsMoving) {
            enemyFrameDelayCounter++;
            if (enemyFrameDelayCounter >= ENEMY_FRAME_DELAY) {
                enemyFrameIndex = (enemyFrameIndex + 1) % FRAME_COUNT;
                enemyFrameDelayCounter = 0;
            }
        }
        else {
            enemyFrameIndex = 0;
        }
    }

    void moveTowards(int targetX, int targetY) {
        enemyIsMoving = true;
        float dx = targetX - enemyX;
        float dy = targetY - enemyY;
        float distance = sqrt(dx*dx + dy*dy);

        if (distance > 0) {
            enemyX += static_cast<int>((dx / distance) * enemySpeed);
            enemyY += static_cast<int>((dy / distance) * enemySpeed);
        }
        else {
            enemyIsMoving = false;
        }
        enemyRect.x = enemyX;
        enemyRect.y = enemyY;
    }
};

struct Bullet {
    int x, y;
    int directionX, directionY;
    bool active;
    SDL_Rect rect;

    Bullet(int startX, int startY, int dx, int dy)
        : x(startX), y(startY), directionX(dx), directionY(dy),
          active(true), rect{x, y, BULLET_WIDTH, BULLET_HEIGHT} {}

    void update() {
        x += directionX * BULLET_SPEED;
        y += directionY * BULLET_SPEED;
        rect.x = x;
        rect.y = y;

        if (x < 0 || x > MAP_WIDTH || y < 0 || y > MAP_HEIGHT) {
            active = false;
        }
    }
};

struct Graphics {
    SDL_Renderer *renderer;
	SDL_Window *window;
	std::vector<SDL_Texture*> tileTextures;
    SDL_Texture* playerTexture;
    SDL_Texture* enemyTexture;
    SDL_Texture* bulletTexture;
    SDL_Texture* hpBarOutlineTexture;
    SDL_Texture* hpBarInsideTexture;
    std::vector<Bullet> bullets;
    Uint32 lastFireTime = 0;
    std::vector<Enemy> enemies;

	void logErrorAndExit(const char* msg, const char* error)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
        SDL_Quit();
    }

	void init() {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
            logErrorAndExit("SDL_Init", SDL_GetError());

        window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (window == nullptr) logErrorAndExit("CreateWindow", SDL_GetError());

        if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
            logErrorAndExit( "SDL_image error:", IMG_GetError());

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                              SDL_RENDERER_PRESENTVSYNC);

        if (renderer == nullptr) logErrorAndExit("CreateRenderer", SDL_GetError());

        if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
            logErrorAndExit( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        }

        if (TTF_Init() == -1) {
            logErrorAndExit("SDL_ttf could not initialize! SDL_ttf Error: ",
                             TTF_GetError());
        }


        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    void prepareScene()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

	void prepareScene(SDL_Texture * background)
    {
        SDL_RenderClear(renderer);
        SDL_RenderCopy( renderer, background, NULL, NULL);
    }

    void presentScene()
    {
        SDL_RenderPresent(renderer);
    }

    SDL_Texture *loadTexture(const char *filename)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

        SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
        if (texture == NULL)
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());

        return texture;
    }

    void renderTexture(SDL_Texture *texture, int x, int y)
    {
        SDL_Rect dest;

        dest.x = x;
        dest.y = y;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    void loadTileTexture(SDL_Renderer* renderer){
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/grass_0.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/grass_1.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/grass_2.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/stone_to_grass_top_left.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/stone_to_grass_top_right.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/stone_to_grass_bottom_left.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/stone_to_grass_bottom_right.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/stone_to_grass_up.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/stone_to_grass_left.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/stone_to_grass_right.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/stone_to_grass_down.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/stone_0.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/stone_1.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/spawn_point_up_left.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/spawn_point_up_right.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/spawn_point_down_right.png"));
        tileTextures.push_back(IMG_LoadTexture(renderer, "Graphics/Map/spawn_point_down_left.png"));
    }

    void renderMap(SDL_Renderer* renderer, int camX, int camY){
        SDL_Rect destRect;

        for (int row = 0; row < MAP_ROWS; row++) {
            for (int col = 0; col < MAP_COLS; col++) {
                int tileID = gameMap[row][col];

                if (tileID < 0 || tileID >= (int)tileTextures.size()) {
                    SDL_Log("Invalid tile ID %d at [%d][%d]", tileID, row, col);
                    continue;
                }

                SDL_Texture* tileTex = tileTextures[tileID];

                if (tileTex == nullptr) {
                    SDL_Log("Texture null for tile ID %d", tileID);
                    continue;
                }

                destRect.x = col * TILE_SIZE - camX;
                destRect.y = row * TILE_SIZE - camY;
                destRect.w = TILE_SIZE;
                destRect.h = TILE_SIZE;

                SDL_RenderCopy(renderer, tileTex, nullptr, &destRect);
            }
        }
    }

    void renderMenu(SDL_Renderer* renderer, SDL_Texture* menuTexture, SDL_Texture* playNormal, SDL_Texture* playHover, SDL_Texture* howNormal, SDL_Texture* howHover, SDL_Texture* quitNormal, SDL_Texture* quitHover, int hoverItem = -1) {
        SDL_RenderCopy(renderer, menuTexture, NULL, NULL);

        SDL_Rect menuRects[3] = {
            {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, 200, 50},
            {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 75, 200, 50},
            {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 150, 200, 50}
        };

        SDL_RenderCopy(renderer, (hoverItem == 0) ? playHover : playNormal, NULL, &menuRects[0]);
        SDL_RenderCopy(renderer, (hoverItem == 1) ? howHover : howNormal, NULL, &menuRects[1]);
        SDL_RenderCopy(renderer, (hoverItem == 2) ? quitHover : quitNormal, NULL, &menuRects[2]);
    }

    void renderHowTo(SDL_Renderer* renderer, SDL_Texture* howToPlayTexture) {
        SDL_RenderCopy(renderer, howToPlayTexture, NULL, NULL);
    }

    void renderInGameMenu(SDL_Renderer* renderer, SDL_Texture* inGameMenu) {
        SDL_RenderCopy(renderer, inGameMenu, NULL, NULL);
    }

    void renderGameOver(SDL_Renderer* renderer, SDL_Texture* gameOverTexture, SDL_Texture* playAgain, SDL_Texture* gameOverQuit, SDL_Texture* playAgain2, SDL_Texture* gameOverQuit2, int hoverItem = -1){
        SDL_RenderCopy(renderer, gameOverTexture, NULL, NULL);

        SDL_Rect gameOverRects[2] = {
            {SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2, 400, 50},
            {SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 100, 400, 50},
        };

        SDL_RenderCopy(renderer, (hoverItem == 0) ? playAgain2 : playAgain, NULL, &gameOverRects[0]);
        SDL_RenderCopy(renderer, (hoverItem == 1) ? gameOverQuit2 : gameOverQuit, NULL, &gameOverRects[1]);
    }

    void loadPlayerTexture(){
        playerTexture = loadTexture("Graphics/player.png");
    }

    void loadEnemyTexture (){
        enemyTexture = loadTexture("Graphics/enemy.png");
    }

    void renderHealthBar(SDL_Renderer* renderer, int currentHP, int maxHP) {
        float healthPercent = static_cast<float>(currentHP) / maxHP;
        if (healthPercent < 0) healthPercent = 0;

        int barWidth = HP_BAR_WIDTH;
        int barHeight = HP_BAR_HEIGHT;
        int barX = (SCREEN_WIDTH - barWidth) / 2;
        int barY = SCREEN_HEIGHT - barHeight - HP_BAR_MARGIN_BOTTOM;

        SDL_Rect background = { barX, barY, barWidth, barHeight };
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &background);

        SDL_Rect foreground = { barX, barY, static_cast<int>(barWidth * healthPercent), barHeight };
        SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        SDL_RenderFillRect(renderer, &foreground);
    }

    void spawnEnemy(int playerX, int playerY) {
        Enemy newEnemy;

        int side = rand() % 4;
        switch(side) {
            case 0:
                newEnemy.enemyX = rand() % MAP_WIDTH;
                newEnemy.enemyY = 0;
                break;
            case 1:
                newEnemy.enemyX = MAP_WIDTH;
                newEnemy.enemyY = rand() % MAP_HEIGHT;
                break;
            case 2:
                newEnemy.enemyX = rand() % MAP_WIDTH;
                newEnemy.enemyY = MAP_HEIGHT;
                break;
            case 3:
                newEnemy.enemyX = 0;
                newEnemy.enemyY = rand() % MAP_HEIGHT;
                break;
        }

        enemies.push_back(newEnemy);
    }


    void renderPlayer(const Player& player) {
        SDL_Rect srcRect = player.getCurrentFrame();
        SDL_Rect destRect = {
            player.playerMapX - player.camX,
            player.playerMapY - player.camY,
            PLAYER_WIDTH,
            PLAYER_HEIGHT
        };

        if (player.isInvincible) {
            if ((SDL_GetTicks() / 100) % 2 == 0) {
                SDL_SetTextureColorMod(playerTexture, 255, 100, 100);
            } else {
                SDL_SetTextureColorMod(playerTexture, 255, 255, 255);
            }
        } else {
            SDL_SetTextureColorMod(playerTexture, 255, 255, 255);
        }

        SDL_RenderCopyEx(renderer, playerTexture, &srcRect, &destRect, 0.0, NULL, player.flip);
    }

    void renderEnemies(int camX, int camY){
        for (auto& enemy : enemies) {
            SDL_Rect srcRect = enemy.getEnemyCurrentFrame();
            SDL_Rect destRect = {
                enemy.enemyX - camX,
                enemy.enemyY - camY,
                enemy.enemyRect.w,
                enemy.enemyRect.h
            };
            SDL_RenderCopyEx(renderer, enemyTexture, &srcRect, &destRect,0.0, NULL, enemy.flip);
        }
    }

    void loadBulletTexture() {
        bulletTexture = loadTexture("Graphics/bullet.png");
    }

    void renderBullets(int camX, int camY) {
        for (auto& bullet : bullets) {
            if (bullet.active) {
                SDL_Rect destRect = {
                    bullet.x - camX,
                    bullet.y - camY,
                    bullet.rect.w,
                    bullet.rect.h
                };
                SDL_RenderCopy(renderer, bulletTexture, NULL, &destRect);
            }
        }
    }

    void fireBullet(Player& player) {
        int startX = player.playerMapX + PLAYER_WIDTH/2 - BULLET_WIDTH/2;
        int startY = player.playerMapY + PLAYER_HEIGHT/2 - BULLET_HEIGHT/2;
        bullets.emplace_back(startX, startY, player.directionX, player.directionY);
    }

    void quit()
    {
        IMG_Quit();
        Mix_Quit();
        TTF_Quit();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

#endif // GRAPHICS_H_INCLUDED
