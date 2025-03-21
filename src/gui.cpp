// main.cpp
// date created 16-03-2025
// author: zugyonozz
// GUI.cpp
#include "gui.h"

int W = 800;							// Default window size
int H = 600;							// Default window size
size_t idShow;							// Index Gambar yang di tampilkan
bool Run = true;						// Parameter jalan program
SDL_Window* window = nullptr;			// inisialisasi Window
SDL_Renderer* renderer = nullptr;		// inisialisasi Renderer
TTF_Font* font = nullptr;				// inisialisasi Font

struct GUIElements {						// Struktur Element GUI
    std::vector<SDL_Texture*> containers;	// Deklarasi Container list
    std::vector<BtnTexture> buttons;		// Deklarasi Button list
	std::vector<Img> Img;					// Deklarasi Image List
    std::vector<SDL_Rect> containerRects;	// Deklarasi Container Rect list
    std::vector<SDL_Rect> btnHoverRects;	// Deklarasi Hovering Button Rect list
    std::vector<SDL_Rect> btnRects;			// Deklarasi Button Rect list
	std::vector<SDL_Rect> imgRects;			// Deklarasi imgRect list
} gui;



void addQueueImage(const char* path) {  // Ubah void* jadi void
    Img img;
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image %s: %s", path, IMG_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture %s: %s", path, IMG_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    img = {texture, surface->w, surface->h};
    gui.Img.push_back(img);
    SDL_FreeSurface(surface);
    SDL_Log("Successfully loaded image: %s", path);
}

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

SDL_Texture* createTextureFromText(const std::string& text, SDL_Color color) {
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

bool isValidImage(const char* filename) {
    const char* validExtensions[] = {".png", ".jpg", ".jpeg", ".bmp", ".gif"};
    size_t len = strlen(filename);

    for (const char* ext : validExtensions) {
        size_t extLen = strlen(ext);
        if (len >= extLen && strcmp(filename + (len - extLen), ext) == 0) {
            return true;
        }
    }
    return false;
}

void loadQueueImg(int argc, char* argv[]) {  // Tambahkan argc
    if (argc < 2) {
        std::cerr << "Tidak ada file yang diberikan!" << std::endl;
        return;
    }

    for (int i = 1; i < argc; i++) {  // Gunakan argc untuk iterasi
        if (isValidImage(argv[i])) {
            addQueueImage(argv[i]);
            SDL_Log("Berhasil Memuat Gambar: %s", argv[i]);
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "File bukan gambar yang valid: %s", argv[i]);
        }
    }
}

void loadTextures() {
    // Load container textures
    gui.containers.push_back(createTextureFromImage("../assets/imgs/Primary.png"));
    gui.containers.push_back(createTextureFromImage("../assets/imgs/Secondary.png"));
    
    // Load button textures
    gui.buttons.push_back({createTextureFromImage("../assets/imgs/MinimizeBtn.png"), createTextureFromImage("../assets/imgs/Hov1.png")});
    gui.buttons.push_back({createTextureFromImage("../assets/imgs/MaximizeBtn.png"), createTextureFromImage("../assets/imgs/Hov1.png")});
    gui.buttons.push_back({createTextureFromImage("../assets/imgs/CloseBtn.png"), createTextureFromImage("../assets/imgs/Hov2.png")});
	gui.buttons.push_back({createTextureFromImage("../assets/imgs/PrevBtn.png"), createTextureFromImage("../assets/imgs/Hov1.png")});
	gui.buttons.push_back({createTextureFromImage("../assets/imgs/NextBtn.png"), createTextureFromImage("../assets/imgs/Hov1.png")});
    
    SDL_Log("Textures loaded successfully");
}

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
    window = SDL_CreateWindow("1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN| SDL_WINDOW_RESIZABLE);
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
    
    // Get the actual window size
    SDL_GetWindowSize(window, &W, &H);
    
    // Load textures
    loadTextures();
    
    // Initialize layout
    updateLayout();
    imgLayout();
    SDL_Log("GUI initialized successfully");
}

