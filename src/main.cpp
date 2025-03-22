// main.cpp
// date created 16-03-2025
// author: zugyonozz
// optimized: 22-03-2025
#define SDL_MAIN_HANDLED			// Prevent SDL from overriding main
#include "gui.h"					// GUI functionality

int main(int argc, char* argv[]) {
    SDL_Event e;					// Event handler
    bool isClick = false;			// Flag to indicate if a mouse click is detected
    int mx = 0, my = 0;				// Mouse coordinates
    bool pendingResize = false;		// Flag to indicate if a resize event is pending
    // Initialize GUI with default window size
    initGUI(800, 600);
    // Set window hit test callback for dragging and resizing
    if (SDL_SetWindowHitTest(window, hitTestCallback, nullptr) != 0) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to set hit test callback: %s", SDL_GetError());
    // Set window as resizable
    SDL_SetWindowResizable(window, SDL_TRUE);
    // Load images from command-line arguments
    loadQueueImg(argc, argv);
    // Initial rendering of image if available
    if (!gui.images.empty()) renderImg(2); // Initialize with first image
    while (Run) {
        // Handle events with 16ms timeout (approx. 60 FPS)
        if (SDL_WaitEventTimeout(&e, 16)) {
            switch (e.type) {
                case SDL_QUIT: Run = false; break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        Run = false;
                    } else if (e.key.keysym.sym == SDLK_LEFT && !gui.images.empty()) {
                        renderImg(0);  // Previous image
                    } else if (e.key.keysym.sym == SDLK_RIGHT && !gui.images.empty()) {
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
                case SDL_MOUSEBUTTONUP: isClick = false; break;
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        // Mark that we need to process a resize, but don't do it immediately
                        pendingResize = true;
                        lastResizeTime = SDL_GetTicks();
                    }
                    break;
            }
        }
        // Handle any pending resize after the delay to avoid excessive calculations
        if (pendingResize && SDL_GetTicks() - lastResizeTime > RESIZE_DELAY) {
            // Get updated window size
            SDL_GetWindowSize(window, &W, &H);
            SDL_Log("Window resized to %d x %d", W, H);
            // Update layout for the new size
            updateLayout();
            imgLayout();   
            pendingResize = false;
        }
        // Render GUI and current image
        renderContainer(mx, my);
        // Only render image if images are loaded
        if (!gui.images.empty()) {
            renderImg(2);  // Use 2 to maintain current image
        }
		renderBtn(mx, my);
        showGUI();
    }
    destroyGUI();
    return 0;
}