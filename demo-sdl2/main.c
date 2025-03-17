#include <SDL.h>
//#include <SDL_image.h>
#include <SDL_timer.h>

extern void reader();

void drawMemory(SDL_Renderer *renderer);
void drawFile(SDL_Renderer *renderer);
extern void drawGif(SDL_Renderer *renderer);

void draw(SDL_Renderer *renderer) {
//    reader();
    drawGif(renderer);
//	drawMemory(renderer);
//	drawFile(renderer);
}

// kill main that might have been defined in SDL_main
#undef main

int main(int argc, char *argv[])
{
    printf("getting\n");
    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 2;
    }
    printf("sdl init\n");

    SDL_Window *window = NULL;
    SDL_Texture *texture = NULL;
    SDL_Renderer *renderer = NULL;

//    SDL_Window* win = SDL_CreateWindow("GAME",
//                                       SDL_WINDOWPOS_CENTERED,
//                                       SDL_WINDOWPOS_CENTERED,
//                                       1000, 1000, 0);
    window = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (window == 0){
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    printf("window created\n");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == 0){
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    draw(renderer);

    SDL_RenderPresent(renderer);

    SDL_Event e;
    int quit = 0;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = 1;
            }
            if (e.type == SDL_KEYDOWN){
                quit = 1;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = 1;
            }
        }
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void drawMemory(SDL_Renderer *renderer) {
    static uint16_t pix[] = {
        1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
        1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
        1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
        1, 1, 1, 0, 0, 0, 0, 2, 2, 2,
        1, 1, 1, 0, 0, 0, 0, 2, 2, 2,
        2, 2, 2, 0, 0, 0, 0, 1, 1, 1,
        2, 2, 2, 0, 0, 0, 0, 1, 1, 1,
        2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
    };
    static uint32_t pct[4] = { 0xffffffff, 0xff000000, 0x0000ffff, 0x000000ff };
    SDL_Rect rect;
    rect.x = 50;
    rect.y = 50;
    rect.w = 10;
    rect.h = 10;

    SDL_RenderClear(renderer);
    for (uint8_t y=0; y<10; y++) {
        for (uint8_t x=0; x<10; x++) {
            rect.x = x * 10 + 100;
            rect.y = y * 10 + 100;
            uint16_t i = y * 10 + x;
            uint32_t c = pct[pix[i]];
            uint8_t r = (c >> 24) & 0xff;;
            uint8_t g = (c >> 16) & 0xff;
            uint8_t b = (c >> 8) & 0xff;
            uint8_t a = c & 0xff;
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void drawFile(SDL_Renderer *renderer) {
//    std::string imagePath = getResourcePath("Lesson1") + "hello.bmp";
    SDL_Surface *bmp = SDL_LoadBMP("./sample.bmp");
    if (bmp == 0){
//        SDL_DestroyRenderer(ren);
//        SDL_DestroyWindow(win);
        printf("SDL_LoadBMP Error: %s\n", SDL_GetError());
//        SDL_Quit();
        return;
    }
    printf("binmap loaded\n");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, bmp);
    SDL_FreeSurface(bmp);
    if (texture == 0){
//        SDL_DestroyRenderer(ren);
//        SDL_DestroyWindow(win);
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
//        SDL_Quit();
        return;
    }

    SDL_Texture *man = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 640, 480);
    if (man == 0){
//        SDL_DestroyRenderer(ren);
//        SDL_DestroyWindow(win);
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
//        SDL_Quit();
        return;
    }

#ifdef DIRECT_DRAW
    void *pixels;
    int pitch;
    int st = SDL_LockTexture(man, NULL, &pixels, &pitch);
    if (st != 0){
//        SDL_DestroyRenderer(ren);
//        SDL_DestroyWindow(win);
        printf("SDL_LockTextture Error: %s\n", SDL_GetError());
//        SDL_Quit();
        return;
    }
    uint32_t *a = pixels;
    printf("size of %lu\n", sizeof(a[0]));
    for (int i=0; i<4000; i++) {
        a[i] = 0x20ff80ff;
    }
    SDL_UnlockTexture(man);
#endif

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    printf("update screen\n");
}
