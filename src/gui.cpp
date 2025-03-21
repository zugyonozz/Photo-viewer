// GUI.cpp
#include "gui.h"

// ##################################################### START INIT ####################################################

// GLOBAL VARIABEL
int W = 800;                          // Default window size
int H = 600;                          // Default window size
size_t idShow = 0;                    // Index of currently displayed image (initialized to 0)
bool Run = true;                      // Program running flag
SDL_Window* window = nullptr;         // Window initialization
SDL_Renderer* renderer = nullptr;     // Renderer initialization
TTF_Font* font = nullptr;             // Font initialization
GUIElements gui;                      // Initialize the GUI elements structure
Uint32 lastResizeTime = 0;            // For throttling resize events

// Initialization of SDL, SDL_ttf libraries, window creation, Load Resources and Layout
void initGUI(int initialWidth, int initialHeight) {
    W = initialWidth;
    H = initialHeight;
    // Initialize SDL subsystems with error checking
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL initialization failed: %s", SDL_GetError());
        return;
    }
    if (TTF_Init() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF initialization failed: %s", TTF_GetError());
        SDL_Quit();
        return;
    }
    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_image initialization failed: %s", IMG_GetError());
        TTF_Quit();
        SDL_Quit();
        return;
    }
    // Load font
    font = TTF_OpenFont("../assets/fonts/NotoSans-Regular.ttf", 24);
    if (!font) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load font: %s", TTF_GetError());
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return;
    }
    // Create window
    window = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 
                             SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
        TTF_CloseFont(font);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return;
    }
    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return;
    }
    // Set window hit test callback for drag and resize
    if (SDL_SetWindowHitTest(window, hitTestCallback, nullptr) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to set hit test callback: %s", SDL_GetError());
    }
    // Get the actual window size (might be different from requested)
    SDL_GetWindowSize(window, &W, &H);
    // Load textures
    loadTextures();
    // Initialize layout
    updateLayout();
    SDL_Log("GUI initialized successfully: %d x %d", W, H);
}

// Initialize Resources and Load Textures
void loadTextures() {
    // Load container textures
    gui.containers.push_back(createTextureFromImage("../assets/imgs/Primary.png"));
    gui.containers.push_back(createTextureFromImage("../assets/imgs/Secondary.png"));
    // Error check for container textures
    for (size_t i = 0; i < gui.containers.size(); i++) {
        if (!gui.containers[i]) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load container texture %zu", i);
        }
    }
    // Load button textures
    gui.buttons.push_back({createTextureFromImage("../assets/imgs/MinimizeBtn.png"), createTextureFromImage("../assets/imgs/Hov1.png")});
    gui.buttons.push_back({createTextureFromImage("../assets/imgs/MaximizeBtn.png"), createTextureFromImage("../assets/imgs/Hov1.png")});
    gui.buttons.push_back({createTextureFromImage("../assets/imgs/CloseBtn.png"), createTextureFromImage("../assets/imgs/Hov2.png")});
    gui.buttons.push_back({createTextureFromImage("../assets/imgs/PrevBtn.png"), createTextureFromImage("../assets/imgs/PrevBtnHov.png")});
    gui.buttons.push_back({createTextureFromImage("../assets/imgs/NextBtn.png"), createTextureFromImage("../assets/imgs/NextBtnHov.png")});
    // Error check for button textures
    for (size_t i = 0; i < gui.buttons.size(); i++) {
        if (!gui.buttons[i].normal || !gui.buttons[i].hover) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load button texture %zu", i);
        }
    }
    SDL_Log("Textures loaded");
}

// Initialize Image List from Input Arguments
void loadQueueImg(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "No image files provided!" << std::endl;
        return;
    }
    // Track if we successfully loaded any images
    bool imagesLoaded = false;
    for (int i = 1; i < argc; i++) {
        if (isValidImage(argv[i])) {
            addQueueImage(argv[i]);
            imagesLoaded = true;
            SDL_Log("Successfully loaded image: %s", argv[i]);
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Not a valid image file: %s", argv[i]);
        }
    }   
    // Only update the layout if we have images
    if (imagesLoaded && !gui.images.empty()) {
        imgLayout();
    }
}
// ###################################################### END INIT #####################################################

// ####################################################	START PROCESS ##################################################

// Create Texture from image to add to queue
void addQueueImage(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image %s: %s", path, IMG_GetError());
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture %s: %s", path, SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }
    // Store original dimensions for calculations
    Img img = {texture, surface->w, surface->h};
    gui.images.push_back(img);
    SDL_FreeSurface(surface);
    SDL_Log("Successfully loaded image: %s", path);
}