void updateLayout() {
    // Update container rectangles
    gui.containerRects = {
        {0, 0, W, H - 32},       // Main container
        {0, H - 32, W, 32}       // Bottom bar
    };
    
    // Update button hover rectangles
    gui.btnHoverRects = {
        {W - 96, 0, 32, 32},     // Minimize hover
        {W - 64, 0, 32, 32},     // Maximize hover
        {W - 32, 0, 32, 32},      // Close hover
		{W/2 - 32, 0, 32, 32},     // Prev Hovered button
		{W/2, 0, 32, 32}		 // Next Hovered button
    };
    
    // Update button rectangles
    gui.btnRects = {
        {W - 84, 12, 8, 8},      // Minimize button
        {W - 52, 12, 8, 8},      // Maximize button
        {W - 20, 12, 8, 8},      // Close button
		{W/2 - 20, 12, 8, 8},     // Prev button
		{W/2 + 12, 12, 8, 8}		 // Next button
    };
    
    SDL_Log("Layout updated: %d x %d", W, H);
}

void imgLayout(){
	for(size_t i = 0; i < gui.Img.size(); i++){
		int fitScale = gui.Img[i].H / (H - 96);
		SDL_Rect rect = {W/2 - gui.Img[i].W, 32, gui.Img[i].W / fitScale, gui.Img[i].H / fitScale};
		gui.imgRects.push_back(rect);
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Successfullty Fitting Image (%i, %i, %i, %i)", rect.x, rect.y, rect.w, rect.h);
	}
}

void renderGUI(int mx, int my) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    
    // Render containers
    for (size_t i = 0; i < gui.containers.size(); i++) {
        SDL_RenderCopy(renderer, gui.containers[i], nullptr, &gui.containerRects[i]);
    }
    
    // Render buttons with hover effects
    for (size_t i = 0; i < gui.btnRects.size(); i++) {
        const SDL_Rect& btnRect = gui.btnRects[i];
        
        // Check if mouse is over button
        bool isHovered = (mx >= btnRect.x && mx <= btnRect.x + btnRect.w && my >= btnRect.y && my <= btnRect.y + btnRect.h);
        
        // Render hover effect if needed
        if (isHovered) {
            SDL_RenderCopy(renderer, gui.buttons[i].hover, nullptr, &gui.btnHoverRects[i]);
        }
        
        // Render button
        SDL_RenderCopy(renderer, gui.buttons[i].normal, nullptr, &btnRect);
    }
}

void renderImg(int Nav){ // 0 = prev, 1 = next, 2 = init
	if(Nav == 2){
		idShow = 0;
	}else if(Nav == 0 && idShow > 0){
		idShow++;
	}else if(Nav == 1 && idShow < gui.Img.size() - 1){
		idShow++;
	}else if((Nav && idShow) == 0){
		idShow = gui.Img.size() - 1;
	}else{
		idShow = 0;
	}

	SDL_RenderCopy(renderer, gui.Img[idShow].texture, nullptr, &gui.imgRects[idShow]);
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Successfullty Show Image");
}

void handleButtonClick(int x, int y) {
    for (size_t i = 0; i < gui.btnRects.size(); i++) {
        const SDL_Rect& btnRect = gui.btnRects[i];
        
        if (x >= btnRect.x && x <= btnRect.x + btnRect.w && 
            y >= btnRect.y && y <= btnRect.y + btnRect.h) {
            
            switch (i) {
                case 0: // Minimize
                    SDL_MinimizeWindow(window);
                    break;
                    
                case 1: // Toggle Fullscreen
                    {
                        Uint32 flags = SDL_GetWindowFlags(window);
                        if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
                            SDL_SetWindowFullscreen(window, 0);
                        } else {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
                    }
                    break;
                    
                case 2: // Close
                    Run = false;
                    break;
				case 3:
					renderImg(0); // prev
					break;
				case 4: // next
					renderImg(1);
					break;
            }
            
            break; // Exit the loop after handling one button
        }
    }
}

void showGUI() {
    SDL_RenderPresent(renderer);
}

void destroyGUI() {
    // Free textures
    for (auto& texture : gui.containers) {
        SDL_DestroyTexture(texture);
    }
    
    for (auto& button : gui.buttons) {
        SDL_DestroyTexture(button.normal);
        SDL_DestroyTexture(button.hover);
    }
    
    // Free resources
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    // Quit subsystems
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    SDL_Log("GUI destroyed successfully");
}