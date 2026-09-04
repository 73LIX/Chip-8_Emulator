#pragma once

#include <cstdint>
#include <SDL2/SDL.h>

class Platform
{
public:
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();
    void Update(void const* buffer, int pitch);
    bool ProcessInput(uint8_t* keys);

private:
    //pointers to SDL structures
    //(Graphics pipeline)
    SDL_Window* window{}; //window
    SDL_Renderer* renderer{}; //rendering engine
    SDL_Texture* texture{}; //digital canvas(texture)
};