// Create Texture from Image
SDL_Texture* createTextureFromImage(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image %s: %s", path.c_str(), IMG_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }
    return texture;
}

// Create Texture from Text
SDL_Texture* createTextureFromText(const std::string& text, SDL_Color color) {
    if (!font) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Font not initialized");
        return nullptr;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to render text: %s", TTF_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }   
    return texture;
}

// Validation Extension file images
bool isValidImage(const char* filename) {
    const char* validExtensions[] = {".png", ".jpg", ".jpeg", ".bmp", ".gif", ".webp"};
    size_t len = strlen(filename);
    for (const char* ext : validExtensions) {
        size_t extLen = strlen(ext);
        if (len >= extLen && strcasecmp(filename + (len - extLen), ext) == 0) return true;
    }
    return false;
}

// Position of Elements and Hovering Areas
void updateLayout() {
    // Update container rectangles
    gui.containerRects = {
        {0, 0, W, H - 32},       		// Main container
        {0, H - 32, W, 32}       		// Bottom bar
    };
    // Update button hover rectangles
    gui.btnHoverRects = {
        {W - 96, 0, 32, 32},     		// Minimize hover
        {W - 64, 0, 32, 32},     		// Maximize hover
        {W - 32, 0, 32, 32},     		// Close hover
        {32, H / 2 - 8, 16, 16},	 	// Prev Hovered button
        {W - 32, H / 2 - 8, 16, 16}		// Next Hovered button
    };    
    // Update button rectangles
    gui.btnRects = {
        {W - 84, 12, 8, 8},				// Minimize button
        {W - 52, 12, 8, 8},				// Maximize button
        {W - 20, 12, 8, 8},				// Close button
        {32, H / 2 - 8, 16, 16},		// Prev button
        {W - 32, H / 2 - 8, 16, 16}		// Next button
    };
    SDL_Log("Layout updated: %d x %d", W, H);
}

// Fitting Image to Container and Centering it
void imgLayout() {
    // Clear previous image rects to avoid duplicates when resizing
    gui.imgRects.clear();
    // Check if we have images to display
    if (gui.images.empty()) return;
    // Calculate fitting dimensions for each image
    for (size_t i = 0; i < gui.images.size(); i++) {
        int imgW = gui.images[i].W;
        int imgH = gui.images[i].H;
        // Calculate the maximum available space for the image
        int maxWidth = W - 40;  // 20px padding on each side
        int maxHeight = H - 96; // Accounting for top and bottom bars + padding
        // Ensure we don't divide by zero
        if (imgW <= 0 || imgH <= 0 || maxWidth <= 0 || maxHeight <= 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid image or window dimensions");
            continue;
        }
        // Calculate scale to fit while maintaining aspect ratio
        float scaleW = static_cast<float>(maxWidth) / imgW;
        float scaleH = static_cast<float>(maxHeight) / imgH;
        float scale = std::min(scaleW, scaleH);
        // Calculate final dimensions
        int finalWidth = static_cast<int>(imgW * scale);
        int finalHeight = static_cast<int>(imgH * scale);
        // Center the image
        int x = (W - finalWidth) / 2;
        int y = ((H - 32) - finalHeight) / 2 + 16; // Centered in main area, below top controls
        // Create and store the rect
        SDL_Rect rect = {x, y, finalWidth, finalHeight};
        gui.imgRects.push_back(rect);       
        SDL_Log("Image %zu layout: (%d, %d, %d, %d)", i, rect.x, rect.y, rect.w, rect.h);
    }
}

// Button Events
void handleButtonClick(int x, int y) {
    for (size_t i = 0; i < gui.btnRects.size(); i++) {
        const SDL_Rect& btnRect = gui.btnRects[i];
        if (x >= btnRect.x && x <= btnRect.x + btnRect.w && y >= btnRect.y && y <= btnRect.y + btnRect.h){
            switch (i) {
                case 0: SDL_MinimizeWindow(window); break; // Minimize
				// Toggle Fullscreen
                case 1: {
                        Uint32 flags = SDL_GetWindowFlags(window);
                        (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) ? SDL_SetWindowFullscreen(window, 0) : SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        // Update layout after fullscreen change
                        SDL_GetWindowSize(window, &W, &H);
                        updateLayout();
                        imgLayout();
                    }
                    break;
                case 2: Run = false; break; // Close
                case 3: if(!gui.images.empty()) renderImg(0); break; // Previous
                case 4: if(!gui.images.empty()) renderImg(1); break; // Next
            }
            break; // Exit the loop after handling one button
        }
    }
}

