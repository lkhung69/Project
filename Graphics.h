#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include "Defs.h"

;

struct Player {
    int camX = MAP_WIDTH/2 - SCREEN_WIDTH/2, camY = MAP_HEIGHT/2 - SCREEN_HEIGHT/2;
    int playerMapX = MAP_WIDTH / 2 - PLAYER_WIDTH / 2;
    int playerMapY = MAP_HEIGHT / 2 - PLAYER_HEIGHT / 2;
    int speed = 4;
    int frameIndex = 0;
    int frameDelayCounter = 0;
    bool isMoving = false;
    bool facingLeft = false;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_Rect frames[FRAME_COUNT];

    Player() {
        for (int i = 0; i < FRAME_COUNT; ++i) {
            frames[i] = {i * PLAYER_WIDTH, 0, PLAYER_WIDTH, PLAYER_HEIGHT};
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

struct Enemy{
    int enemyX, enemyY;
    int enemySpeed = 2;
    int enemyHealth = 100;
    int enemyFrameIndex = 0;
    int enemyFrameDelayCounter = 0;
    bool enemyIsMoving = false;
    bool facingRight = false;
    SDL_Rect enemyRect {0,0,ENEMY_WIDTH, ENEMY_HEIGHT};
    SDL_Rect enemyFrames[FRAME_COUNT];

    Enemy(){
        for (int i = 0; i < FRAME_COUNT; i++){
            enemyFrames[i] = {i*ENEMY_WIDTH, 0, ENEMY_WIDTH, ENEMY_HEIGHT};
        }
    }

    SDL_Rect getEnemyCurrentFrame() const{
        return enemyFrames[enemyFrameIndex];
    }

    void updateEnemyAnimation(bool isMoving) {
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
        enemyRect.x = enemyX;
        enemyRect.y = enemyY;
    }
};

struct Graphics {
    SDL_Renderer *renderer;
	SDL_Window *window;
	std::vector<SDL_Texture*> tileTextures;
    SDL_Texture* playerTexture;
    SDL_Texture* enemyTexture;
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

    void loadPlayerTexture(){
        playerTexture = loadTexture("Graphics/player.png");
    }

    void loadEnemyTexture (){
        enemyTexture = loadTexture("Graphics/enemy.png");
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
            SDL_RenderCopy(renderer, enemyTexture, &srcRect, &destRect);
        }
    }

    void quit()
    {
        IMG_Quit();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

#endif // GRAPHICS_H_INCLUDED
