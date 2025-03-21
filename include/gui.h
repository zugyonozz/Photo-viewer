// gui.h
// date created 16-03-2025
// author: zugyonozz
#ifndef GUI_H
#define GUI_H

#include <string>
#include <iostream>
#include <vector>
#include <algorithm> // For std::min
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"

struct BtnTexture {
    SDL_Texture* normal;
    SDL_Texture* hover;
};

struct Img {
    SDL_Texture* texture;
    int W, H;
};

// Define the GUI Elements structure
struct GUIElements {
    std::vector<SDL_Texture*> containers;   // Container list
    std::vector<BtnTexture> buttons;        // Button list
    std::vector<Img> images;                // Image List (renamed from Img for clarity)
    std::vector<SDL_Rect> containerRects;   // Container Rect list
    std::vector<SDL_Rect> btnHoverRects;    // Hovering Button Rect list
    std::vector<SDL_Rect> btnRects;         // Button Rect list
    std::vector<SDL_Rect> imgRects;         // Image Rect list
};

extern bool Run;
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern int W;
extern int H;
extern size_t idShow;
extern GUIElements gui;  // Now this will work since GUIElements is fully defined

void loadQueueImg(int argc, char* argv[]);
void renderImg(int Nav = 2); // 0 = prev, 1 = next, 2 = maintain current
void imgLayout();
void initGUI(int initialWidth = 800, int initialHeight = 600);
void destroyGUI();
void updateLayout();
void renderGUI(int mx, int my);
void handleButtonClick(int x, int y);
void showGUI();

#endif // GUI_H