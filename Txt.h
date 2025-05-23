#ifndef TXT_H_INCLUDED
#define TXT_H_INCLUDED

#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_ttf.h>

void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        SDL_Log("TTF_RenderText_Blended error: %s", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("SDL_CreateTextureFromSurface error: %s", SDL_GetError());
        return;
    }

    SDL_Rect destRect;
    SDL_QueryTexture(texture, NULL, NULL, &destRect.w, &destRect.h);
    destRect.x = x;
    destRect.y = y;

    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
}

void renderScore(SDL_Renderer* renderer, TTF_Font* font, int score, int x, int y, SDL_Color color) {
    std::string scoreText = "Score: " + std::to_string(score);
    renderText(renderer, font, scoreText, x, y, color);
}

#endif
