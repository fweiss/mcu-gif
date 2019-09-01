#include <SDL.h>
//#include <SDL_image.h>
#include <SDL_timer.h>

int main(int argc, char *argv[])
{
    printf("getting\n");
    // retutns zero on success else non-zero
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 2;
    }
    printf("sdl init\n");
//    SDL_Window* win = SDL_CreateWindow("GAME",
//                                       SDL_WINDOWPOS_CENTERED,
//                                       SDL_WINDOWPOS_CENTERED,
//                                       1000, 1000, 0);
    SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (win == 0){
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    printf("window created\n");
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == 0){
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
//    std::string imagePath = getResourcePath("Lesson1") + "hello.bmp";
    SDL_Surface *bmp = SDL_LoadBMP("./sample.bmp");
    if (bmp == 0){
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        printf("SDL_LoadBMP Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    printf("binmap loaded\n");
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);
    if (tex == 0){
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Texture *man = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 640, 480);
    if (man == 0){
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    void *pixels;
    int pitch;
    int st = SDL_LockTexture(man, NULL, &pixels, &pitch);
    if (st != 0){
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        printf("SDL_LockTextture Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    uint32_t *a = pixels;
    printf("size of %lu\n", sizeof(a[0]));
    for (int i=0; i<4000; i++) {
        a[i] = 0x20ff80ff;
    }
    SDL_UnlockTexture(man);


    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, man, NULL, NULL);
    printf("update screen\n");
    SDL_RenderPresent(ren);

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
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
