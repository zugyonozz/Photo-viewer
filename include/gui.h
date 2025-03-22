// gui.h
// date created 16-03-2025
// author: zugyonozz
// optimized: 22-03-2025
#ifndef GUI_H
#define GUI_H

#include <cstring>																		// For std::memcpy
#include <iostream>																		// For std::cout and std::cerr
#include <vector>																		// For std::vector
#include <algorithm>																	// For std::min
#include <memory>																		// For smart pointers
#include "SDL2/SDL.h"																	// For SDL functions
#include "SDL2/SDL_ttf.h"																// For TTF functions
#include "SDL2/SDL_image.h"																// For IMG functions

// Define btn texture 
struct BtnTexture {
    SDL_Texture* normal;																// Normal texture
    SDL_Texture* hover;																	// Hovered texture
};

// Define the Img 
struct Img {
    SDL_Texture* texture;																// Normal texture
    int W, H;																			// Width and Height of the image
};

// Define the GUI Elements structure
struct GUIElements {
    std::vector<SDL_Texture*> containers;   											// Container list
    std::vector<BtnTexture> buttons;        											// Button list
    std::vector<Img> images;                											// Image List
    std::vector<SDL_Rect> containerRects;   											// Container Rect list
    std::vector<SDL_Rect> btnHoverRects;    											// Hovering Button Rect list
    std::vector<SDL_Rect> btnRects;         											// Button Rect list
    std::vector<SDL_Rect> imgRects;         											// Image Rect list
};

// Application state
extern bool Run;																		// Main loop control variable
extern SDL_Window* window;																// Window for rendering
extern SDL_Renderer* renderer;															// Renderer for drawing
extern TTF_Font* font;																	// Font for rendering text
extern int W;																			// Width Window dimensions
extern int H;																			// Height Window dimensions
extern size_t idShow;																	// Current image index
extern GUIElements gui;																	// GUI elements
extern Uint32 lastResizeTime;															// For throttling resize events
constexpr Uint32 RESIZE_DELAY = 100;													// Milliseconds to wait before processing resize

// Hit test callback for window dragging and resizing
SDL_HitTestResult hitTestCallback(SDL_Window* win, const SDL_Point* area, void* data);	// For window dragging and resizing

// Image management
void loadQueueImg(int argc, char* argv[]);												// Load images from command line arguments
void addQueueImage(const char* path);													// Add an image to the queue
bool isValidImage(const char* filename);												// Check if a file is a valid image
void imgLayout(); 																		// Update image layout based on window size
void renderImg(int Nav = 2); 															// 0 = prev, 1 = next, 2 = init images for first image showed

// GUI management
void initGUI(int initialWidth = 800, int initialHeight = 600);							// Initialize GUI elements
void loadTextures();																	// Load textures for GUI elements
void updateLayout();																	// update GUI elements layout based on window size
void handleButtonClick(int x, int y);													// Handle button click events
void renderContainer(int mx, int my);													// Render container elements
void renderBtn(int mx, int my);															// Render button elements
void showGUI();																			// Render GUI elements
void destroyGUI();																		// Destroy Resources, and free memory

// Resource creation helpers
SDL_Texture* createTextureFromImage(const std::string& path);							// Create texture from image file
SDL_Texture* createTextureFromText(const std::string& text, SDL_Color color);			// Create texture from text

#endif // GUI_H