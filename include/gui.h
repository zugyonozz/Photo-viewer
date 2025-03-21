// main.cpp
// date created 16-03-2025
// author: zugyonozz
#ifndef GUI_H
#define GUI_H

#include <string>
#include <iostream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"

struct BtnTexture {
    SDL_Texture* normal;
    SDL_Texture* hover;
};

struct Img{
	SDL_Texture* texture;
	int W, H;
};

extern bool Run;
extern SDL_Window* window;
extern int W;
extern int H;

void loadQueueImg(int argc, char* argv[]);
void renderImg(int Nav = 2);
void imgLayout();
void initGUI(int initialWidth = 800, int initialHeight = 600);
void destroyGUI();
void updateLayout();
void renderGUI(int mx, int my);
void handleButtonClick(int x, int y);
void showGUI();

#endif // GUI_H
