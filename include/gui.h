// gui.h
// date created 16-03-2025
// author: zugyonozz
// optimized: 22-03-2025
#ifndef GUI_H
#define GUI_H

#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm> // For std::min
#include <memory>    // For smart pointers
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
    std::vector<Img> images;                // Image List
    std::vector<SDL_Rect> containerRects;   // Container Rect list
    std::vector<SDL_Rect> btnHoverRects;    // Hovering Button Rect list
    std::vector<SDL_Rect> btnRects;         // Button Rect list
    std::vector<SDL_Rect> imgRects;         // Image Rect list
};

// Application state
extern bool Run;
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;
extern int W;
extern int H;
extern size_t idShow;
extern GUIElements gui;
extern Uint32 lastResizeTime;				// For throttling resize events
constexpr Uint32 RESIZE_DELAY = 100;	// Milliseconds to wait before processing resize

// Hit test callback for window dragging and resizing
SDL_HitTestResult hitTestCallback(SDL_Window* win, const SDL_Point* area, void* data);

// Image management
void loadQueueImg(int argc, char* argv[]);
void addQueueImage(const char* path);
bool isValidImage(const char* filename);
void renderImg(int Nav = 2); // 0 = prev, 1 = next, 2 = maintain current
void imgLayout();

// GUI management
void initGUI(int initialWidth = 800, int initialHeight = 600);
void destroyGUI();
void updateLayout();
void renderContainer(int mx, int my);
void renderBtn(int mx, int my);
void handleButtonClick(int x, int y);
void showGUI();
void loadTextures();

// Resource creation helpers
SDL_Texture* createTextureFromImage(const std::string& path);
SDL_Texture* createTextureFromText(const std::string& text, SDL_Color color);

#endif // GUI_H