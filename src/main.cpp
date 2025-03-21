// main.cpp
// date created 16-03-2025
// author: zugyonozz
#define SDL_MAIN_HANDLED
#include "gui.h"

int main(int argc, char* argv[]) {
    SDL_Event e;
    bool isClick = false;
    int mx = 0, my = 0;
    
    // Initialize GUI with default window size
    initGUI(800, 600);
    loadQueueImg(argc, argv);
    
    // Initial rendering of image if available
    if (!gui.images.empty()) {
        renderImg(2);  // Initialize with first image
    }
    
    while (Run) {
        // Handle events with 16ms timeout (approx. 60 FPS)
        if (SDL_WaitEventTimeout(&e, 16)) {
            switch (e.type) {
                case SDL_QUIT:
                    Run = false;
                    break;
                    
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        Run = false;
                    } else if (e.key.keysym.sym == SDLK_LEFT) {
                        renderImg(0);  // Previous image
                    } else if (e.key.keysym.sym == SDLK_RIGHT) {
                        renderImg(1);  // Next image
                    }
                    break;
                    
                case SDL_MOUSEMOTION:
                    mx = e.motion.x;
                    my = e.motion.y;
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    if (!isClick) {
                        handleButtonClick(e.button.x, e.button.y);
                        isClick = true;
                    }
                    break;
                    
                case SDL_MOUSEBUTTONUP:
                    isClick = false;
                    break;
                    
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || 
                        e.window.event == SDL_WINDOWEVENT_RESIZED) {
                        SDL_GetWindowSize(window, &W, &H);
                        updateLayout();
                        imgLayout();
                    }
                    break;
            }
        }
        
        // Render GUI and current image
        renderGUI(mx, my);
        
        // Only render image if images are loaded
        if (!gui.images.empty()) {
            renderImg(2);  // Use 2 to maintain current image
        }
        
        showGUI();
    }
    
    destroyGUI();
    return 0;
}