// Resize Window
SDL_HitTestResult hitTestCallback(SDL_Window* win, const SDL_Point* area, void* data) {
    int w, h;
    SDL_GetWindowSize(win, &w, &h);
	// Size of the edge area for resizing
    int borderSize = 8;
    // Check if cursor is on the edge for resizing
    if (area->y < borderSize) {
        if (area->x < borderSize) return SDL_HITTEST_RESIZE_TOPLEFT;
        if (area->x > w - borderSize) return SDL_HITTEST_RESIZE_TOPRIGHT;
        return SDL_HITTEST_RESIZE_TOP;
    }
    if (area->y > h - borderSize) {
        if (area->x < borderSize) return SDL_HITTEST_RESIZE_BOTTOMLEFT;
        if (area->x > w - borderSize) return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
        return SDL_HITTEST_RESIZE_BOTTOM;
    }
    if (area->x < borderSize) return SDL_HITTEST_RESIZE_LEFT;
    if (area->x > w - borderSize) return SDL_HITTEST_RESIZE_RIGHT;
    // If at the top, consider it as title bar for dragging (except near buttons)
    if (area->y < 30 && area->x < (w - 100)) return SDL_HITTEST_DRAGGABLE;
    return SDL_HITTEST_NORMAL;
}

// destroy Resources and Inputs
void destroyGUI() { 
    // Free image textures
    for (auto& img : gui.images) {
        if (img.texture) {
            SDL_DestroyTexture(img.texture);
            img.texture = nullptr;
        }
    }
    gui.images.clear();
    // Free container textures
    for (auto& texture : gui.containers) {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
    gui.containers.clear();
    // Free button textures
    for (auto& button : gui.buttons) {
        if (button.normal) {
            SDL_DestroyTexture(button.normal);
            button.normal = nullptr;
        }
        if (button.hover) {
            SDL_DestroyTexture(button.hover);
            button.hover = nullptr;
        }
    }
    gui.buttons.clear();
    // Free resources
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    // Quit subsystems
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    SDL_Log("GUI destroyed successfully");
}

// ##################################################### END PROCESS ####################################################

// ####################################################	START RENDER ####################################################


// Render Container
void renderContainer(int mx, int my) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);   
    // Render containers
    for (size_t i = 0; i < gui.containers.size() && i < gui.containerRects.size(); i++) {
        if (gui.containers[i]) SDL_RenderCopy(renderer, gui.containers[i], nullptr, &gui.containerRects[i]);
    }
}

// Render Buttons and Hover Effects
void renderBtn(int mx, int my){
	// Render buttons with hover effects
    for (size_t i = 0; i < gui.btnRects.size() && i < gui.buttons.size(); i++) {
        const SDL_Rect& btnRect = gui.btnRects[i];
        const SDL_Rect& hoverRect = gui.btnHoverRects[i];
        // Check if mouse is over button
        bool isHovered = (mx >= btnRect.x && mx <= btnRect.x + btnRect.w && my >= btnRect.y && my <= btnRect.y + btnRect.h);
        // Render hover effect if needed
        if (isHovered && gui.buttons[i].hover) SDL_RenderCopy(renderer, gui.buttons[i].hover, nullptr, &hoverRect);
        // Render button
        if (gui.buttons[i].normal) SDL_RenderCopy(renderer, gui.buttons[i].normal, nullptr, &btnRect);
    }
}

// Render Current Image
void renderImg(int Nav) { // 0 = prev, 1 = next, 2 = init
    // Check if there are any images to display
    if (gui.images.empty() || gui.imgRects.empty()) return;
    // Update the current image index based on navigation
    if (Nav == 2) {
        // Keep current image or initialize to first image
        idShow = (idShow < gui.images.size()) ? idShow : 0;
    } else if (Nav == 0) { // Previous
        idShow = (idShow > 0) ? idShow - 1 : gui.images.size() - 1;
    } else if (Nav == 1) { // Next
        idShow = (idShow < gui.images.size() - 1) ? idShow + 1 : 0;
    }   
    // Display the current image
    if (idShow < gui.images.size() && idShow < gui.imgRects.size()) SDL_RenderCopy(renderer, gui.images[idShow].texture, nullptr, &gui.imgRects[idShow]);
}

// Show GUI
void showGUI() {
    SDL_RenderPresent(renderer);
}
// ##################################################### END RENDER ####################################################